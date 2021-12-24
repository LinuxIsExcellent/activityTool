#include "globalConfig.h"

void GlobalConfig::LoadConfig(lua_State* L, string fileName)
{
    if (!L) return;

    int ret = luaL_dofile(L, fileName.c_str());
    if (ret)
    {
        cout << "load file error" << endl;
        return;
    }

    lua_getglobal(L, "config_path");
    m_ConfigPath = lua_tostring(L, -1);
    lua_getglobal(L, "temp_config_path");
    m_TempConfigPath = lua_tostring(L, -1);
    lua_getglobal(L, "listening_process_path");
    m_ListeningProcessPath = lua_tostring(L, -1);
    lua_getglobal(L, "server_ip");
    m_ListeningIp = lua_tostring(L, -1);
    lua_getglobal(L, "server_port");
    m_ListeningPort = lua_tonumber(L, -1);
}