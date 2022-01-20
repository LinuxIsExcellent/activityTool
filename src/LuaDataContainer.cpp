#include "LuaDataContainer.h"


LuaDataContainer::LuaDataContainer(string sLuaFileName, string sLuaFilePath) : m_LuaFileName(sLuaFileName), m_LuaFilePath(sLuaFilePath)
{

}

LuaDataContainer::~LuaDataContainer()
{
	// free data
}

string LuaDataContainer::GetStrData()
{
    string output;
    _table.SerializeToString(&output);
    
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
            sKey = std::to_string(lua_tointeger(L, -2));
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

    _table.set_table_name(m_LuaFileName);

    //置空栈顶
    lua_pushnil(L);

    int nRow = 0;
    int nColumn = 0;

    // 默认读的是二维表
    // 目前只支持二维表，所以第一层key的读取用int
    test_2::row_data* row_lists;
    while(lua_next(L, -2))
    {
    	int nKey = lua_tonumber(L, -2);
        
        row_lists = _table.add_row_lists();
        row_lists->set_key(std::to_string(nKey));

    	if (lua_type(L, -1) == LUA_TTABLE)
        {
        	lua_pushnil(L);

            int nColumn_ = 0;
        	while(lua_next(L, -2))
    		{
    			string sKey = lua_tostring(L, -2);
                string sValue;
    			// cout << "sKey = " << sKey << endl;

                test_2::pair_value* pairValue = row_lists->add_pair();
    			// 如果key值是一个table
    			if (lua_type(L, -1) == LUA_TTABLE)
    			{
    				// ParseLuaTableToString(L);
    				sValue = ParseLuaTableToString(L);
    			}
    			else if (lua_type(L, -1) == LUA_TSTRING)
    			{
    				sValue = lua_tostring(L, -1);
    			}
    			else if (lua_type(L, -1) == LUA_TBOOLEAN)
    			{
    				sValue = std::to_string(lua_toboolean(L, -1));
    			}
    			else if (lua_type(L, -1) == LUA_TNIL)
    			{
    				sValue = std::to_string(lua_tointeger(L, -1));
    			}
    			else if (lua_type(L, -1) == LUA_TNUMBER)
    			{
                    char str[64];
                    sprintf(str, "%g", lua_tonumber(L, -1));

                    sValue = str;
    			}

                pairValue->set_key(sKey);
                pairValue->set_value(sValue);

    			lua_pop(L, 1);
                nColumn_ += 1;
    		}

            nColumn = MAX(nColumn_, nColumn);
        }

    	lua_pop(L, 1);
        nRow += 1;
    }

    _table.set_row_count(nRow);
    _table.set_column_count(nColumn);

    // std::string protoStr;
    // google::protobuf::TextFormat::PrintToString(_table, &protoStr);
    // LOG_INFO(protoStr);

    // string output;
    // _table.SerializeToString(&output);
    // LOG_ERROR(output);

    return true;
}
