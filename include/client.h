#pragma once
#include "include.h"

// 客户端连接类
class Client
{
public:
	Client();
	~Client();

	void OnConnect(struct sockaddr_in & address, int fd);

	void OnDisconnect();

	void SendData(int16_t nSystem, int16_t nCmd, string data);
private:
	struct sockaddr_in m_client_address;	//客户端链接信息
	int m_fd;	//客户端的文件描述符
};