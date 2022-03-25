#include "LuaConfigManager.h"

void LuaConfigManager::FreeData()
{
    for(auto it = m_mDataMap.begin(); it != m_mDataMap.end(); it++)
    {
        LuaTableDataContainer *luaData = it->second;
        delete luaData;

        luaData = NULL;
    }

    for(auto it = m_mLuaListDataMap.begin(); it != m_mLuaListDataMap.end(); it++)
    {
        LuaListDataContainer *luaData = it->second;
        delete luaData;

        luaData = NULL;
    }

    for(auto it = m_mTableInfoMap.begin(); it != m_mTableInfoMap.end(); it++)
    {
        LuaExtInfoContainer *luaData = it->second;
        delete luaData;

        luaData = NULL;
    }
}

string LuaConfigManager::GetLuaListDataByName(string name)
{
    auto iter = m_mLuaListDataMap.find(name);
    if (iter != m_mLuaListDataMap.end())
    {
        const std::vector<LUAKEYVALUE> listData = iter->second->GetListData();

        test_2::send_lua_list_data_notify notify;
        notify.set_table_name(name);

        for (int i = 0; i < listData.size(); ++i)
        {
            test_2::field_type_key_value* key_value = notify.add_filed_types();
            if (key_value)
            {
                key_value->set_key(listData[i].sKey);
                key_value->set_value(listData[i].sValue);
                key_value->set_type(listData[i].fieldType);
            }
        }

        // 再填充数据表的外围信息
        auto iter1 = m_mTableInfoMap.find(name);
        if (iter1 != m_mTableInfoMap.end())
        {
            std::map<std::string, FIELDSQUENCE> mFieldSquences = iter1->second->GetFieldQquenceData();
            for (auto loopIter = mFieldSquences.begin(); loopIter != mFieldSquences.end(); ++loopIter)
            {
                test_2::field_squence* fieldSquence = notify.add_filed_sequences();
                if (fieldSquence)
                {
                    fieldSquence->set_index(loopIter->first);

                    FIELDSQUENCE squence = loopIter->second;
                    for (int j = 0; j < squence.vSFieldSquences.size(); ++j)
                    {
                        test_2::field_info* fieldInfo = fieldSquence->add_infos();
                        if(fieldInfo)
                        {
                            fieldInfo->set_field_name(squence.vSFieldSquences[j].sFieldName);
                            fieldInfo->set_field_desc(squence.vSFieldSquences[j].sFieldAnnonation);
                            fieldInfo->set_field_link(squence.vSFieldSquences[j].sFieldLink);
                        }
                    }
                }
                
            }
        }

        string output;
        notify.SerializeToString(&output);
        return output;
    }

    return "";
}

string LuaConfigManager::GetLuaTableDataByName(string name)
{
    auto iter = m_mDataMap.find(name);
    if (iter != m_mDataMap.end())
    {
        const TABLEDATA tableData = iter->second->GetTableData();
        const std::vector<std::string> feildStr = iter->second->GetVFeildStr();
        const std::map<std::string, int> feildType = iter->second->GetVFeildTypes();
        
        test_2::table_data table_data_proto;
        table_data_proto.set_table_name(name);
        table_data_proto.set_row_count(tableData.nRow);
        table_data_proto.set_column_count(tableData.nColumn);

        for (int16_t i = 0; i < tableData.dataList.size(); ++i)
        {
            test_2::row_data* row_lists = table_data_proto.add_row_lists();
            if(row_lists)
            {
                ROWDATA rowData = tableData.dataList[i];
                row_lists->set_key(std::to_string(rowData.id));
    
                for (int16_t j = 0; j < rowData.dataList.size(); ++ j)
                {
                    test_2::pair_value* pairValue = row_lists->add_pair();
                    if (pairValue)
                    {
                        VALUEPAIR pairData = rowData.dataList[j];
    
                        pairValue->set_key(pairData.sField);
                        pairValue->set_value(pairData.sValue);
                    }
                }
            }
        }
    
        // 根据配置表读出来的字段
        for (int i = 0; i < feildStr.size(); ++i)
        {
            std::string* feildName = table_data_proto.add_filed_names();
            if(feildName)
            {
                *feildName = feildStr[i];

                if (feildType.find(*feildName) != feildType.end())
                {
                    test_2::field_type_pair* filed_types = table_data_proto.add_filed_types();
                    if (filed_types)
                    {
                        filed_types->set_key(*feildName);
                        filed_types->set_lua_type(feildType.find(*feildName)->second);
                    }
                }
            }
        }

        // 再填充数据表的外围信息
        auto iter1 = m_mTableInfoMap.find(name);
        if (iter1 != m_mTableInfoMap.end())
        {
            std::map<std::string, FIELDSQUENCE> mFieldSquences = iter1->second->GetFieldQquenceData();
            for (auto loopIter = mFieldSquences.begin(); loopIter != mFieldSquences.end(); ++loopIter)
            {
                test_2::field_squence* fieldSquence = table_data_proto.add_filed_sequences();
                if (fieldSquence)
                {
                    fieldSquence->set_index(loopIter->first);

                    FIELDSQUENCE squence = loopIter->second;
                    for (int j = 0; j < squence.vSFieldSquences.size(); ++j)
                    {
                        test_2::field_info* fieldInfo = fieldSquence->add_infos();
                        if(fieldInfo)
                        {
                            fieldInfo->set_field_name(squence.vSFieldSquences[j].sFieldName);
                            fieldInfo->set_field_desc(squence.vSFieldSquences[j].sFieldAnnonation);
                            fieldInfo->set_field_link(squence.vSFieldSquences[j].sFieldLink);
                        }
                    }
                }
                
            }
        }

        string output;
        table_data_proto.SerializeToString(&output);
        return output;
    }

    return "";
}


