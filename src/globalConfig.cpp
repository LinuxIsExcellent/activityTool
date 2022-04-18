#include "globalConfig.h"

string GlobalConfig::CalculateFileMd5()
{
    ifstream ifs(m_LuaConfigPath.c_str());
    MD5* md5 = new MD5(ifs);
    if (md5)
    {
        return md5->toString();
    }

    return "";
}

void GlobalConfig::ReLoadConfig(lua_State* L, string fileName)
{
    if (CalculateFileMd5() == m_sMd5)
    {
        return;
    }
    if (!L) return;

    LOG_INFO("全局配置文件热加载开始");

    int ret = luaL_dofile(L, fileName.c_str());
    if (ret)
    {
        string error = lua_tostring(L,-1);
        LOG_ERROR("load lua file error : " + error);
        return;
    }

    // Lua文件列表
    m_LuaTableFileList.clear();
    lua_getglobal(L, "config_file_path");
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while(lua_next(L, -2))
        {
            string strFile = lua_tostring(L, -1);
    
            m_LuaTableFileList.push_back(strFile);
            lua_pop(L, 1);
        }
    }

    // 可执行的shell脚本列表
    m_vShellConfig.clear();
    lua_getglobal(L, "shell_config");
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while(lua_next(L, -2))
        {
            VALUEPAIR pair;
            pair.sField = lua_tostring(L, -2);
            pair.sValue = lua_tostring(L, -1);

            m_vShellConfig.push_back(pair);

            lua_pop(L, 1);
        }   
    }

    // Lua文件列表
    m_LuaMapFileList.clear();
    lua_getglobal(L, "config_list_file_path");
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while(lua_next(L, -2))
        {
            string strFile = lua_tostring(L, -1);
    
            m_LuaMapFileList.push_back(strFile);
            lua_pop(L, 1);
        }
    }

    // 需要监听的服务器的进程id的文件列表
    m_vListeningProcess.clear();
    lua_getglobal(L, "listening_process_config");
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while(lua_next(L, -2))
        {
            lua_pushnil(L);

            LISTENPROCESSINFO info;
            while(lua_next(L, -2))
            {
                string strFile = lua_tostring(L, -1);
                if (lua_tonumber(L, -2) == 1) 
                {
                    info.pidFile = strFile;
                }   
                else if (lua_tonumber(L, -2) == 2)
                {
                    info.processName = strFile;
                }
                
                lua_pop(L, 1);
            }

            lua_pop(L, 1);
            m_vListeningProcess.push_back(info);
        }
    }

    m_sMd5 = CalculateFileMd5();
}

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

    m_LuaConfigPath = fileName;

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
    lua_getglobal(L, "script_operate_path");
    m_sShellPath = lua_tostring(L, -1);

    lua_getglobal(L, "global_config_reload_interval");
    m_globalConfigReloadInterval = lua_tonumber(L, -1);
    lua_getglobal(L, "config_reload_interval");
    m_configReloadInterval = lua_tonumber(L, -1);

    // Lua文件列表
    m_LuaTableFileList.clear();
    lua_getglobal(L, "config_file_path");
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while(lua_next(L, -2))
        {
            string strFile = lua_tostring(L, -1);
    
            m_LuaTableFileList.push_back(strFile);
            lua_pop(L, 1);
        }
    }

    // 可执行的shell脚本列表
    m_vShellConfig.clear();
    lua_getglobal(L, "shell_config");
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while(lua_next(L, -2))
        {
            VALUEPAIR pair;
            pair.sField = lua_tostring(L, -2);
            pair.sValue = lua_tostring(L, -1);

            m_vShellConfig.push_back(pair);

            lua_pop(L, 1);
        }   
    }

    // Lua文件列表
    m_LuaMapFileList.clear();
    lua_getglobal(L, "config_list_file_path");
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while(lua_next(L, -2))
        {
            string strFile = lua_tostring(L, -1);
    
            m_LuaMapFileList.push_back(strFile);
            lua_pop(L, 1);
        }
    }

    // 需要监听的服务器的进程id的文件列表
    m_vListeningProcess.clear();
    lua_getglobal(L, "listening_process_config");
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while(lua_next(L, -2))
        {
            lua_pushnil(L);

            LISTENPROCESSINFO info;
            while(lua_next(L, -2))
            {
                string strFile = lua_tostring(L, -1);
                if (lua_tonumber(L, -2) == 1) 
                {
                    info.pidFile = strFile;
                }   
                else if (lua_tonumber(L, -2) == 2)
                {
                    info.processName = strFile;
                }
                
                lua_pop(L, 1);
            }

            lua_pop(L, 1);
            m_vListeningProcess.push_back(info);
        }
    }

    m_sMd5 = CalculateFileMd5();
}