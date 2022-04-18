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

    void ReLoadConfig(lua_State* L, string fileName);

    string CalculateFileMd5();

    inline string getListeningIp()
    {
        return m_ListeningIp;
    }

    inline int getListeningPort()
    {
    	return m_ListeningPort;
    }

    inline string getConfigPath()
    {
        return m_ConfigPath;
    }

    inline string getTempConfigPath()
    {
        return m_TempConfigPath;
    }

    inline string getListeningProcessPath()
    {
        return m_ListeningProcessPath;
    }

    inline int getGlobalConfigReloadInterval()
    {
        return m_globalConfigReloadInterval;
    }

    inline int getConfigReloadInterval()
    {
        return m_configReloadInterval;
    }

    inline std::vector<string>& GetListenLuaFileList()
    {
        return m_LuaTableFileList;
    }

    inline std::vector<string>& GetListenLuaListFileList()
    {
        return m_LuaMapFileList;
    }

    inline const string& GetShellPath()
    {
        return m_sShellPath;
    }

    inline const std::vector<VALUEPAIR>& GetShellConfig()
    {
        return m_vShellConfig;
    }

    inline const std::vector<LISTENPROCESSINFO>& GetListeningProcessInfo()
    {
        return m_vListeningProcess;
    }
private:
    static GlobalConfig* m_instance;
    string m_LuaConfigPath; //配置文件名字

    string m_ListeningIp;   //监听的ip
    int m_ListeningPort;   //监听的端口

    string m_sMd5;
    string m_ConfigPath;   //配置文件路径
    string m_TempConfigPath;   //中间件文件路径
    string m_ListeningProcessPath; //监听的进程id文件产生的路径
    string m_sShellPath;        //shell脚本路径

    int m_globalConfigReloadInterval;  //global_config里面可热加载部分多长时间检测文件变化加载一次
    int m_configReloadInterval;  //被监听的二维展开和一维展开的文件多长时间检测文件变化热加载一次

    // 可热更部分
    std::vector<string> m_LuaTableFileList;    //所有二维表文件列表
    std::vector<string> m_LuaMapFileList;    //监听的所有lua文件列表
    std::vector<VALUEPAIR>      m_vShellConfig;     //可以执行的shell脚本
    std::vector<LISTENPROCESSINFO>      m_vListeningProcess;     //需要监听的程序列表
    // 可热更部分
};
