#pragma once
#include "include.h"

// 键值对
typedef struct oneValuePair
{
    string sField;
    string sValue;
}VALUEPAIR;

// 一行的数据
typedef struct oneRowData
{
    int16_t id;      //对应的唯一id
    int16_t nFeildCount;    //有多少个字段(二维表的所有行都是一致的，只是为了好读取)
    std::vector<VALUEPAIR> dataList;    //一行内所有的列数据
}ROWDATA;

// 一个表的数据
typedef struct tableData
{
    string sTableName;
    int16_t nRow;    //多少行
    int16_t nColumn;    //多少列
    std::vector<ROWDATA> dataList;    //每行的数据
}TABLEDATA;

// Lua文件数据类
class LuaDataContainer
{
public:
    LuaDataContainer(string sLuaFileName, string sLuaFilePath);
    ~LuaDataContainer();

    // 加载lua配置的数据
    bool LoadLuaConfigData(lua_State* L);

    // 获取数据
    string GetProtoDataStr();
private:
	// 把lua表解析成string格式
    string ParseLuaTableToString(lua_State* L);

private:
    string m_LuaFileName;   //lua文件的名字
    string m_LuaFilePath;   //lua文件的路径

    TABLEDATA m_table_data;                             //对应的二维表的数据
    std::map<string, int>   m_mFeildStrs;                //最外层的字段
    std::vector<std::string>         m_vFeildStrs;      //最外层的字段
    std::vector<test_2::DATA_TYPE>   m_vFeildTypes;      //字段对应的类型
};
