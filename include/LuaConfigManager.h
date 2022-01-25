#pragma once
#include "include.h"
#include <map>

class LuaDataContainer;
class LuaTableInfoContainer;

// LuaDataContainer类管理器
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

    // 加载所有的lua配置
    void LoadAllLuaConfigData(lua_State *L);

    // 加载所有文件的中间配置
    void LoadAllLuaTempConfigData(lua_State *L);

    // 释放资源
    void FreeData();

    string GetLuaDataByName(string name);

    std::map<string, LuaDataContainer*>* GetTableDataMap()
    {
        return &m_mDataMap;
    }

    std::map<string, LuaTableInfoContainer*>* GetTableInfoMap()
    {
        return &m_mTableInfoMap;
    }
private:
    static LuaConfigManager* m_instance;

    std::map<string, LuaDataContainer*> m_mDataMap;

    std::map<string, LuaTableInfoContainer*> m_mTableInfoMap;
};
