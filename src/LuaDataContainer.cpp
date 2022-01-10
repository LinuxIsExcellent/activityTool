#include "LuaDataContainer.h"


LuaDataContainer::LuaDataContainer(string sLuaFileName, string sLuaFilePath) : m_LuaFileName(sLuaFileName), m_LuaFilePath(sLuaFilePath)
{

}

LuaDataContainer::~LuaDataContainer()
{
	// free data
}

// 把lua栈中的栈顶元素解析成lua格式的字符串
string LuaDataContainer::ParseLuaTableToString(lua_State *L)
{
	string sValueTable = "{";
    if (!lua_type(L, -1) == LUA_TTABLE)
    {
        cout << "is not a string" << endl;
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
        cout << "load file error" << endl;
        return false;
    }
    else
    {
    	cout << "load file success : " << m_LuaFilePath << endl;
    }

    string sLuaTableName = "dataconfig_" + m_LuaFileName;
    test_2::table_info table;

    lua_getglobal(L, sLuaTableName.c_str());
    if (!lua_istable(L, -1))
    {
    	cout << "is not a table, "<< sLuaTableName << endl;
    	return false;
    }
    cout << "lua table = " << sLuaTableName << endl;

    //置空栈顶
    lua_pushnil(L);

    // 默认读的是二维表
    while(lua_next(L, -2))
    {
    	int nKey = lua_tonumber(L, -2);
    	cout << nKey << endl;

    	if (lua_type(L, -1) == LUA_TTABLE)
        {
        	lua_pushnil(L);
        	while(lua_next(L, -2))
    		{
    			string sKey = lua_tostring(L, -2);
    			cout << "sKey = " << sKey << endl;

    			// 如果key值是一个table
    			if (lua_type(L, -1) == LUA_TTABLE)
    			{
    				// ParseLuaTableToString(L);
    				cout << "table value = " << ParseLuaTableToString(L) << endl;
    			}
    			else if (lua_type(L, -1) == LUA_TSTRING)
    			{
    				cout << "sValue = " << lua_tostring(L, -1) << endl;
    			}
    			else if (lua_type(L, -1) == LUA_TBOOLEAN)
    			{
    				cout << "sValue = " << lua_toboolean(L, -1) << endl;
    			}
    			else if (lua_type(L, -1) == LUA_TNIL)
    			{
    				cout << "sValue = " << lua_tointeger(L, -1) << endl;
    			}
    			else if (lua_type(L, -1) == LUA_TNUMBER)
    			{
    				cout << "sValue = " << lua_tonumber(L, -1) << endl;
    			}

    			lua_pop(L, 1);
    		}
        }

    	lua_pop(L, 1);
    }

    return false;
}
