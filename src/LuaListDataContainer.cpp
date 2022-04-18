#include "LuaListDataContainer.h"

// extern lua_State* L;

LuaListDataContainer::LuaListDataContainer(string sLuaFileName, string sLuaFilePath) : m_LuaFileName(sLuaFileName), m_LuaFilePath(sLuaFilePath)
{

}

LuaListDataContainer::~LuaListDataContainer()
{
	// free data
}

// 计算文件的MD5码
string LuaListDataContainer::CalculateFileMd5()
{
    ifstream ifs(m_LuaFilePath.c_str());
    MD5* md5 = new MD5(ifs);
    if (md5)
    {
        return md5->toString();
    }

    return "";
}

std::vector<LUAKEYVALUE> LuaListDataContainer::GetLinkInfoByKey(std::string sKey)
{
    std::vector<LUAKEYVALUE> vLinkFieldInfo;
    for (int i = 0; i < m_vValueLists.size(); ++i)
    {
        LUAKEYVALUE luaKeyValue = m_vValueLists[i];
        if (luaKeyValue.sKey == sKey)
        {
            if(luaKeyValue.fieldType == LUA_TTABLE)
            {
                lua_State* L = luaL_newstate();
                if (L)
                {
                    string sTempTable = "temp_table = " + luaKeyValue.sValue;
                    int ret = luaL_dostring(L, sTempTable.c_str());
                    std::vector<LUAKEYVALUE> vKeyValue;
                    std::vector<LUAKEYVALUE> vArrayValue;

                    if (ret == 0)
                    {
                        lua_getglobal(L, "temp_table");
                        if (!lua_istable(L, -1)) return vLinkFieldInfo;

                         //置空栈顶
                        lua_pushnil(L);
    
                        while(lua_next(L, -2))
                        {
                            int64_t nValueType = lua_type(L, -1);
                            int64_t nKeyType = lua_type(L, -2);

                            string sValue = lua_tostring(L, -1);
                            string sKey = "";

                            if (nKeyType == LUA_TSTRING)
                            {
                                sKey = lua_tostring(L, -2);
                            }
                            else if (nKeyType == LUA_TNUMBER)
                            {
                                sKey = std::to_string(lua_tointeger(L, -2));
                            }                            
                            
                            LUAKEYVALUE keyValue;
                            keyValue.sKey = sKey;
                            keyValue.sValue = sValue;
                    
                            if (nValueType == LUA_TNUMBER)
                            {
                                if (nKeyType == LUA_TSTRING)
                                {
                                    vKeyValue.push_back(keyValue);
                                }
                                else if (nKeyType == LUA_TNUMBER)
                                {
                                    vArrayValue.push_back(keyValue);
                                }
                            }
    
                            lua_pop(L, 1);
                        }

                        if (vKeyValue.size() > 0)
                        {
                            // 根据表格的额外信息进行键的排序
                            std::map<string, LuaExtInfoContainer*>* mTableInfoMap = LuaConfigManager::GetInstance()->GetTableInfoMap();
                            if (mTableInfoMap)
                            {                                
                                auto iter = mTableInfoMap->find(m_LuaFileName);
                                if (iter != mTableInfoMap->end())
                                {                                    
                                    std::map<string, int> mFieldSquence;
                                    std::map<string, string> mFieldDesc;
                                    FIELDSQUENCE* squence = iter->second->GetFieldSquenceDataByKey(sKey);
                                    if (squence)
                                    {
                                        for (int j = 0; j < squence->vSFieldSquences.size(); ++j)
                                        {
                                            mFieldSquence.insert(pair<string, int> (squence->vSFieldSquences[j].sFieldName, j));
                                            mFieldDesc.insert(pair<string, string> (squence->vSFieldSquences[j].sFieldName, squence->vSFieldSquences[j].sFieldAnnonation));
                                        }
                        
                                        sort(vKeyValue.begin(), vKeyValue.end(), [mFieldSquence](const LuaKeyValue& a, const LuaKeyValue& b)
                                        {
                                            auto iterA = mFieldSquence.find(a.sKey);
                                            auto iterB = mFieldSquence.find(b.sKey);
                                            int nFactorA = 9999;
                                            int nFactorB = 9999;
                                    
                                            if (iterA != mFieldSquence.end())
                                            {
                                                nFactorA = iterA->second;
                                            }
                                    
                                            if (iterB != mFieldSquence.end())
                                            {
                                                nFactorB = iterB->second;
                                            }
                                    
                                    
                                            return nFactorA < nFactorB;
                                        });
                                    }
    
                                    for (auto data : vKeyValue)
                                    {
                                        LUAKEYVALUE keyValue;
                                        keyValue.sKey = data.sValue;

                                        // 填充备注信息
                                        if (mFieldDesc.find(data.sKey) != mFieldDesc.end())
                                        {
                                            keyValue.sValue = mFieldDesc.find(data.sKey)->second;
                                        }
    
                                        vLinkFieldInfo.push_back(keyValue);
                                    }
                                }
                            }
                        }
                        else if (vArrayValue.size() > 0)
                        {
                            sort(vArrayValue.begin(), vArrayValue.end(), [](const LUAKEYVALUE& a, const LUAKEYVALUE& b)
                            {
                                return a.sKey < b.sKey;
                            });
    
                            for (auto data : vArrayValue)
                            {
                                LUAKEYVALUE keyValue;
                                keyValue.sKey = data.sValue;
                                vLinkFieldInfo.push_back(keyValue);
                            }
                        }
                    }
    
                    lua_close(L);
                }
            }
        }
    }

    return vLinkFieldInfo;
}

