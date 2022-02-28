#include "LuaDataContainer.h"


LuaDataContainer::LuaDataContainer(string sLuaFileName, string sLuaFilePath) : m_LuaFileName(sLuaFileName), m_LuaFilePath(sLuaFilePath)
{

}

LuaDataContainer::~LuaDataContainer()
{
	// free data
}

string LuaDataContainer::GetProtoDataStr()
{
    test_2::table_data table_data_proto;
    table_data_proto.set_table_name(m_LuaFileName);
    table_data_proto.set_row_count(m_table_data.nRow);
    table_data_proto.set_column_count(m_table_data.nColumn);

    for (int16_t i = 0; i < m_table_data.dataList.size(); ++i)
    {
        test_2::row_data* row_lists = table_data_proto.add_row_lists();
        if(row_lists)
        {
            ROWDATA rowData = m_table_data.dataList[i];
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
    
    for (int i = 0; i < m_vFeildStrs.size(); ++i)
    {
        std::string* feildName = table_data_proto.add_filed_names();
        if(feildName)
        {
            *feildName = m_vFeildStrs[i];
        }
        table_data_proto.add_filed_types(m_vFeildTypes[i]);
    }

    string output;
    table_data_proto.SerializeToString(&output);
    return output;
}

// 把lua栈中的栈顶元素解析成lua格式的字符串
string LuaDataContainer::ParseLuaTableToString(lua_State *L)
{
	string sValueTable = "{";
    if (!lua_type(L, -1) == LUA_TTABLE)
    {
        LOG_ERROR("is not a string");
        return "";
    }

    lua_pushnil(L);
    bool has_field = false;
    while(lua_next(L, -2))
    {
        string sKey = "";

        if (lua_type(L, -2) == LUA_TNUMBER || lua_type(L, -2) == LUA_TNIL)
        {
            sKey = "[" + std::to_string(lua_tointeger(L, -2)) + "]";
        }
        else if (lua_type(L, -2) == LUA_TSTRING)
        {
            sKey = lua_tostring(L, -2);
        }

        sValueTable = sValueTable + sKey + " = ";

        // 如果key值是一个table
        if (lua_type(L, -1) == LUA_TTABLE)
        {
            sValueTable = sValueTable + ParseLuaTableToString(L);
        }
        else if (lua_type(L, -1) == LUA_TSTRING)
        {
            sValueTable = sValueTable + lua_tostring(L, -1);
            // cout << "sValue = " << lua_tostring(L, -1) << endl;
        }
        else if (lua_type(L, -1) == LUA_TBOOLEAN)
        {
            sValueTable = sValueTable + std::to_string(lua_toboolean(L, -1));
            // cout << "sValue = " << lua_toboolean(L, -1) << endl;
        }
        else if (lua_type(L, -1) == LUA_TNIL)
        {
            sValueTable = sValueTable + std::to_string(lua_tointeger(L, -1));
        }
        else if (lua_type(L, -1) == LUA_TNUMBER)
        {
            char str[64];
            sprintf(str, "%g", lua_tonumber(L, -1));

            sValueTable = sValueTable + str;
        }

        sValueTable = sValueTable + ", ";
        lua_pop(L, 1);

        has_field = true;
    }

    // 最后一个", "要去掉，暂时找不到怎么判断lua_next中的元素全部遍历结束的方法，如果有办法判断可以在lua_next里面加方法处理
    if(has_field)
    {
        sValueTable = sValueTable.erase(sValueTable.length() - 2, 2);
    }

    sValueTable = sValueTable + "}";
    return sValueTable;
}

// 读取lua配置到一个容器中
bool LuaDataContainer::LoadLuaConfigData(lua_State* L)
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

    string sLuaTableName = "dataconfig_" + m_LuaFileName;

    lua_getglobal(L, sLuaTableName.c_str());
    if (!lua_istable(L, -1))
    {
        LOG_ERROR("file data is not a table : " + m_LuaFilePath);
    	cout << "is not a table, "<< sLuaTableName << endl;
    	return false;
    }

    m_table_data.sTableName = m_LuaFileName;

    std::map<string, int> mFeildStrs;
    m_vFeildTypes.clear();
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
    				// ParseLuaTableToString(L);
    				sValue = ParseLuaTableToString(L);
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
                    char str[64];
                    sprintf(str, "%g", lua_tonumber(L, -1));

                    sValue = str;
    			}

                VALUEPAIR pair_value;
                pair_value.sField = sKey;
                pair_value.sValue = sValue;

                // 记录一个读取的字段顺序，为了确保每行的数据展示是一致的
                if (mFeildStrs.find(sKey) == mFeildStrs.end())
                {
                    mFeildStrs.insert(pair<string, int> (sKey, mFeildStrs.size() + 1));

                    m_vFeildStrs.push_back(sKey);
                    m_vFeildTypes.push_back(test_2::DATA_TYPE(nValueType));
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

    m_table_data.nRow = nRow;
    m_table_data.nColumn = nColumn;

    return true;
}
