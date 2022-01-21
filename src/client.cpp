#include "client.h"

Client::Client()
{
	m_recvBuffer = (char*) malloc(RECV_BUFFER_SIZE);
	m_nBufferDataSize = 0;
}

Client::~Client()
{
	LOG_INFO("客户端析构");

	std::cout << "大小 = " << RECV_BUFFER_SIZE << std::endl;

	free(m_recvBuffer);
	m_recvBuffer = nullptr;

}

void Client::OnRecvMsg(char* buffer, uint nLength)
{
	if(m_nBufferDataSize <= 0)
	{
		// 把数据拷贝到客户端数据缓冲区
		memcpy(m_recvBuffer, buffer, nLength);
		m_nBufferDataSize = nLength;
	}
	else if (m_nBufferDataSize <= RECV_BUFFER_SIZE)
	{
		uint nAvaliableSize = RECV_BUFFER_SIZE - m_nBufferDataSize;
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
	if (packetLength < RECV_BUFFER_SIZE && m_nBufferDataSize - 4 >= packetLength)
	{
		Packet packet(m_recvBuffer + 4, packetLength);
		OnNetMsgProcess(packet);

		// 使用了的缓冲区大小
		uint nUseSize = packetLength + 4;
		// 把缓冲区的剩余数据移到最前面
		memcpy(m_recvBuffer, m_recvBuffer + nUseSize, RECV_BUFFER_SIZE - nUseSize);
		// 重新设置缓冲区的字节大小
		m_nBufferDataSize = m_nBufferDataSize - nUseSize;
	}
}

void Client::SendData(uint16_t nSystem, uint16_t nCmd, string& data)
{
	// 先计算出包体的总长度
	// 因为packet类增加字符串的时候会增加2字节的长度和1字节的结束字符
	// 所以除了nSystem和nCmd之外需要多增加3字节的数据长度

	LOG_INFO("发送数据包: nSystem = " + std::to_string(nSystem) + ", nCmd = " + std::to_string(nCmd) + ", length = " + std::to_string(data.length()));
	uint nDataLength = sizeof(nSystem) + sizeof(nCmd) + 3 + data.length();
	Packet packet;
	packet << nDataLength << nSystem << nCmd << data.c_str();

	// 如果该文件描述符是非阻塞模式的话，send函数会根据内核缓冲区的可用空间把数据拷贝到内核，并且直接返回，返回值为已经拷贝了的字节数
	// 所以最好设置成阻塞模式，这样能保证需要发送的数据全部拷贝到内核缓冲区，并且通过tcp发送到客户端
	// TODO：可以使用非阻塞模式的socket，但是这样需要在应用层进行比较多的处理，显而易见的解决方案是：如果只拷贝一部分到内核缓冲区，需要继续循环调用send函数
	send(m_fd, packet.getDataBegin(), packet.getLength(), 0);

	// LOG_INFO("nSendCount = " + std::to_string(nSendCount));
}

void Client::OnNetMsgProcess(Packet &packet)
{
	uint16_t nSystem, nCmd;
    const char* strData;

    packet >> nSystem >> nCmd >> strData;
    LOG_INFO("客户端请求协议: " + std::to_string(nSystem) + " " + std::to_string(nCmd));

    if (nSystem == 0)
    {
        if (nCmd == test_2::client_msg::REQUEST_LOGIN)
        {
        	OnSendFileTreeInfoToClient();
        }
        else if (nCmd == test_2::client_msg::REQUSET_LUA_TABLE_INFO)
        {
        	test_2::client_lua_table_data_quest quest;
			quest.ParseFromString(strData);

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

		for (map<string, LuaDataContainer*>::iterator iter = mLuaTableDatas->begin(); iter != mLuaTableDatas->end(); ++iter)
		{
			std::string* fileName = notify.add_lua_table_file_names();
			if(fileName)
			{
				*fileName = iter->first;
			}
		}

		string output;
    	notify.SerializeToString(&output);
    	SendData(0, test_2::server_msg::SEND_FILE_TREE_INFO, output);
	}
}