string LuaListDataContainer::DumpLuaTableToStream(string sTableKey, string sValue, LuaExtInfoContainer* extInfo, lua_State* L, int nLevel)
{
    string sTempTableName = "temp_table = " + sValue;
    int ret = luaL_dostring(L, sTempTableName.c_str());
    if (ret)
    {
        LOG_ERROR("temp_table is a error lua format, key = " + sTableKey + " value = " + sValue);
        return "";
    }

    lua_getglobal(L, "temp_table");

    if (!lua_istable(L, -1))
    {
        LOG_ERROR("temp_table is not a lua table, key = " + sTableKey + " value = " + sValue);        
        return "";
    }

    //置空栈顶
    lua_pushnil(L);

    std::vector<LuaKeyValue> vKeyValue;
    while(lua_next(L, -2))
    {
        int nKeyType = lua_type(L, -2);
        int nValueType = lua_type(L, -1);
        string sKey = "";

        if (nKeyType == LUA_TNUMBER)
        {            
            sKey = std::to_string(lua_tonumber(L, -2));
        }
        else
        {
            sKey = lua_tostring(L, -2);
        }

        LUAKEYVALUE keyValue;
        keyValue.sKey = sKey;
        keyValue.keyType = nKeyType;
        keyValue.fieldType = nValueType;
        keyValue.sValue = nValueType;

        stringstream ss;

        switch(nValueType)
        {
            case LUA_TSTRING:
            {
                string sValue = lua_tostring(L, -1);
                sValue = subreplace(sValue, "\n", "\\n");
                sValue = subreplace(sValue, "\"", "\\\"");

                ss << "\"" << sValue << "\"";
                break;
            }
            case LUA_TBOOLEAN:
            {
                if (lua_toboolean(L, -1) == 0)
                {
                    ss << "false";
                }
                else
                {
                    ss << "true";   
                }
                break;
            }
            case LUA_TNIL:
            {
                ss << "nil";
                break;
            }
            case LUA_TNUMBER:
            {
                string sValue = std::to_string(lua_tonumber(L, -1));
                ss << sValue;
                break;
            }
            case LUA_TTABLE:
            {
                // if (sValue == "")
                // {
                //     ofs << "nil";
                // }
                // else
                // {
                //     ofs << "{" << endl;
                //     string sTableValue = DumpLuaTableToStream(sKey, sValue, extInfo, L, nLevel + 1);
                //     ofs << sTableValue;
                //     ofs << TAB << "}";
                // }
                break;
            }
            default:
            {
                break;
            }
        }

        if (extInfo && nKeyType != LUA_TNUMBER)
        {
            FIELDSQUENCE* fieldSquence = extInfo->GetFieldSquenceDataByKey(sTableKey);
            if(fieldSquence)
            {
                for (int j = 0; j < fieldSquence->vSFieldSquences.size(); ++j)
                {
                    string sFieldName = fieldSquence->vSFieldSquences[j].sFieldName;
                    if (sFieldName == sKey)
                    {
                        string sAnnonation = fieldSquence->vSFieldSquences[j].sFieldAnnonation;
                        if (sAnnonation != "")
                        {
                            for (int i = 0; i < nLevel; ++i)
                            {
                                ss << TAB;
                            }
                            ss << "-- " << sAnnonation << endl;
                        }

                        break;
                    }
                }
            }
        }

        for (int i = 0; i < nLevel; ++i)
        {
            ss << TAB;
        }
        ss << sKey << " = " << endl;

        lua_pop(L, 1);
    }

    return "";
    // return ss.str();  
}

