#include "client.h"

Client::Client()
{
	
}

void Client::SendData(int16_t nSystem, int16_t nCmd, string data)
{
	test_2::net_packet packet;
	packet.set_system(nSystem);
	packet.set_cmd(nCmd);
	packet.set_data("asdassdas");

	string output;
    packet.SerializeToString(&output);

	// send(m_fd, output.c_str(), output.length(), 0);
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