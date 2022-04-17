#pragma once
#include "include.h"

// Lua文件的非数据的信息类
// 根据在客户端操作表格自动生成的表的信息（字段顺序，字段标注，活动的字段组合...）
class LuaExtInfoContainer
{
public:
    LuaExtInfoContainer(string sLuaFileName, string sLuaFilePath);
    ~LuaExtInfoContainer();

    string CalculateFileMd5();

    string GetFileMd5()
    {
        return m_sMd5;
    }  

    string GetLuaFileName()
    {
        return m_LuaFileName;
    }

    // 把字段的后面的数字切割出来
    void SplitSequenceKeyToNumVector(const std::string str, std::vector<string>& v, const char* delimiter = "_");

    // 加载lua表的信息数据
    bool LoadTableInfoData();

    // 通过键值获得字段顺序数据
    FIELDSQUENCE* GetFieldSquenceDataByKey(std::string sKey)
    {
        if(m_mFieldSquences.find(sKey) != m_mFieldSquences.end())
        {
            return &m_mFieldSquences.find(sKey)->second;
        }

        return NULL;
    }

    // 获取字段顺序数据
    std::map<std::string, FIELDSQUENCE>& GetFieldQquenceData()
    {
        return m_mFieldSquences;
    }
    
    // 客户端请求保存外围信息
    void UpdateData(const test_2::client_save_table_info_request& quest);
private:
    void DumpTableInfoToConfigFile();
private:
    string m_LuaFileName;   //lua文件的名字
    string m_LuaFilePath;   //lua文件的路径
    string m_sMd5;    //该文件的md5码
    
    std::map<std::string, FIELDSQUENCE> m_mFieldSquences;     //字段顺序队列
};
