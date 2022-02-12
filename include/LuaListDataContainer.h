#pragma once
#include "include.h"

// Lua文件数据类
// 只支持键值对的lua表格式
class LuaListDataContainer
{
public:
    LuaListDataContainer(string sLuaFileName, string sLuaFilePath);
    ~LuaListDataContainer();

    // 加载lua配置的数据
    bool LoadLuaConfigData(lua_State* L);
private:
private:
    string m_LuaFileName;   //lua文件的名字
    string m_LuaFilePath;   //lua文件的路径

    std::vector<LUAKEYVALUE> m_vValueLists;     //数据集合
};
