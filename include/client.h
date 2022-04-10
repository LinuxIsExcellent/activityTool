#pragma once
#include "include.h"

#define RECV_BUFFER_SIZE 10 * 1024 * 1024

// 客户端连接类
class Client
{
public:
	Client();
	~Client();

	void OnConnect(struct sockaddr_in & address, int fd);

	void OnDisconnect();

	void SendData(uint16_t nSystem, uint16_t nCmd, string& data);

	// 接收到网络数据
	void OnRecvMsg(char* buffer, uint nLength);

	// 网络数据处理
	void OnNetMsgProcess(Packet &packet);

	// 发送当前的文件树列表给前端
	void OnSendFileTreeInfoToClient();

	// 发送可执行的shell操作给前端
	void OnSendShellConfigToClient();

	// 发送单个lua二维表文件
	void OnSendLuaTableDataToClient(std::string sFile, std::string sLinkInfo = "");

	// 客户端修改表数据
	void OnClientQuestSaveTableData(const test_2::client_save_table_data_request& quest);

	// 客户端修改表的外围数据
	void OnClientQuestSaveTableInfo(const test_2::client_save_table_info_request& quest);

	// 发送服务器当前时间
	void OnSendServerCurrentTimestamp();

	// 请求修改服务器时间
	void OnClientQuestModifyServerTime(uint64_t nTime);

	// 发送单个一维表文件数据
	void OnSendLuaListDataToClient(std::string sFile, std::string sLinkInfo = "");

	// 请求保存单个一维表数据
	void OnClientQuestSaveLuaListInfo(const test_2::save_lua_list_data_request& quest);

	//客戶端字段的关联信息获取字段的所有键值
	void OnClientQuestFieldInfoByLink(std::string sLinkInfo);

	// 发送能关联的信息
	void OnSendFieldLinkInfo();

	//发送当前的服务进程的状态
	void OnSendCurrentProcessStatusInfo();
private:
	// 执行shell指令
	void RequesetExceShellOps(string option);
private:
	struct sockaddr_in m_client_address;	//客户端连接信息
	char ip[INET_ADDRSTRLEN];	//客户端ip
	uint16_t m_nPort;	//客户端端口
	int m_fd;	//客户端的文件描述符

	char* m_recvBuffer;		//客户端的socket接收数据缓冲区
	uint m_nBufferDataSize;	//接收缓存区中的字节大小
};