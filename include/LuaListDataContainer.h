#pragma once
#include "include.h"

// Lua文件数据类
// 只支持键值对的lua表格式
class LuaListDataContainer
{
public:
    LuaListDataContainer(string sLuaFileName, string sLuaFilePath);
    ~LuaListDataContainer();

    // 根据链接信息的key值获取列表的数据(只能获取最外层的
    std::vector<LUAKEYVALUE> GetLinkInfoByKey(std::string sKey);

    const std::vector<LUAKEYVALUE>& GetListData()
    {
        return m_vValueLists;
    }

    // 保存一维表数据
    bool UpdateData(const test_2::save_lua_list_data_request& proto);

    // 加载lua配置的数据
    bool LoadLuaConfigData(lua_State* L);

    // 把数据写入到lua文件中
    void DumpListDataToConfigFile();
private:

    // 根据最外层的key在文件内的顺序给数据集合排序
    void SortValueListsByKeySquence();
private:
    string m_LuaFileName;   //lua文件的名字
    string m_LuaFilePath;   //lua文件的路径

    std::vector<LUAKEYVALUE> m_vValueLists;     //数据集合
};
