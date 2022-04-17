#include "IOManager.h"
#include "client.h"

/*信号处理函数*/
void IOManager::sig_handler(int sig)
{
    /*保留原来的errno，在函数最后恢复，以保证函数的可重入性*/
    int save_errno = errno;
    int msg = sig;
    send(s_pipefd[1], (char*)&msg, 1, 0);/*将信号值写入管道，以通知主循环*/
    errno = save_errno;
}

int IOManager::setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void IOManager::addfd(int epollfd, int fd)
{
    epoll_event event;
    event.data.fd=fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

/*设置信号的处理函数*/
void IOManager::addsig(int sig)
{
    struct sigaction sa;
    memset(&sa,'\0',sizeof(sa));
    sa.sa_handler = IOManager::sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

// 监听进程的状态
void IOManager::ProcessMonitorHandler()
{
    const std::vector<LISTENPROCESSINFO>& vListenProcessInfo = GlobalConfig::GetInstance()->GetListeningProcessInfo();

    bool bStatusChange = false;
    for (auto data : vListenProcessInfo)
    {
        std::string sPath = GlobalConfig::GetInstance()->getListeningProcessPath();

        ifstream ifs;
        //1.打开文件，如果没有，会在同级目录下自动创建该文件
        ifs.open(sPath + "/" + data.pidFile, ios::in);
        string pid;
        ifs >> pid;

        ifs.close();

        int nPid = atoi(pid.c_str());
        uint16_t nStatus = 0;       //未启动
        if (nPid > 0)
        {
            if(kill(nPid, 0) == 0)
            {
                nStatus = 1;
            }
            else
            {
                nStatus = 0;
            }
        }


        if(m_listenProcessStatues.find(data.pidFile) != m_listenProcessStatues.end() && m_listenProcessStatues.find(data.pidFile)->second != nStatus)
        {
            m_listenProcessStatues.find(data.pidFile)->second = nStatus;
            bStatusChange = true;
        }
        else
        {
            m_listenProcessStatues.insert(pair<string, int>(data.pidFile, nStatus));
        }
    }

    if (bStatusChange)
    {
        // 推送给当前所有的客户端监听的进程状态改变
        for (auto it = m_mClients.begin(); it != m_mClients.end(); ++it)
        {
            it->second->OnSendCurrentProcessStatusInfo();
        }
    }

    // 每5s发送一次心跳包
    for (auto it = m_mClients.begin(); it != m_mClients.end(); ++it)
    {
        it->second->OnSendServerCurrentTimestamp();
    }
}

void IOManager::TimerHandler()
{
    timer_lst.tick();
    alarm(1);

    // ProcessMonitorHandler();
}

void IOManager::InitIOManager()
{
    // 创建epoll实例
    m_epollFd = epoll_create(5);
    assert(m_epollFd != -1);

    int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, s_pipefd);
    assert(ret != -1);

    setnonblocking(s_pipefd[1]);
    addfd(m_epollFd, s_pipefd[0]);

    /*设置一些信号的处理函数*/
    addsig(SIGHUP);
    addsig(SIGCHLD);
    addsig(SIGTERM);
    addsig(SIGINT);
    addsig(SIGALRM);
    addsig(SIGUSR1);
    addsig(SIGUSR2);

    alarm(1);
}

void IOManager::AddListeningFd(string ip, int port)
{
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &address.sin_addr);
    address.sin_port = htons(port);

    m_listenFd = socket(PF_INET, SOCK_STREAM, 0);
    assert(m_listenFd >= 0);
    
    // 设置监听的socket可以重用
    // 因为如果服务器退出的时候，断开所有的tcp连接，这时候客户端那边还没有返回四次挥手的确认断开ack，服务器直接关闭的话，会导致产生TIME_OUT状态的tcp连接
    // 一旦有TIME_OUT状态的连接存在，服务器重启是无法监听这个地址的
    const int on = 1;
    setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int ret = bind(m_listenFd, (struct sockaddr*)&address, sizeof(address));
    if(ret == -1)
    {
        LOG_ERROR("绑定监听的服务器ip和端口失败 :" + ip + ", " + std::to_string(port));
        return;
    }

    ret = listen(m_listenFd, 5);

    addfd(m_epollFd,m_listenFd);

    LOG_INFO("绑定监听的服务器ip和端口成功 :" + ip + ", " + std::to_string(port));
}

