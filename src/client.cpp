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

	if (m_nBufferDataSize < 4) return;

	while(1)
	{
		int packetLength = *(int*)m_recvBuffer;
		int nowDataLength = m_nBufferDataSize - 4;

		// 如果缓冲区的字节数量大于等于包头的大小, 则解析一个完整的数据包
		if (packetLength < RECV_BUFFER_SIZE && nowDataLength >= packetLength)
		{
			Packet packet(m_recvBuffer + 4, packetLength);
			OnNetMsgProcess(packet);
	
			// 使用了的缓冲区大小
			uint nUseSize = packetLength + 4;

			// 把缓冲区的剩余数据移到最前面
			memmove(m_recvBuffer, (m_recvBuffer + nUseSize), RECV_BUFFER_SIZE - nUseSize);
			// 重新设置缓冲区的字节大小
			m_nBufferDataSize = m_nBufferDataSize - nUseSize;

			if (nowDataLength == packetLength)
			{
				break;
			}
		}
		else
		{
			break;
		}

		nowDataLength = m_nBufferDataSize - 4;
		// 如果一遍循环之后，数据缓冲区的大小还大于包头的大小
		if (packetLength >= RECV_BUFFER_SIZE || nowDataLength > packetLength || nowDataLength < 0)
		{
			LOG_ERROR("数据解析出错，丢弃包: " + std::to_string(packetLength) + ", " + std::to_string(nowDataLength));
			memset(m_recvBuffer, '\0', RECV_BUFFER_SIZE);
			m_nBufferDataSize = 0;
			
			OnDisconnect();
			return;
		}
	}
}