void LuaConfigManager::LoadLuaListConfigData(lua_State *L)
{
    if (!L) return;

    std::vector<string> fileList = GlobalConfig::GetInstance()->GetListenLuaListFileList();
    string sConfigDirPath = GlobalConfig::GetInstance()->getConfigPath();
    if (fileList.size() <= 0) return ;

    for (int i = 0; i < fileList.size(); ++i)
    {
        string sFileTableName = fileList[i];
        string sLuaFileAbsolutePath = sConfigDirPath + "/" + fileList[i];

        // 处理字符串得出lua配置文件的名字
        int nBeginPos = sFileTableName.rfind("/") + 1;
        int nEndPos = sFileTableName.find(".lua");

        sFileTableName = sFileTableName.substr(nBeginPos, nEndPos - nBeginPos);

        LuaListDataContainer* luaData = new LuaListDataContainer(sFileTableName, sLuaFileAbsolutePath);
        if (luaData)
        {
            if (luaData->LoadLuaConfigData(L))
            {
                LOG_INFO("加载lua数据成功 : " + sFileTableName);
                m_mLuaListDataMap.insert(pair<string, LuaListDataContainer*> (sFileTableName, luaData));
            }
        }
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
        string sFileTableName = fileList[i];
        string sLuaFileAbsolutePath = sConfigDirPath + "/" + fileList[i];

        // 处理字符串得出lua配置文件的名字
        int nBeginPos = sFileTableName.rfind("/") + 1;
        int nEndPos = sFileTableName.find(".lua");

        sFileTableName = sFileTableName.substr(nBeginPos, nEndPos - nBeginPos);

        LuaTableDataContainer* luaData = new LuaTableDataContainer(sFileTableName, sLuaFileAbsolutePath);
        if (luaData)
        {
            LOG_INFO("加载lua数据开始 : " + sFileTableName);
            if (luaData->LoadLuaConfigData(L))
            {
                LOG_INFO("加载lua数据成功 : " + sFileTableName);
                m_mDataMap.insert(pair<string, LuaTableDataContainer*> (sFileTableName, luaData));
            }
        }
    }
}

void LuaConfigManager::LoadAllLuaTempConfigData(lua_State *L)
{
    if (!L) return;

    std::vector<string> fileList = GlobalConfig::GetInstance()->GetListenLuaFileList();
    string sTempConfigDirPath = GlobalConfig::GetInstance()->getTempConfigPath();

    if (fileList.size() <= 0)
    {
        LOG_INFO("没有配置任何的lua配置文件");
        return;
    }

    for (int i = 0; i < fileList.size(); ++i)
    {
        string sFileTableName = fileList[i];
        // 处理字符串得出lua配置文件的名字
        int nBeginPos = sFileTableName.rfind("/") + 1;
        int nEndPos = sFileTableName.find(".lua");

        sFileTableName = sFileTableName.substr(nBeginPos, nEndPos - nBeginPos);

        string fileName = sFileTableName + "_TABLE_INFO.lua";
        string sLuaFileAbsolutePath = sTempConfigDirPath + "/" + fileName;

        LuaExtInfoContainer* container = new LuaExtInfoContainer(sFileTableName, sLuaFileAbsolutePath);
        if (container)
        {
            if (container->LoadTableInfoData(L))
            {
                LOG_INFO("加载lua二维表信息成功 : " + sFileTableName);
            }
        }

        m_mTableInfoMap.insert(pair<string, LuaExtInfoContainer*> (sFileTableName, container));
    }

    std::vector<string> LuaListFileInfoList = GlobalConfig::GetInstance()->GetListenLuaListFileList();
    for (int i = 0; i < LuaListFileInfoList.size(); ++i)
    {
        string sFileTableName = LuaListFileInfoList[i];
        // 处理字符串得出lua配置文件的名字
        int nBeginPos = sFileTableName.rfind("/") + 1;
        int nEndPos = sFileTableName.find(".lua");

        sFileTableName = sFileTableName.substr(nBeginPos, nEndPos - nBeginPos);

        string fileName = sFileTableName + "_TABLE_INFO.lua";
        string sLuaFileAbsolutePath = sTempConfigDirPath + "/" + fileName;

        LuaExtInfoContainer* container = new LuaExtInfoContainer(sFileTableName, sLuaFileAbsolutePath);
        if (container)
        {
            if (container->LoadTableInfoData(L))
            {
                LOG_INFO("加载lua二维表信息成功 : " + sFileTableName);
            }
        }

        m_mTableInfoMap.insert(pair<string, LuaExtInfoContainer*> (sFileTableName, container));
    }
}