void IOManager::OnClientDisconnect(int sockfd)
{
    epoll_ctl(m_epollFd, EPOLL_CTL_DEL, sockfd, NULL);

    std::map<int, Client*>::iterator it = m_mClients.find(sockfd);
    if (it != m_mClients.end() )
    {
        Client* client = it->second;
        client->OnDisconnect();
        delete client;

        shutdown(sockfd, SHUT_RDWR);
        m_mClients.erase(it);
    }
}

// 退出事件循环，释放资源，断开所有客户端链接
void IOManager::Exit()
{
    for (auto it = m_mClients.begin(); it != m_mClients.end(); ++it)
    {
        // 在epoll中删除掉监听的文件描述符
        epoll_ctl(m_epollFd, EPOLL_CTL_DEL, it->first, NULL);
        // 客户端断开链接
        it->second->OnDisconnect();
        shutdown(it->first, SHUT_RDWR);  
    }

    m_mClients.clear();
}

// 增加定时器
void IOManager::AddTimer(util_timer* timer)
{
    timer_lst.add_timer(timer);
}

void IOManager::Loop()
{
    LOG_INFO("服务启动完毕，开启事件循环");
    loop = true;
    bool timeout = false;
    while(loop)
    {
        // 等待事件的产生（阻塞等待）
        int number = epoll_wait(m_epollFd, m_events, MAX_EVENT_NUMBER, -1);
        for (int i = 0; i < number; ++i)
        {
            int sockfd = m_events[i].data.fd;
            if (sockfd == m_listenFd)
            {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
                int connfd = accept(m_listenFd, (struct sockaddr*)
                    &client_address, &client_addrlength);
                addfd(m_epollFd, connfd);

                Client* client = new Client();
                if (client)
                {
                    m_mClients.insert(pair <int,Client*> (connfd, client));
                    client->OnConnect(client_address, connfd);

                    // client->OnSendFileTreeInfoToClient();
                }
            }
            // 信号事件
            else if (sockfd == s_pipefd[0] && (m_events[i].events&EPOLLIN))
            {
                int sig;
                char signals[1024];
                int ret = recv(s_pipefd[0], signals, sizeof(signals),0);

                if (ret == -1)
                {
                    continue;
                }
                else if(ret == 0)
                {
                    continue;
                }
                else
                {
                    for (int i = 0; i < ret; ++i)
                    {
                        switch(signals[i])
                        {
                            case SIGALRM:
                            {
                                timeout = true;
                                continue;
                            }
                            case SIGCHLD:
                            case SIGHUP:
                            {
                                continue;
                            }
                            case SIGTERM:
                            case SIGINT:
                            {
                                loop = false;
                                continue;
                            }
                            // 立即比对修改的lua文件
                            case SIGUSR1:
                            {
                                LOG_INFO("收到信号请求立即重新加载全局配置");
                                GlobalConfig::GetInstance()->ReLoadConfig("../config/global_config.lua");
                                continue;
                            }
                            // 重新加在需要监听的lua文件列表
                            case SIGUSR2:
                            {
                                LOG_INFO("收到信号请求立即重新加载所有lua配置");
                                LuaConfigManager::GetInstance()->CheckConfigFileIsChange();
                                continue;
                            }
                        }
                    }
                }
            }
            // IO事件
            else if (m_events[i].events&EPOLLIN)
            {
                char buf[TCP_BUFFER_SIZE];
                while(1)
                {
                    memset(buf, '\0', TCP_BUFFER_SIZE);
                    int ret = recv(sockfd, buf, TCP_BUFFER_SIZE - 1, 0);
                    if (ret < 0)
                    {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                        {
                            break;
                        }
                        // OnClientDisconnect(sockfd);
                        // break;
                    }
                    else if (ret == 0)
                    {
                        LOG_INFO("客户端主动断开连接");
                        OnClientDisconnect(sockfd);
                        break;
                    }
                    else
                    {
                        LOG_INFO("recv size = " + std::to_string(ret));
                        std::map<int, Client*>::iterator it = m_mClients.find(sockfd);
                        if (it != m_mClients.end() )
                        {
                            it->second->OnRecvMsg(buf, ret);
                        }
                    }
                }
            }
        }

        if (timeout)
        {
            TimerHandler();
            timeout = false;
        }
    }

    if (loop == false)
    {
        shutdown(m_listenFd, SHUT_RDWR);
        shutdown(m_epollFd, SHUT_RDWR);
        shutdown(s_pipefd[0], SHUT_RDWR);
        shutdown(s_pipefd[1], SHUT_RDWR);

        LOG_INFO("网络循环退出，关闭所有的网络链接");
        Exit();
    }
}
