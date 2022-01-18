#include "client.h"

Client::Client()
{
	m_recvBuffer = (char*) malloc(TCP_PACKET_MAX);
	m_nBufferDataSize = 0;
}

Client::~Client()
{
	free(m_recvBuffer);
	m_recvBuffer = nullptr;
}

void Client::OnRecvMsg(char* buffer, int16_t nLength)
{
	if(m_nBufferDataSize <= 0)
	{
		// 把数据拷贝到客户端数据缓冲区
		memcpy(m_recvBuffer, buffer, nLength);
		m_nBufferDataSize = nLength;
	}
	else if (m_nBufferDataSize <= TCP_PACKET_MAX)
	{
		int16_t nAvaliableSize = TCP_PACKET_MAX - m_nBufferDataSize;
		nLength = MIN(nLength, nAvaliableSize);

		memcpy(m_recvBuffer + m_nBufferDataSize, buffer, nLength);

		m_nBufferDataSize += nLength;		
	}
	else
	{
		LOG_ERROR("数据包越界, 强制断开连接");
		OnDisconnect();

		return;
	}

	int packetLength = *(int*)m_recvBuffer;
	// 如果缓冲区的字节数量大于等于包头的大小, 则解析一个完整的数据包
	if (packetLength < TCP_PACKET_MAX && m_nBufferDataSize - 4 >= packetLength)
	{
		Packet packet(m_recvBuffer + 4, packetLength);
		OnNetMsgProcess(packet);

		// 使用了的缓冲区大小
		int16_t nUseSize = packetLength + 4;
		// 把缓冲区的剩余数据移到最前面
		memcpy(m_recvBuffer, m_recvBuffer + nUseSize, TCP_PACKET_MAX - nUseSize);
		// 重新设置缓冲区的字节大小
		m_nBufferDataSize = m_nBufferDataSize - nUseSize;
	}
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

void Client::OnNetMsgProcess(Packet &packet)
{
	int16_t nSystem, nCmd;
	packet >> nSystem >> nCmd;
	
	std::string dataStr(packet.getDataBegin() + 4, packet.getLength() - 4);
	if (nSystem == 0)
	{
		if (nCmd == test_2::client_msg::REQUEST_LOGIN)
		{
			OnSendFileTreeInfoToClient();
		}
		else if (nCmd == test_2::client_msg::REQUSET_LUA_TABLE_INFO)
		{
			test_2::client_lua_table_data_quest quest;
			quest.ParseFromArray(packet.getDataBegin() + 4, packet.getLength() - 4);

			OnSendLuaTableDataToClient(quest.file_name());
		}
	}
}

void Client::OnConnect(struct sockaddr_in & address, int fd)
{
	m_client_address = address;
	m_fd = fd;

	m_nPort = m_client_address.sin_port;

	struct in_addr in  = m_client_address.sin_addr;
	inet_ntop(AF_INET, &in, ip, sizeof(ip));

	LOG_INFO("客户端链接： ip = " + std::string(ip) + ", port = " + std::to_string(m_nPort));
}

// 断开链接
void Client::OnDisconnect()
{
	// 释放文件描述符
	close(m_fd);

	LOG_INFO("客户端断开链接： ip = " + std::string(ip) + ", port = " + std::to_string(m_nPort));
}

void Client::OnSendLuaTableDataToClient(std::string sFile)
{
	string testData = LuaConfigManager::GetInstance()->GetLuaDataByName(sFile);

	SendData(0, test_2::server_msg::SEND_LUA_TABLE_DATA, testData);
}

void Client::OnSendFileTreeInfoToClient()
{
	std::map<string, LuaDataContainer*>* mLuaTableDatas = LuaConfigManager::GetInstance()->GetTableDataMap();
	if (mLuaTableDatas)
	{
		test_2::server_send_file_tree_notify notify;

		map<string, LuaDataContainer*>::iterator iter;
		for (; iter != mLuaTableDatas->end(); ++iter)
		{
			std::string* fileName = notify.add_lua_file_names();

			*fileName = iter->first;
		}

		string output;
    	notify.SerializeToString(&output);

    	SendData(0, test_2::server_msg::SEND_FILE_TREE_INFO, output);
	}
}