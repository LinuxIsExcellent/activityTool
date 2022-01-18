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

	// 接收到网络数据
	void OnRecvMsg(char* buffer, int16_t nLength);

	// 网络数据处理
	void OnNetMsgProcess(Packet &packet);

	// 发送当前的文件树列表给前端
	void OnSendFileTreeInfoToClient();

	// 发送单个lua二维表文件
	void OnSendLuaTableDataToClient(std::string sFile);
private:
	struct sockaddr_in m_client_address;	//客户端链接信息
	char ip[INET_ADDRSTRLEN];	//客户端ip
	int16_t m_nPort;	//客户端端口
	int m_fd;	//客户端的文件描述符

	char* m_recvBuffer;		//客户端的socket接收数据缓冲区
	int16_t m_nBufferDataSize;	//接收缓存区中的字节大小
};