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

    bool UpdateData(test_2::client_save_table_data_request& proto);

    // 获取二维表的数据
    const TABLEDATA& GetTableData()
    {
        return m_table_data;
    }

    // 获取字段列表
    const std::vector<std::string>& GetVFeildStr()
    {
        return m_vFeildStrs;
    }


    // 排序最外层的字段
    void SortFieldSquence();
    // 获取字段对应的类型
    const std::map<std::string, int>& GetVFeildTypes()
    {
        return m_mFeildTypes;
    }
private:
	// 把lua表解析成string格式
    string ParseLuaTableToString(lua_State* L);

    // 把数据重写到配置文件中
    void DumpTableDataToConfigFile();
private:
    string m_LuaFileName;   //lua文件的名字
    string m_LuaFilePath;   //lua文件的路径

    TABLEDATA m_table_data;                             //对应的二维表的数据
    std::vector<std::string>         m_vFeildStrs;      //最外层的字段
    std::map<std::string, int> m_mFeildTypes; //字段对应的类型
};