void LuaListDataContainer::DumpListDataFormatToConfigFile()
{
    ofstream ofs;
    //1.打开文件，如果没有，会在同级目录下自动创建该文件
    // ofs.open(m_LuaFilePath, ios::out);
    ofs.open("/root/workspace/dev_ph/script/config/tmp_pay_activity_config.lua", ios::out);

    timeval p;
    gettimeofday(&p, NULL);

    struct tm* ptm = localtime (&(p.tv_sec));
    char time_string[40];

    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", ptm);

    char title[256];
    sprintf(title, AUTO_GEN_FILE_DESC, time_string);
    
    ofs << title << endl;

    string sGlobalLuaTableName = m_LuaFileName;

    //2.写入一维表数据
    ofs << sGlobalLuaTableName << " = " << "{" << endl;

    LuaExtInfoContainer* extInfo = NULL;
    std::map<string, LuaExtInfoContainer*>* mTableInfoMap = LuaConfigManager::GetInstance()->GetTableInfoMap();
    if (mTableInfoMap)
    {
        auto iter = mTableInfoMap->find(m_LuaFileName);
        if (iter != mTableInfoMap->end())
        {
            extInfo = iter->second;
        }
    }

    lua_State *L = luaL_newstate();

    for (int i = 0; i < m_vValueLists.size(); ++i)
    {
        LUAKEYVALUE luaKeyValue = m_vValueLists[i];

        if (extInfo)
        {
            FIELDSQUENCE* fieldSquence = extInfo->GetFieldSquenceDataByKey("###field_sequence###");
            if(fieldSquence)
            {
                for (int j = 0; j < fieldSquence->vSFieldSquences.size(); ++j)
                {
                    string sFieldName = fieldSquence->vSFieldSquences[j].sFieldName;
                    if (sFieldName == luaKeyValue.sKey)
                    {
                        string sAnnonation = fieldSquence->vSFieldSquences[j].sFieldAnnonation;
                        if (sAnnonation != "")
                        {
                            ofs << TAB << "-- " << sAnnonation << endl;
                        }

                        break;
                    }
                }
            }
        }

        ofs << TAB << luaKeyValue.sKey << " = ";

        std::string sKey = luaKeyValue.sKey;
        std::string sValue = luaKeyValue.sValue;
        switch(luaKeyValue.fieldType)
        {
            case LUA_TSTRING:
            {
                sValue = subreplace(sValue, "\n", "\\n");
                sValue = subreplace(sValue, "\"", "\\\"");
                ofs << "\"" << sValue << "\"";
                break;
            }
            case LUA_TBOOLEAN:
            {
                ofs << sValue;
                break;
            }
            case LUA_TNIL:
            {
                ofs << "nil";
                break;
            }
            case LUA_TNUMBER:
            {
                if (sValue == "")
                {
                    ofs << "0";
                }
                else
                {
                    ofs << sValue;
                }
                break;
            }
            case LUA_TTABLE:
            {
                if (sValue == "")
                {
                    ofs << "nil";
                }
                else
                {
                    ofs << "{" << endl;
                    string sTableValue = DumpLuaTableToStream(sKey, sValue, extInfo, L, 2);
                    ofs << sTableValue;
                    ofs << TAB << "}";
                }
                break;
            }
            default:
            {
                break;
            }
        }
        
        ofs << "," << endl;
        ofs << endl;
    }

    ofs << "}";
    //5.关闭流
    ofs.close();

    lua_close(L);
}

