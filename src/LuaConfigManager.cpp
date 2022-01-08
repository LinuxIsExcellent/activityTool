#include "LuaConfigManager.h"

void LuaConfigManager::FreeData()
{
    std::map<string, LuaDataContainer*>::iterator it;
    for(it = m_mDataMap.begin(); it != m_mDataMap.end(); it++)
    {
        LuaDataContainer *luaData = it->second;
        delete luaData;

        luaData = NULL;
    }
}

void LuaConfigManager::LoadAllLuaConfigData(lua_State *L)
{

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
            luaData->LoadLuaConfigData(L);
            m_mDataMap.insert(pair<string, LuaDataContainer*> (sLuaFileAbsolutePath, luaData));
        }
    }
}