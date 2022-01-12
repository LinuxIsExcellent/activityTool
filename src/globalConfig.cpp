#include "globalConfig.h"

void GlobalConfig::LoadConfig(lua_State* L, string fileName)
{
    if (!L) return;

    int ret = luaL_dofile(L, fileName.c_str());
    if (ret)
    {
        string error = lua_tostring(L,-1);
        LOG_ERROR("load lua file error : " + error);
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

    // Lua文件列表
    lua_getglobal(L, "config_file_path");
    if (!lua_istable(L, -1))
    {
        LOG_ERROR("没有任何需要监听的lua文件列表");
        return;
    }

    m_listenLuaFileList.clear();
    lua_pushnil(L);
    while(lua_next(L, -2))
    {
        string strFile = lua_tostring(L, -1);

        m_listenLuaFileList.push_back(strFile);
        lua_pop(L, 1);
    }
}