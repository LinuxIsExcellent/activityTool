#pragma once
#include <map>
#include "include.h"
#include "client.h"

#define MAX_EVENT_NUMBER 1024
static int s_pipefd[2];    //把信号事件转换成IO事件的管道

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
    void OnClientDisconnect(int sockfd, Client* client);
    // 退出事件循环
    void Exit();
public:
    ~IOManager() {};

    static IOManager* GetInstance(){
        if (m_instance == nullptr)
        {
            m_instance = new IOManager();
        }

        return m_instance;
    }

    // 初始化io管理器
    void InitIOManager();

    // 增加网络端口监听
    void AddListeningFd(const char* ip, int port);

    // 事件循环
    void Loop();
private:
    static IOManager* m_instance;

    int m_epollFd;      //epoll实例的文件描述符
    int m_listenFd;     //监听的fd

    struct epoll_event m_events[MAX_EVENT_NUMBER];  //所有事件

    std::map<int, Client*> m_mClients; //所有处于链接状态的客户端

    bool loop;
};