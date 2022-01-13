#pragma once
#include "include.h"
#include <map>

class LuaDataContainer;

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

    // 释放资源
    void FreeData();

    string GetLuaDataByName(string name);
private:
    static LuaConfigManager* m_instance;

    std::map<string, LuaDataContainer*> m_mDataMap;
};
