#include "LuaTableDataContainer.h"


LuaTableDataContainer::LuaTableDataContainer(string sLuaFileName, string sLuaFilePath) : m_LuaFileName(sLuaFileName), m_LuaFilePath(sLuaFilePath)
{

}

LuaTableDataContainer::~LuaTableDataContainer()
{
	// free data
}

// 把lua栈中的栈顶元素解析成lua格式的字符串
string LuaTableDataContainer::ParseLuaTableToString(lua_State *L, std::string sTableKey/* = ""*/)
{
    if (!lua_type(L, -1) == LUA_TTABLE)
    {
        LOG_ERROR("is not a string");
        return "";
    }

    string sValueTable = "{";

    std::vector<LuaKeyValue> vKeyValueData;
    std::vector<LUAARRAYVALUE> vArrayValueData;

    lua_pushnil(L);
    // LOG_INFO("lua_rawlen - " + std::to_string(lua_rawlen(L, -2)));
    while(lua_next(L, -2))
    {   
        std::string sValue = "";
        int16_t nValueType = lua_type(L, -1);
        int16_t nKeyType = lua_type(L, -2);

        if (nValueType == LUA_TTABLE)
        {
            std::string sSubTableKey = "";
            // 先判断是数组还是key—value
            if (nKeyType == LUA_TNUMBER || nKeyType == LUA_TNIL)
            {
                sSubTableKey = sTableKey + "%ARRAY";
            }
            else
            {
                sSubTableKey = sTableKey + "#" + lua_tostring(L, -2);
            }

            sValue = ParseLuaTableToString(L, sSubTableKey);
        }
        else if (nValueType == LUA_TSTRING)
        {
            sValue = std::string("\"") + lua_tostring(L, -1) + std::string("\"");
        }
        else if (nValueType == LUA_TBOOLEAN)
        {
            sValue = std::to_string(lua_toboolean(L, -1));
        }
        else if (nValueType == LUA_TNIL)
        {
            sValue = std::to_string(lua_tointeger(L, -1));
        }
        else if (nValueType == LUA_TNUMBER)
        {
            sValue = lua_tostring(L, -1);
        }

        if (nKeyType == LUA_TNUMBER || nKeyType == LUA_TNIL)
        {
            int16_t nKey = lua_tointeger(L, -2);

            LUAARRAYVALUE luaArrayValue;
            luaArrayValue.nKey = nKey;
            luaArrayValue.sValue = sValue;
            luaArrayValue.fieldType = nKeyType;

            vArrayValueData.push_back(luaArrayValue);
        }
        else if (nKeyType == LUA_TSTRING)
        {
            string strKey = lua_tostring(L, -2);

            LuaKeyValue luaKeyValue;
            luaKeyValue.sKey = strKey;
            luaKeyValue.sValue = sValue;
            luaKeyValue.fieldType = nKeyType;

            vKeyValueData.push_back(luaKeyValue);
        }

        lua_pop(L, 1);
    }
    
    // 键值对部分排序
    std::map<string, LuaExtInfoContainer*>* mTableInfoMap = LuaConfigManager::GetInstance()->GetTableInfoMap();
    if (mTableInfoMap)
    {
        auto iter = mTableInfoMap->find(m_LuaFileName);
        if (iter != mTableInfoMap->end())
        {
            std::map<string, int> mFieldSquence;
            FIELDSQUENCE* squence = iter->second->GetFieldSquenceDataByKey(sTableKey);
            if (squence)
            {
                for (int j = 0; j < squence->vSFieldSquences.size(); ++j)
                {
                    mFieldSquence.insert(pair<string, int> (squence->vSFieldSquences[j].sFieldName, j));
                }

                sort(vKeyValueData.begin(), vKeyValueData.end(), [mFieldSquence](const LuaKeyValue& a, const LuaKeyValue& b)
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
        }
    }

    // 数据部分进行排序
    sort(vArrayValueData.begin(), vArrayValueData.end(), [](const LUAARRAYVALUE& a, const LUAARRAYVALUE& b)
    {  
        return a.nKey < b.nKey;
    });

    // 判断数组部分的[key]是否可以省略
    int nFlag = 1;
    bool isCompleteArray = true;
    for (int i = 0; i < vArrayValueData.size(); ++i)
    {
        if (nFlag != vArrayValueData[i].nKey){
            isCompleteArray = false;
            break;
        }

        nFlag++;
    }

    // 填充数据，键值对在前，数组部分在后
    for (int i = 0; i < vKeyValueData.size(); ++i)
    {
        if (ISDIGIT(vKeyValueData[i].sKey))
        {
            sValueTable = sValueTable + std::string("[\"") + vKeyValueData[i].sKey + std::string("\"]");
        }
        else
        {
            sValueTable = sValueTable + vKeyValueData[i].sKey;
        }

        sValueTable = sValueTable + " = " + vKeyValueData[i].sValue;

        if (i < vKeyValueData.size() - 1 || vArrayValueData.size() > 0)
        {
            sValueTable = sValueTable + ", ";
        }
    }

    for (int i = 0; i < vArrayValueData.size(); ++i)
    {
        if (!isCompleteArray)
        {
            sValueTable = sValueTable + "[" + std::to_string(vArrayValueData[i].nKey) + "]" + " = ";
        }

        sValueTable = sValueTable + vArrayValueData[i].sValue;

        if (i < vArrayValueData.size() - 1)
        {
            sValueTable = sValueTable + ", ";
        }
    }

    sValueTable = sValueTable + "}";

    return sValueTable;
}

// 读取lua配置到一个容器中
bool LuaTableDataContainer::LoadLuaConfigData(lua_State* L)
{
    if (!L) return false;

    int ret = luaL_dofile(L, m_LuaFilePath.c_str());
    if (ret)
    {
        string error = lua_tostring(L,-1);
        LOG_ERROR(error);
        return false;
    }
    else
    {
        LOG_INFO("load lua file success : " + m_LuaFilePath);
    }

    string sGlobalLuaTableName = "dataconfig_" + m_LuaFileName;

    lua_getglobal(L, sGlobalLuaTableName.c_str());
    if (!lua_istable(L, -1))
    {
        LOG_ERROR("file data is not a table : " + m_LuaFilePath);
    	cout << "is not a table, "<< sGlobalLuaTableName << endl;
    	return false;
    }

    m_table_data.sTableName = m_LuaFileName;

    std::map<string, int> mFeildStrs;
    m_mFeildTypes.clear();
    //置空栈顶
    lua_pushnil(L);

    int nRow = 0;
    int nColumn = 0;

    // 默认读的是二维表
    // 目前只支持二维表，所以第一层key的读取用int
    while(lua_next(L, -2))
    {
    	int nKey = lua_tonumber(L, -2);

        ROWDATA row_data;
        row_data.id = nKey;

    	if (lua_type(L, -1) == LUA_TTABLE)
        {
        	lua_pushnil(L);

            int nColumn_ = 0;
        	while(lua_next(L, -2))
    		{
    			string sKey = lua_tostring(L, -2);
                string sValue;
    			// cout << "sKey = " << sKey << endl;

                int nValueType = lua_type(L, -1);

    			// 如果key值是一个table
    			if (nValueType == LUA_TTABLE)
    			{
                    sValue = ParseLuaTableToString(L, sKey);
    			}
    			else if (nValueType == LUA_TSTRING)
    			{
    				sValue = lua_tostring(L, -1);
    			}
    			else if (nValueType == LUA_TBOOLEAN)
    			{
    				sValue = std::to_string(lua_toboolean(L, -1));
    			}
    			else if (nValueType == LUA_TNIL)
    			{
    				sValue = std::to_string(lua_tointeger(L, -1));
    			}
    			else if (nValueType == LUA_TNUMBER)
    			{
                    sValue = lua_tostring(L, -1);
    			}

                VALUEPAIR pair_value;
                pair_value.sField = sKey;
                pair_value.sValue = sValue;

                // 记录一个读取的字段顺序，为了确保每行的数据展示是一致的
                if (mFeildStrs.find(sKey) == mFeildStrs.end())
                {
                    mFeildStrs.insert(pair<string, int> (sKey, mFeildStrs.size() + 1));

                    m_vFeildStrs.push_back(sKey);
                    m_mFeildTypes.insert(pair<string, int> (sKey, int(nValueType)));
                }

                row_data.dataList.push_back(pair_value);

    			lua_pop(L, 1);
                nColumn_ += 1;
    		}

            nColumn = MAX(nColumn_, nColumn);
            row_data.nFeildCount = nColumn;
        }

        m_table_data.dataList.push_back(row_data);

    	lua_pop(L, 1);
        nRow += 1;
    }

    sort(m_table_data.dataList.begin(), m_table_data.dataList.end(), [](const ROWDATA& a, const ROWDATA& b)
    {
        return a.id < b.id;
    });

    // 把最外层的字段先排序
    SortFieldSquence();

    m_table_data.nRow = nRow;
    m_table_data.nColumn = nColumn;

    return true;
}

void LuaTableDataContainer::SortFieldSquence()
{
    // 根据中间文件的外围数据，把字段重新排序一次
    std::map<string, LuaExtInfoContainer*>* mTableInfoMap = LuaConfigManager::GetInstance()->GetTableInfoMap();
    if (mTableInfoMap)
    {
        auto iter = mTableInfoMap->find(m_LuaFileName);
        if (iter != mTableInfoMap->end())
        {   
            std::map<string, int> mFieldSquence;
            FIELDSQUENCE* squence = iter->second->GetFieldSquenceDataByKey("field_sequence");
            if (squence)
            {
                for (int j = 0; j < squence->vSFieldSquences.size(); ++j)
                {
                    mFieldSquence.insert(pair<string, int> (squence->vSFieldSquences[j].sFieldName, j));
                }
            }

            sort(m_vFeildStrs.begin(), m_vFeildStrs.end(), [mFieldSquence](const string& a, const string& b)
            {
                auto iterA = mFieldSquence.find(a);
                auto iterB = mFieldSquence.find(b);
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
    }
}

void LuaTableDataContainer::DumpTableDataToConfigFile()
{
    ofstream ofs;
    //1.打开文件，如果没有，会在同级目录下自动创建该文件
    ofs.open(m_LuaFilePath, ios::out);//采取追加的方式写入文件
    
    string sGlobalLuaTableName = "dataconfig_" + m_LuaFileName;
    string sLocalLuaTableName = "local_dataconfig_" + m_LuaFileName;
    //2.写入文件标题
    char title[64];
    sprintf(title, "asddas %s", "asd");

    LOG_INFO("请求保存一维表数据");
    LOG_INFO("title" + std::string(title));

    // ofs << std::string(title) << endl;
    //3.写入配置数据
    ofs << sGlobalLuaTableName << " = " << "{}" << endl;
    ofs << endl;
    ofs << "local " << sLocalLuaTableName << " = " << sGlobalLuaTableName << endl;
    ofs << endl;

    for (int i = 0; i < m_table_data.dataList.size(); ++i)
    {
        ROWDATA rowData = m_table_data.dataList[i];

        sort(rowData.dataList.begin(), rowData.dataList.end(), [this](VALUEPAIR a, VALUEPAIR b)
        {
            int nFactorA = 9999;
            int nFactorB = 9999;
            for (int i = 0; i < this->m_vFeildStrs.size(); ++i)
            {
                if (this->m_vFeildStrs[i] == a.sField)
                {
                    nFactorA = i;
                }

                if (this->m_vFeildStrs[i] == b.sField)
                {
                    nFactorB = i;
                }
            }

            return nFactorA < nFactorB;
        });

        ofs << sLocalLuaTableName << "[" << rowData.id << "]" << " = { ";
        for (int j = 0; j < rowData.dataList.size(); ++j)
        {
            VALUEPAIR pair = rowData.dataList[j];
            string sValue = pair.sValue;
            string sField = pair.sField;

            ofs << sField << " = ";

            if (m_mFeildTypes.find(sField) != m_mFeildTypes.end())
            {
                switch(m_mFeildTypes.find(sField)->second)
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
                        if(sValue == "0" or sValue == "")
                        {
                            ofs << "false";
                        }
                        else
                        {
                            ofs << "true";
                        }
                        break;
                    }
                    case LUA_TNIL:
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
            }

            if (j != rowData.dataList.size() - 1)
            {
                 ofs << ", ";
            }
        }


        ofs << " }" << endl;
    }

    ofs << endl;
    ofs << "return " << sGlobalLuaTableName << endl;
    //5.关闭流
    ofs.close();
}

bool LuaTableDataContainer::UpdateData(const test_2::client_save_table_data_request& proto)
{
    m_table_data.dataList.clear();
    m_table_data.sTableName = proto.table_name(); 

    int nRow = 0;
    int nColumn = 0;
    for (int i = 0; i < proto.row_lists_size();++i)
    {
        test_2::row_data row_data = proto.row_lists(i);
        std::string row_key_str = row_data.key();

        int nRowKey = atoi(row_key_str.c_str());

        ROWDATA rowData;
        rowData.id = nRowKey;

        int nColumn_ = 0;
        for (int m = 0; m < row_data.pair_size(); ++m) {
            test_2::pair_value pair = row_data.pair(m);
            VALUEPAIR vPairs;

            vPairs.sField = pair.key();
            vPairs.sValue = pair.value();

            rowData.dataList.push_back(vPairs);

            nColumn_++;
        }

        nColumn = MAX(nColumn, nColumn_);
        nRow++;
        m_table_data.dataList.push_back(rowData);
    }

    m_table_data.nRow = nRow;
    m_table_data.nColumn = nColumn;

    DumpTableDataToConfigFile();   

    return true;
}
