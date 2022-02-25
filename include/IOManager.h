#pragma once
#include <map>
#include "include.h"
#include "client.h"

#define MAX_EVENT_NUMBER 1024
static int s_pipefd[2];    //把信号事件转换成IO事件的管道

class Client;
// IO管理类
// 使用epoll编写的一个类，用来统一服务中的所有事件源
class IOManager
{
private:
    IOManager() {};

    int setnonblocking(int fd);
    void addfd(int epollfd,int fd);
    static void sig_handler(int sig);
    void addsig(int sig);

    // 客户端断开链接
    void OnClientDisconnect(int sockfd);
    // 退出事件循环
    void Exit();

    // 处理定时器
    void TimerHandler();

    // 监听进程的状态
    void ProcessMonitorHandler();
public:
    ~IOManager() {};

    static IOManager* GetInstance(){
        if (m_instance == nullptr)
        {
            m_instance = new IOManager();
        }

        return m_instance;
    }

    const std::map<std::string, uint16_t>& GetProcessStatues()
    {
        return m_listenProcessStatues;
    }

    // 增加定时器
    void AddTimer(util_timer* timer);

    // 初始化io管理器
    void InitIOManager();

    // 增加网络端口监听
    void AddListeningFd(string ip, int port);

    // 事件循环
    void Loop();
private:
    static IOManager* m_instance;

    int m_epollFd;      //epoll实例的文件描述符
    int m_listenFd;     //监听的fd

    struct epoll_event m_events[MAX_EVENT_NUMBER];  //所有事件

    std::map<int, Client*> m_mClients; //所有处于链接状态的客户端

    bool loop;

    sort_timer_lst timer_lst;

    std::map<std::string, uint16_t>  m_listenProcessStatues;     //监听的服务的运行状态
};
