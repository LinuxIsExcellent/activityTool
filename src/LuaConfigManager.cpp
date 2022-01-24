#include "LuaConfigManager.h"

void LuaConfigManager::FreeData()
{
    for(auto it = m_mDataMap.begin(); it != m_mDataMap.end(); it++)
    {
        LuaDataContainer *luaData = it->second;
        delete luaData;

        luaData = NULL;
    }
}

void LuaConfigManager::LoadAllLuaConfigData(lua_State *L)
{
    if (!L) return;

    std::vector<string> fileList = GlobalConfig::GetInstance()->GetListenLuaFileList();
    string sConfigDirPath = GlobalConfig::GetInstance()->getConfigPath();
    if (fileList.size() <= 0) return ;

    for (int i = 0; i < fileList.size(); ++i)
    {
        string sFileName = fileList[i];
        string sLuaFileAbsolutePath = sConfigDirPath + "/" + fileList[i];

        // 处理字符串得出lua配置文件的名字
        int nBeginPos = sFileName.rfind("/") + 1;
        int nEndPos = sFileName.find(".lua");

        sFileName = sFileName.substr(nBeginPos, nEndPos - nBeginPos);

        LuaDataContainer* luaData = new LuaDataContainer(sFileName, sLuaFileAbsolutePath);
        if (luaData)
        {
            if (luaData->LoadLuaConfigData(L))
            {
                LOG_INFO("加载lua数据成功 : " + sFileName);
                m_mDataMap.insert(pair<string, LuaDataContainer*> (sFileName, luaData));
            }
        }
    }
}

string LuaConfigManager::GetLuaDataByName(string name)
{
    auto iter = m_mDataMap.find(name);
    if (iter != m_mDataMap.end())
    {
        return iter->second->GetStrData();
    }

    return "";
}

void LuaConfigManager::LoadAllLuaTempConfigData(lua_State *L)
{
    if (!L) return;

    string sTempConfigDirPath = GlobalConfig::GetInstance()->getTempConfigPath();

    if (m_mDataMap.size() <= 0)
    {
        LOG_INFO("没有配置任何的lua配置文件");
        return;
    }

    for(auto iter = m_mDataMap.begin(); iter != m_mDataMap.end(); ++iter)
    {
        string fileName = iter->first + "_TABLE_INFO.lua";
        string sLuaFileAbsolutePath = sTempConfigDirPath + "/" + fileName;

        LuaTableInfoContainer* container = new LuaTableInfoContainer(iter->first, sLuaFileAbsolutePath);
        if (container)
        {
            if (container->LoadTableInfoData(L))
            {
                LOG_INFO("加载lua二维表信息成功 : " + iter->first);
                m_mTableInfoMap.insert(pair<string, LuaTableInfoContainer*> (iter->first, container));
            }
        }
    }
}