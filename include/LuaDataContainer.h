#pragma once
#include "include.h"

// Lua文件数据类
class LuaDataContainer
{
public:
    LuaDataContainer(string sLuaFileName, string sLuaFilePath);
    ~LuaDataContainer();

    // 加载lua配置的数据
    bool LoadLuaConfigData(lua_State* L);

    string ParseLuaTableToString(lua_State* L);
private:
    string m_LuaFileName;   //lua文件的名字
    string m_LuaFilePath;   //lua文件的路径
};
