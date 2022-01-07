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
}

void IOManager::AddListeningFd(const char* ip, int port)
{
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    m_listenFd = socket(PF_INET, SOCK_STREAM, 0);
    assert(m_listenFd >= 0);
 
    int ret = bind(m_listenFd, (struct sockaddr*)&address, sizeof(address));
    if(ret == -1)
    {
        cout << "errno is" << errno << endl;
        assert(m_listenFd >= 0);
        return;
    }

    ret = listen(m_listenFd, 5);

    addfd(m_epollFd,m_listenFd);
}

void IOManager::OnClientDisconnect(int sockfd, Client* client)
{
    epoll_ctl(m_epollFd, EPOLL_CTL_DEL, sockfd, NULL);
    if (client)
    {
        client->OnDisconnect();
    }
}

// 退出事件循环，释放资源，断开所有客户端链接
void IOManager::Exit()
{
    std::map<int, Client*>::iterator it;

    for (it = m_mClients.begin(); it != m_mClients.end(); ++it)
    {
        OnClientDisconnect(it->first, it->second);
    }
}

void IOManager::Loop()
{
    loop = true;
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
                    cout << "new client is connect" << endl;
                    send(connfd, "hello", 5, 0);
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
                            case SIGCHLD:
                            case SIGHUP:
                            {
                                continue;
                            }
                            case SIGTERM:
                            case SIGINT:
                            {
                                loop = false;
                            }
                        }
                    }
                }
            }
            // IO事件
            else if (m_events[i].events&EPOLLIN)
            {
                char buf[TCP_PACKET_MAX];
                int ret = recv(sockfd, buf, TCP_PACKET_MAX - 1, 0);
                if (ret <= 0)
                {
                    std::map<int, Client*>::iterator it = m_mClients.find(sockfd);
                    if (it != m_mClients.end())
                    {
                        OnClientDisconnect(sockfd, it->second);
                    }
                }
                else if (ret > 0)
                {
                    cout << "client msg: " << "size = " << ret << "content = " << buf << endl;
                }
            }
        }
    }

    if (loop == false)
    {
        close(m_listenFd);
        close(m_epollFd);
        close(s_pipefd[0]);
        close(s_pipefd[1]);
        Exit();
    }
}