void Client::SendData(uint16_t nSystem, uint16_t nCmd, string& data)
{
	// 先计算出包体的总长度
	// 因为packet类增加字符串的时候会增加2字节的长度和1字节的结束字符
	// 所以除了nSystem和nCmd之外需要多增加3字节的数据长度

	//LOG_INFO("发送数据包: data length = " + std::to_string(data.length()));

	const char* str = data.c_str();
	uint nDataLength = strlen(str);
	uint nPacketLength = sizeof(nSystem) + sizeof(nCmd) + 3 + nDataLength;
	Packet packet;

	packet << nPacketLength << nSystem << nCmd << str;


	// LOG_INFO("发送数据包: nSystem = " + std::to_string(nSystem) + ", nCmd = " + std::to_string(nCmd) + ", nPacketLength = " + std::to_string(nPacketLength));
	// 如果该文件描述符是非阻塞模式的话，send函数会根据内核缓冲区的可用空间把数据拷贝到内核，并且直接返回，返回值为已经拷贝了的字节数
	// 所以最好设置成阻塞模式，这样能保证需要发送的数据全部拷贝到内核缓冲区，并且通过tcp发送到客户端
	// 可以使用非阻塞模式的socket，但是这样需要在应用层进行比较多的处理，显而易见的解决方案是：如果只拷贝一部分到内核缓冲区，需要继续循环调用send函数
	int nSent = 0;

	// LOG_INFO("数据包的大小" + std::to_string(packet.getLength()));
	// send(m_fd, packet.getDataBegin() + nSent, packet.getLength() - nSent, 0);
	while(nSent < packet.getLength())
	{
        int ret = send(m_fd, packet.getDataBegin() + nSent, packet.getLength() - nSent, 0);
        if (ret > 0)
        {
            nSent += ret;
        }
	}
	// LOG_INFO("total nSent = " + std::to_string(nSent));
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
        	OnSendShellConfigToClient();

        	OnSendServerCurrentTimestamp();
        	OnSendCurrentProcessStatusInfo();
        }
        else if (nCmd == test_2::client_msg::REQUSET_LUA_TABLE_DATA)
        {
        	test_2::client_lua_table_data_quest quest;
			quest.ParseFromString(strData);

			OnSendLuaTableDataToClient(quest.file_name(), quest.link_info());
        }
        else if (nCmd == test_2::client_msg::REQUSET_SAVE_TABLE_DATA)
        {
        	test_2::client_save_table_data_request quest;
			quest.ParseFromString(strData);

			OnClientQuestSaveTableData(quest);
        }
        else if (nCmd == test_2::client_msg::REQUSET_SHELL_OPTIONS)
        {
        	test_2::client_shell_option_quest quest;
			quest.ParseFromString(strData);

			RequesetExceShellOps(quest.option());
        }
        else if (nCmd == test_2::client_msg::REQUEST_SAVE_TABLE_INFO)
        {
        	test_2::client_save_table_info_request quest;
			quest.ParseFromString(strData);

			OnClientQuestSaveTableInfo(quest);
        }
        else if (nCmd == test_2::client_msg::REQUEST_MODIFY_SERVER_TIME)
        {
        	test_2::client_modify_server_time_quest quest;
			quest.ParseFromString(strData);

			OnClientQuestModifyServerTime(quest.time());
        }
        else if (nCmd == test_2::client_msg::REQUSET_LUA_LIST_DATA)
        {
        	test_2::client_lua_list_data_quest quest;
			quest.ParseFromString(strData);

			OnSendLuaListDataToClient(quest.file_name(), quest.link_info());
        }
        else if (nCmd == test_2::client_msg::REQUSET_SAVE_LUA_LIST_DATA)
        {
        	test_2::save_lua_list_data_request quest;
			quest.ParseFromString(strData);

			OnClientQuestSaveLuaListInfo(quest);
        }
        else if (nCmd == test_2::client_msg::REQUSET_FIELD_LINK_INFO)
        {
        	OnSendFieldLinkInfo();	
        }
        else if (nCmd == test_2::client_msg::REQUSET_FIELD_INFO_BY_LINK)
        {
        	test_2::client_request_field_link_info quest;
			quest.ParseFromString(strData);

			OnClientQuestFieldInfoByLink(quest.link_info());
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

void Client::OnClientQuestModifyServerTime(uint64_t nTime)
{
	timeval p;
	gettimeofday(&p, NULL);

	struct tm* ptm = localtime (&(p.tv_sec));
    char time_string[40];

    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", ptm);

	LOG_INFO("请求修改系统时间 : " + std::string(time_string));
	p.tv_sec = nTime;
    settimeofday(&p, NULL);

    ptm = localtime (&(p.tv_sec));
    time_string[40];

    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", ptm);

    LOG_INFO("系统时间修改为 : " + std::string(time_string));
}

void Client::RequesetExceShellOps(string option)
{
	const std::vector<VALUEPAIR>& vShellConfig = GlobalConfig::GetInstance()->GetShellConfig();

	const string& sShellPath = GlobalConfig::GetInstance()->GetShellPath();

	string shell_value;
	for (int i = 0; i < vShellConfig.size(); ++i)
	{
		if (vShellConfig[i].sField == option)
		{
			shell_value = vShellConfig[i].sValue;
		}
	}

	if (shell_value.empty()) return;

	shell_value = sShellPath + "/" +  shell_value;

	char line[2048];

	FILE* fp;

	LOG_INFO("请求执行shell 指令： " + shell_value);
	if ((fp = popen(shell_value.c_str(), "r")) == NULL)
	{
		LOG_ERROR("执行shell指令错误 : " + option);
		return;
	}

	while(fgets(line, sizeof(line) - 1, fp) != NULL)
	{
		test_2::send_shell_option_print_notify notify;

		notify.set_line(string(line));
		notify.set_flag(0);

		string output;
    	notify.SerializeToString(&output);

		SendData(0, test_2::server_msg::SEND_OPTION_SHELL_PRINT, output);
		LOG_INFO(string(line));
	}

	test_2::send_shell_option_print_notify notify;

	notify.set_line("=======脚本执行结束========");
	notify.set_flag(1);

	string output;
    notify.SerializeToString(&output);

	SendData(0, test_2::server_msg::SEND_OPTION_SHELL_PRINT, output);

	LOG_INFO("shell 指令执行完毕");
	fclose(fp);
}

void Client::OnClientQuestSaveTableInfo(const test_2::client_save_table_info_request& quest)
{
	std::string sTableName = quest.table_name();

	std::map<string, LuaExtInfoContainer*>* tableInfoMap = LuaConfigManager::GetInstance()->GetTableInfoMap();
	if (tableInfoMap)
	{
		auto iter = tableInfoMap->find(sTableName);
		if (iter != tableInfoMap->end())
		{
			iter->second->UpdateData(quest);
		}

		std::map<string, LuaTableDataContainer*>* tableDataMap = LuaConfigManager::GetInstance()->GetTableDataMap();
		if(tableDataMap)
		{
			auto iter = tableDataMap->find(sTableName);
			if (iter != tableDataMap->end())
			{
				OnSendLuaTableDataToClient(sTableName);
			}
		}

		std::map<string, LuaListDataContainer*>* luaListDataMap = LuaConfigManager::GetInstance()->GetLuaListMap();
		if(luaListDataMap)
		{
			auto iter = luaListDataMap->find(sTableName);
			if (iter != luaListDataMap->end())
			{
				OnSendLuaListDataToClient(sTableName);
			}
		}
	}
}

void Client::OnClientQuestSaveTableData(const test_2::client_save_table_data_request& quest)
{
	std::map<string, LuaTableDataContainer*>* tableDataMap = LuaConfigManager::GetInstance()->GetTableDataMap();
	if(tableDataMap)
	{
		std::string sFileName = quest.table_name();
		auto iter = tableDataMap->find(sFileName);
		if (iter != tableDataMap->end())
		{
			iter->second->UpdateData(quest);
		}

		OnSendLuaTableDataToClient(sFileName);
	}
}

void Client::OnSendLuaTableDataToClient(std::string sFile, std::string sLinkInfo/* = ""*/)
{
	string testData = LuaConfigManager::GetInstance()->GetLuaTableDataByName(sFile, sLinkInfo);

	SendData(0, test_2::server_msg::SEND_LUA_TABLE_DATA, testData);
}

void Client::OnSendFileTreeInfoToClient()
{
	test_2::server_send_file_tree_notify notify;

	std::map<string, LuaTableDataContainer*>* mLuaTableDatas = LuaConfigManager::GetInstance()->GetTableDataMap();
	if (mLuaTableDatas)
	{
		for (map<string, LuaTableDataContainer*>::iterator iter = mLuaTableDatas->begin(); iter != mLuaTableDatas->end(); ++iter)
		{
			std::string* fileName = notify.add_lua_table_file_names();
			if(fileName)
			{
				*fileName = iter->first;
			}
		}
	}

	std::map<string, LuaListDataContainer*>* mLuaListDatas = LuaConfigManager::GetInstance()->GetLuaListMap();
	if (mLuaListDatas)
	{
		for (map<string, LuaListDataContainer*>::iterator iter = mLuaListDatas->begin(); iter != mLuaListDatas->end(); ++iter)
		{
			std::string* fileName = notify.add_lua_file_names();
			if(fileName)
			{
				*fileName = iter->first;
			}
		}
	}

	string output;
    notify.SerializeToString(&output);
    SendData(0, test_2::server_msg::SEND_FILE_TREE_INFO, output);
}

void Client::OnSendShellConfigToClient()
{
	const std::vector<VALUEPAIR>& vShellConfig = GlobalConfig::GetInstance()->GetShellConfig();

	if (vShellConfig.size() > 0)
	{
		test_2::server_send_shell_config_notify notify;

		for (int i = 0; i < vShellConfig.size(); ++i)
		{
			std::string* fileName = notify.add_shell_ops();
			if(fileName)
			{
				*fileName = vShellConfig[i].sField;
			}
		}

		string output;
    	notify.SerializeToString(&output);
    	SendData(0, test_2::server_msg::SEND_SHELL_CONFIG, output);
	}
}

void Client::OnSendServerCurrentTimestamp()
{
	time_t timestamp;
	uint64_t nTime = time(&timestamp);

	test_2::send_server_current_time_nofity notify;

	notify.set_time(nTime);

	string output;
    notify.SerializeToString(&output);
    SendData(0, test_2::server_msg::SEND_SERVER_TIME, output);
}

void Client::OnSendLuaListDataToClient(std::string sFile, std::string sLinkInfo/* = ""*/)
{
	string testData = LuaConfigManager::GetInstance()->GetLuaListDataByName(sFile, sLinkInfo);

	SendData(0, test_2::server_msg::SEND_LUA_LIST_DATA, testData);
}


void Client::OnClientQuestFieldInfoByLink(std::string sLinkInfo)
{
	vector<string> strvec = split(sLinkInfo, '#');
	string sTableType = strvec[0];
	string sTableName = strvec[1];
	string sTableField = strvec[2];
	std::map<string, LuaTableDataContainer*>* tableDataMap = LuaConfigManager::GetInstance()->GetTableDataMap();
	if(tableDataMap)
	{
		auto iter = tableDataMap->find(sTableName);
		if (iter != tableDataMap->end())
		{
			test_2::send_field_all_values_info notify;
			notify.set_link_info(sLinkInfo);			

			TABLEDATA tableData = iter->second->GetTableData();
			for (auto record : tableData.dataList)
			{
				for (auto pair : record.dataList)
				{
					if (pair.sField == sTableField)
					{
						test_2::link_field_info* info = notify.add_infos();
						if (info)
						{
							info->set_field_value(pair.sValue);
						}
					}
				}
			}

			string output;
    		notify.SerializeToString(&output);
    		SendData(0, test_2::server_msg::SEND_FIELD_INFO_BY_LINK, output);
		}
	}

	std::map<string, LuaListDataContainer*>* listDataMap = LuaConfigManager::GetInstance()->GetLuaListMap();
	if (listDataMap)
	{
		auto iter = listDataMap->find(sTableName);
		if (iter != listDataMap->end())
		{
			test_2::send_field_all_values_info notify;
			notify.set_link_info(sLinkInfo);

			std::vector<LUAKEYVALUE> luaKeyValue = iter->second->GetLinkInfoByKey(sTableField);
			for (auto data : luaKeyValue)
			{
				test_2::link_field_info* info = notify.add_infos();
				if (info)
				{
					info->set_field_value(data.sKey);
					info->set_field_desc(data.sValue);
				}
			}

			string output;
    		notify.SerializeToString(&output);
    		SendData(0, test_2::server_msg::SEND_FIELD_INFO_BY_LINK, output);
		}
	}
}

void Client::OnClientQuestSaveLuaListInfo(const test_2::save_lua_list_data_request& quest)
{
	LOG_INFO("请求保存一维表文件: " + quest.table_name());
	std::map<string, LuaListDataContainer*>* tableDataMap = LuaConfigManager::GetInstance()->GetLuaListMap();
	if(tableDataMap)
	{
		std::string sFileName = quest.table_name();
		auto iter = tableDataMap->find(sFileName);
		LOG_INFO("找到一维表文件的map: " + sFileName);
		if (iter != tableDataMap->end())
		{
			LOG_INFO("找到一维表文件的对象: " + sFileName);
			iter->second->UpdateData(quest);
		}

		OnSendLuaListDataToClient(sFileName);
	}	
}

void Client::OnSendFieldLinkInfo()
{
	test_2::send_field_link_info notify;

	std::map<string, LuaTableDataContainer*>* mLuaTableDatas = LuaConfigManager::GetInstance()->GetTableDataMap();
	if (mLuaTableDatas)
	{
		for (map<string, LuaTableDataContainer*>::iterator iter = mLuaTableDatas->begin(); iter != mLuaTableDatas->end(); ++iter)
		{
			test_2::table_field_list* tableFieldList = notify.add_table();
			if (tableFieldList)
			{
				tableFieldList->set_table_name(iter->first);
			}

			const std::vector<std::string>& vFieldStr = iter->second->GetVFeildStr();
			for (int i = 0; i < vFieldStr.size(); ++i)
			{
				std::string* fieldName = tableFieldList->add_fields();
				if (fieldName)
				{
					*fieldName = vFieldStr[i];
				}
			}
		}
	}

	std::map<string, LuaListDataContainer*>* mTableDataMap = LuaConfigManager::GetInstance()->GetLuaListMap();
	if (mTableDataMap)
	{
		for (map<string, LuaListDataContainer*>::iterator iter = mTableDataMap->begin(); iter != mTableDataMap->end(); ++iter)
		{
			test_2::table_field_list* tableFieldList = notify.add_list();
			if (tableFieldList)
			{
				tableFieldList->set_table_name(iter->first);
			}

			const std::vector<LUAKEYVALUE>& luaKeyValue = iter->second->GetListData();
			for (int i = 0; i < luaKeyValue.size(); ++i)
			{
				std::string* fieldName = tableFieldList->add_fields();
				if (fieldName)
				{
					*fieldName = luaKeyValue[i].sKey;
				}
			}
		}
	}

	string output;
    notify.SerializeToString(&output);

	SendData(0, test_2::server_msg::SEND_FIELD_LINK_DATA, output);
}

void Client::OnSendCurrentProcessStatusInfo()
{
	test_2::send_process_listening_status_info notify;
	const std::map<std::string, uint16_t>& vListenProcessStatus = IOManager::GetInstance()->GetProcessStatues();

	const std::vector<LISTENPROCESSINFO>& vListenProcessInfo = GlobalConfig::GetInstance()->GetListeningProcessInfo();
	for (auto data : vListenProcessInfo)
	{	
		uint16_t nStatus = 0;
		if (vListenProcessStatus.find(data.pidFile) != vListenProcessStatus.end())
		{
			nStatus = vListenProcessStatus.find(data.pidFile)->second;
		}

		test_2::process_statue_info* info = notify.add_infos();
		if (info)
		{
			info->set_process_name(data.processName);
			info->set_statue(nStatus);
		}
	}

	string output;
    notify.SerializeToString(&output);

	SendData(0, test_2::server_msg::SEND_PROCESS_STATUS_INFO, output);
}
