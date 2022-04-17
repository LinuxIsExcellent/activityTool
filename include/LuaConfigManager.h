#pragma once
#include "include.h"
#include <map>

class LuaTableDataContainer;
class LuaExtInfoContainer;
class LuaListDataContainer;

// LuaTableDataContainer类管理器
class LuaConfigManager
{
private:
    LuaConfigManager() {};
public:
    ~LuaConfigManager() {};

    static LuaConfigManager* GetInstance(){
        if (m_instance == nullptr)
        {
            m_instance = new LuaConfigManager();
        }

        return m_instance;
    }

    // 检查配置文件是否有改变
    void CheckConfigFileIsChange();

    // 加载所有的lua配置(二维数组)
    void LoadAllLuaConfigData();

    // 加载所有的键值对一维表lua配置
    void LoadLuaListConfigData();

    // 加载所有文件的中间配置
    void LoadAllLuaTempConfigData();

    // 释放资源
    void FreeData();

    string GetLuaTableDataByName(string name, string sLinkInfo);

    std::map<string, LuaTableDataContainer*>* GetTableDataMap()
    {
        return &m_mDataMap;
    }

    std::map<string, LuaExtInfoContainer*>* GetTableInfoMap()
    {
        return &m_mTableInfoMap;
    }

    string GetLuaListDataByName(string name, string sLinkInfo = "");

    std::map<string, LuaListDataContainer*>* GetLuaListMap()
    {
        return &m_mLuaListDataMap;
    }
private:
    static LuaConfigManager* m_instance;

    std::map<string, LuaTableDataContainer*> m_mDataMap;

    std::map<string, LuaListDataContainer*> m_mLuaListDataMap;

    std::map<string, LuaExtInfoContainer*> m_mTableInfoMap;
};
