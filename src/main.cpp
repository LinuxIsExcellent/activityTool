#include "include.h"
#include "IOManager.h"

// 全局配置单例
GlobalConfig* GlobalConfig::m_instance = NULL;

// Lua配置管理器
LuaConfigManager* LuaConfigManager::m_instance = NULL;
// 全局唯一虚拟机
static lua_State *L = NULL;
// 全局I/O管理器
IOManager* IOManager::m_instance = NULL;

string subreplace(string resource_str, string sub_str, string new_str)
{
    string::size_type last_pos = 0;
    string::size_type pos = resource_str.find_first_of(sub_str, last_pos);

    while(pos != string::npos)   //替换所有指定子串
    {
        // 替换
        resource_str.replace(pos, sub_str.length(), new_str);

        // 移动位置，避免重复循环替换
        last_pos = pos + new_str.length();

        pos = resource_str.find_first_of(sub_str, last_pos);
    }
    return resource_str;
}

int main()
{
    // 一些初始化的工作
    L = luaL_newstate();
    if(L == NULL)
    {
        return 0;
    }

    //加载全局配置表
    GlobalConfig::GetInstance()->LoadConfig(L, "../config/global_config.lua");

    //加载中间所有配置的中间文件
    LuaConfigManager::GetInstance()->LoadAllLuaTempConfigData(L);

    //加载所有lua配置数据
    LuaConfigManager::GetInstance()->LoadAllLuaConfigData(L);

    // 加载所有的lua键值对一维表数据
    LuaConfigManager::GetInstance()->LoadLuaListConfigData(L);

    // 初始化IO管理器
    IOManager::GetInstance()->InitIOManager();
    // 监听ip和端口
    IOManager::GetInstance()->AddListeningFd(GlobalConfig::GetInstance()->getListeningIp(), GlobalConfig::GetInstance()->getListeningPort());

    // 开始IO循环
    IOManager::GetInstance()->Loop();

    // IO循环结束，释放全局资源
    LuaConfigManager::GetInstance()->FreeData();

    // 释放lua虚拟机
    lua_close(L);
    L = NULL;

    return 0;
}