void LuaListDataContainer::DumpListDataToConfigFile()
{
    ofstream ofs;
    //1.打开文件，如果没有，会在同级目录下自动创建该文件
    ofs.open(m_LuaFilePath, ios::out);
    
    timeval p;
    gettimeofday(&p, NULL);

    struct tm* ptm = localtime (&(p.tv_sec));
    char time_string[40];

    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", ptm);

    char title[256];
    sprintf(title, AUTO_GEN_FILE_DESC, time_string);
    
    ofs << title << endl;

    string sGlobalLuaTableName = m_LuaFileName;

    //2.写入一维表数据
    ofs << sGlobalLuaTableName << " = " << "{" << endl;

    for (int i = 0; i < m_vValueLists.size(); ++i)
    {
        LUAKEYVALUE luaKeyValue = m_vValueLists[i];

        ofs << TAB << luaKeyValue.sKey << " = ";

        std::string sValue = luaKeyValue.sValue;

        switch(luaKeyValue.fieldType)
        {
            case LUA_TSTRING:
            {
                sValue = subreplace(sValue, "\n", "\\n");
                sValue = subreplace(sValue, "\"", "\\\"");
                ofs << "\"" << sValue << "\"";
                break;
            }
            case LUA_TBOOLEAN:
            {
                ofs << sValue;
                break;
            }
            case LUA_TNIL:
            {
                ofs << "nil";
                break;
            }
            case LUA_TNUMBER:
            {
                if (sValue == "")
                {
                    ofs << "0";
                }
                else
                {
                    ofs << sValue;
                }
                break;
            }
            case LUA_TTABLE:
            {
                if (sValue == "")
                {
                    ofs << "nil";
                }
                else
                {
                    ofs << sValue;
                }
                break;
            }
            default:
            {
                break;
            }
        }
        
        ofs << "," << endl;

        if (i < m_vValueLists.size() - 1)
        {
            if (m_vValueLists[i + 1].fieldType == LUA_TTABLE)
            {
                ofs << endl;
            }
        }
    }

    ofs << "}";
    //5.关闭流
    ofs.close();
}

