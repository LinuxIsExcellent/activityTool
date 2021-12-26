#include "client.h"

Client::Client()
{
	
}

void Client::OnConnect(struct sockaddr_in & address, int fd)
{
	m_client_address = address;
	m_fd = fd;
}

// 断开链接
void Client::OnDisconnect()
{
	// 释放文件描述符
	close(m_fd);

	cout << "client is disconnect " << "client ip = " << "123132" << endl;
}