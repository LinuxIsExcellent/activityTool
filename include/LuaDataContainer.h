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

    // 获取数据
    string GetStrData();
private:
	// 把lua表解析成string格式
    string ParseLuaTableToString(lua_State* L);

private:
    string m_LuaFileName;   //lua文件的名字
    string m_LuaFilePath;   //lua文件的路径

    test_2::table_info _table;  //对应的protobuf的数据(把lua表的数据直接存入protobuf里面)
};
