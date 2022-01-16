#include "client.h"

Client::Client()
{
	
}

void Client::SendData(int16_t nSystem, int16_t nCmd, string data)
{
	// 先计算出包体的总长度
	// 因为packet类增加字符串的时候会增加2字节的长度和1字节的结束字符
	// 所以除了nSystem和nCmd之外需要多增加3字节的数据长度
	int nDataLength = sizeof(nSystem) + sizeof(nCmd) + 3 + data.length();
	Packet packet;
	packet << nDataLength << nSystem << nCmd << data.c_str();

	send(m_fd, packet.getDataBegin(), packet.getLength(), 0);
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
