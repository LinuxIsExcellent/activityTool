#pragma once
#include "include.h"

class LuaDataContainer
{
public:
    LuaDataContainer() {};
    ~LuaDataContainer() {};

    bool LoadLuaConfigData(lua_State* L , string path, string name);
private:
    const char* m_LuaFileName;   //lua文件的名字
    const char* m_LuaFilePath;   //lua文件的路径
};
