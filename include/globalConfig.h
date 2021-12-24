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

    const char* getListeningIp()
    {
        return m_ListeningIp;
    }

    int getListeningPort()
    {
    	return m_ListeningPort;
    }

    const char* getConfigPath()
    {
        return m_ConfigPath;
    }

    const char* getTempConfigPath()
    {
        return m_TempConfigPath;
    }

    const char* getListeningProcessPath()
    {
        return m_ListeningProcessPath;
    }
private:
    static GlobalConfig* m_instance;

    const char* m_ListeningIp;   //监听的ip
    int m_ListeningPort;   //监听的端口

    const char* m_ConfigPath;   //配置文件路径
    const char* m_TempConfigPath;   //中间件文件路径
    const char* m_ListeningProcessPath; //监听的进程id文件产生的路径
    // char* m_ConfigPath;  //可监听的进程id文件的名字
};