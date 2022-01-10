#pragma once
#include "include.h"

// 全局配置类
class GlobalConfig
{
private:
    GlobalConfig() {};
public:
    ~GlobalConfig() {};

    static GlobalConfig* GetInstance(){
        if (m_instance == nullptr)
        {
            m_instance = new GlobalConfig();
        }

        return m_instance;
    }

    void LoadConfig(lua_State* L, string fileName);

    string getListeningIp()
    {
        return m_ListeningIp;
    }

    int getListeningPort()
    {
    	return m_ListeningPort;
    }

    string getConfigPath()
    {
        return m_ConfigPath;
    }

    string getTempConfigPath()
    {
        return m_TempConfigPath;
    }

    string getListeningProcessPath()
    {
        return m_ListeningProcessPath;
    }

    std::vector<string>& GetListenLuaFileList()
    {
        return m_listenLuaFileList;
    }
private:
    static GlobalConfig* m_instance;

    string m_ListeningIp;   //监听的ip
    int m_ListeningPort;   //监听的端口

    string m_ConfigPath;   //配置文件路径
    string m_TempConfigPath;   //中间件文件路径
    string m_ListeningProcessPath; //监听的进程id文件产生的路径
    // char* m_ConfigPath;  //可监听的进程id文件的名字

    std::vector<string> m_listenLuaFileList;    //监听的所有lua文件列表
};