// 读取lua配置到一个容器中
bool LuaListDataContainer::LoadLuaConfigData()
{
    lua_State *L = luaL_newstate();
    if (!L) return false;

    int ret = luaL_dofile(L, m_LuaFilePath.c_str());
    if (ret)
    {
        string error = lua_tostring(L,-1);
        LOG_ERROR(error);
        lua_close(L);
        L = NULL;
        return false;
    }
    else
    {
        LOG_INFO("load lua file success : " + m_LuaFilePath);
    }

    string sGlobalLuaTableName = m_LuaFileName;

    lua_getglobal(L, sGlobalLuaTableName.c_str());
    if (!lua_istable(L, -1))
    {
        LOG_ERROR("file data is not a table : " + m_LuaFilePath);
    	cout << "is not a table, "<< sGlobalLuaTableName << endl;
        lua_close(L);
        L = NULL;
    	return false;
    }

    m_vValueLists.clear();

    //置空栈顶
    lua_pushnil(L);

    while(lua_next(L, -2))
    {
        int nValueType = lua_type(L, -1);

        LUAKEYVALUE keyValue;
        keyValue.sKey = lua_tostring(L, -2);

        string sValue;
        if (nValueType == LUA_TTABLE)
        {
            keyValue.sValue = LuaTableDataContainer::ParseLuaTableToString(m_LuaFileName, L, keyValue.sKey);
        }
        else if (nValueType == LUA_TSTRING)
        {
            keyValue.sValue = lua_tostring(L, -1);
        }
        else if (nValueType == LUA_TNIL)
        {
            keyValue.sValue = "nil";
        }
        else if (nValueType == LUA_TNUMBER)
        {
            // double num = lua_tonumber(L, -1);
            // std::string str_num = doubleToString(num);
            // keyValue.sValue = str_num;

            keyValue.sValue = lua_tostring(L, -1);
        }
        else if (nValueType == LUA_TBOOLEAN)
        {
            if (lua_toboolean(L, -1) == 0)
            {
                keyValue.sValue = "false";
            }
            else
            {
                keyValue.sValue = "true";   
            }
        }

        keyValue.fieldType = nValueType;

    	lua_pop(L, 1);

        m_vValueLists.push_back(keyValue);
    }

    lua_close(L);
    L = NULL;

    SortValueListsByKeySquence();

    m_sMd5 = CalculateFileMd5();

    return true;
}

void LuaListDataContainer::SortValueListsByKeySquence()
{
    if (m_vValueLists.size() <= 0) return;

    ifstream ifs;
    //1.打开文件，如果没有，会在同级目录下自动创建该文件
    ifs.open(m_LuaFilePath, ios::in);
    
    std::map<std::string, unsigned int> mFieldRawSquence;
    int nRow = 1;
    while(ifs.peek() != EOF)
    {
        nRow++;
        char buffer[1024 * 1024];
        ifs.getline(buffer, 1024 * 1024);

        char* cPtr = buffer;
        
        string sKey;
        while(*cPtr != '\0')
        {
            if (*cPtr != ' ' && *cPtr != '\t')
            {
                sKey.push_back(*cPtr);
            }

            if (*(++cPtr) == '=')
            {
                break;
            }
        }
        
        mFieldRawSquence.insert(pair <std::string, unsigned int> (sKey, nRow));
    }

    ifs.close();

    //根据原生顺序进行排序
    sort(m_vValueLists.begin(), m_vValueLists.end(), [mFieldRawSquence](const LUAKEYVALUE& a, const LUAKEYVALUE& b)
            {
                unsigned int nFactorA = 0;
                unsigned int nFactorB = 0;
                if (mFieldRawSquence.find(a.sKey) != mFieldRawSquence.end())
                {
                    nFactorA = mFieldRawSquence.find(a.sKey)->second;
                }

                if (mFieldRawSquence.find(b.sKey) != mFieldRawSquence.end())
                {
                    nFactorB = mFieldRawSquence.find(b.sKey)->second;
                }
                
                return nFactorA < nFactorB;
            });
}

bool LuaListDataContainer::UpdateData(const test_2::save_lua_list_data_request& proto)
{
    m_vValueLists.clear();

    for (int i = 0; i < proto.filed_types_size();++i)
    {
        LUAKEYVALUE value;

        test_2::field_type_key_value fieldTypeKeyValue = proto.filed_types(i);
        value.sKey = fieldTypeKeyValue.key();
        value.sValue = fieldTypeKeyValue.value();
        value.fieldType = fieldTypeKeyValue.type();

        m_vValueLists.push_back(value);
    }

    // DumpListDataToConfigFile();
    DumpListDataFormatToConfigFile();
    m_sMd5 = CalculateFileMd5();

    return true;
}
