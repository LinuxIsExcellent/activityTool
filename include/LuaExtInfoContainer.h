#pragma once
#include "include.h"

// Lua文件的非数据的信息类
// 根据在客户端操作表格自动生成的表的信息（字段顺序，字段标注，活动的字段组合...）
class LuaExtInfoContainer
{
public:
    LuaExtInfoContainer(string sLuaFileName, string sLuaFilePath);
    ~LuaExtInfoContainer();

    // 把字段的后面的数字切割出来
    void SplitSequenceKeyToNumVector(const std::string str, std::vector<string>& v, const char* delimiter = "_");

    // 加载lua表的信息数据
    bool LoadTableInfoData(lua_State* L);

    // 获取字段顺序队列数据
    std::vector<FIELDSQUENCE>& GetFieldQquenceData()
    {
        return m_vFieldSquences;
    }
    
    // 客户端请求保存外围信息
    void UpdateData(const test_2::client_save_table_info_request& quest);
private:
    void DumpTableInfoToConfigFile();
private:
    string m_LuaFileName;   //lua文件的名字
    string m_LuaFilePath;   //lua文件的路径
    
    std::vector<FIELDSQUENCE> m_vFieldSquences;     //字段顺序队列
};
