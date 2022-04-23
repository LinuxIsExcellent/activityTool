#pragma once
#include "include.h"


class LuaExtInfoContainer;
// Lua文件数据类
// 只支持键值对的lua表格式
class LuaListDataContainer
{
public:
    LuaListDataContainer(string sLuaFileName, string sLuaFilePath);
    ~LuaListDataContainer();

    string CalculateFileMd5();

    string GetFileMd5()
    {
        return m_sMd5;
    }  

    string GetLuaFileName()
    {
        return m_LuaFileName;
    }

    // 根据链接信息的key值获取列表的数据(只能获取最外层的
    std::vector<LUAKEYVALUE> GetLinkInfoByKey(std::string sKey);

    const std::vector<LUAKEYVALUE>& GetListData()
    {
        return m_vValueLists;
    }

    // 保存一维表数据
    bool UpdateData(const test_2::save_lua_list_data_request& proto);

    // 加载lua配置的数据
    bool LoadLuaConfigData();

    // 把数据写入到lua文件中
    void DumpListDataToConfigFile();

    // 根据外围信息表格式化展开lua表
    void DumpListDataFormatToConfigFile();
private:

    // 把Lua堆栈里面的lua表解析成格式化的字符串
    string DumpLuaTableToStream(string sExtTableKey, LuaExtInfoContainer* extInfo, lua_State* L, int nLevel);

    // 把一个lua表格式的字符串通过外围信息表格式化写入到ofstream中
    string DumpLuaTableStrValueToStream(string sTableKey, string sExtTableKey, string sValue, LuaExtInfoContainer* extInfo, lua_State* L, int nLevel);

    // 根据最外层的key在文件内的顺序给数据集合排序
    void SortValueListsByKeySquence();
private:
    string m_LuaFileName;   //lua文件的名字
    string m_LuaFilePath;   //lua文件的路径
    string m_sMd5;    //该文件的md5码

    std::vector<LUAKEYVALUE> m_vValueLists;     //数据集合
};
