#pragma once
#include "include.h"

typedef struct fieldSquence
{
    std::vector<uint16_t> vNLevels;         //深度队列
    std::vector<std::string> vSFieldSquences;       //对应的字段顺序
}FIELDSQUENCE;


// Lua文件的非数据的信息类
// 根据在客户端操作表格自动生成的表的信息（字段顺序，字段标注，活动的字段组合...）
class LuaTableInfoContainer
{
public:
    LuaTableInfoContainer(string sLuaFileName, string sLuaFilePath);
    ~LuaTableInfoContainer();

    // 把字段的后面的数字切割出来
    void SplitSequenceKeyToNumVector(const std::string str, std::vector<string>& v, const char* delimiter = "_");

    // 加载lua表的信息数据
    bool LoadTableInfoData(lua_State* L);

    // 获取数据
    string GetStrData();

    // 获取字段顺序队列数据
    std::vector<FIELDSQUENCE>& GetFieldQquenceData()
    {
        return m_vFieldSquences;
    }
private:

private:
    string m_LuaFileName;   //lua文件的名字
    string m_LuaFilePath;   //lua文件的路径

    test_2::table_info _table_info;  //对应的protobuf的数据(把lua表的数据直接存入protobuf里面)
    
    std::vector<FIELDSQUENCE> m_vFieldSquences;     //字段顺序队列
};
