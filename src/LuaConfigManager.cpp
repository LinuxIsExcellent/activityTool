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

string LuaConfigManager::GetLuaDataByName(string name)
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
            std::vector<FIELDSQUENCE> vFieldSquences = iter1->second->GetFieldQquenceData();
            for (int i = 0; i < vFieldSquences.size(); ++i)
            {
                FIELDSQUENCE squence = vFieldSquences[i];
                test_2::field_squence* fieldSquence = table_data_proto.add_filed_sequences();
                if (fieldSquence)
                {
                    for (int j = 0; j < squence.vNLevels.size(); ++j)
                    {
                        fieldSquence->add_levels(squence.vNLevels[j]);
                    }

                    for (int j = 0; j < squence.vSFieldSquences.size(); ++j)
                    {
                        std::string* fieldStr = fieldSquence->add_fields();
                        if(fieldStr)
                        {
                            *fieldStr = squence.vSFieldSquences[j];
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

        LuaDataContainer* luaData = new LuaDataContainer(sFileTableName, sLuaFileAbsolutePath);
        if (luaData)
        {
            if (luaData->LoadLuaConfigData(L))
            {
                LOG_INFO("加载lua数据成功 : " + sFileTableName);
                m_mDataMap.insert(pair<string, LuaDataContainer*> (sFileTableName, luaData));
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

        LuaTableInfoContainer* container = new LuaTableInfoContainer(sFileTableName, sLuaFileAbsolutePath);
        if (container)
        {
            if (container->LoadTableInfoData(L))
            {
                LOG_INFO("加载lua二维表信息成功 : " + sFileTableName);
            }
        }

        m_mTableInfoMap.insert(pair<string, LuaTableInfoContainer*> (sFileTableName, container));
    }
}