#include "LuaListDataContainer.h"


LuaListDataContainer::LuaListDataContainer(string sLuaFileName, string sLuaFilePath) : m_LuaFileName(sLuaFileName), m_LuaFilePath(sLuaFilePath)
{

}

LuaListDataContainer::~LuaListDataContainer()
{
	// free data
}

// 读取lua配置到一个容器中
bool LuaListDataContainer::LoadLuaConfigData(lua_State* L)
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

    string sGlobalLuaTableName = m_LuaFileName;

    lua_getglobal(L, sGlobalLuaTableName.c_str());
    if (!lua_istable(L, -1))
    {
        LOG_ERROR("file data is not a table : " + m_LuaFilePath);
    	cout << "is not a table, "<< sGlobalLuaTableName << endl;
    	return false;
    }

    //置空栈顶
    lua_pushnil(L);

    // 默认读的是二维表
    // 目前只支持二维表，所以第一层key的读取用int
    while(lua_next(L, -2))
    {
        LUAKEYVALUE keyValue;
        keyValue.sKey = lua_tostring(L, -2);

        string sValue;
        if (lua_type(L, -1) == LUA_TTABLE)
        {
            keyValue.sValue = LuaTableDataContainer::ParseLuaTableToString(L);
        }
        else if (lua_type(L, -1) == LUA_TSTRING)
        {
            keyValue.sValue = lua_tostring(L, -1);
        }
        else if (lua_type(L, -1) == LUA_TNIL)
        {
            keyValue.sValue = std::to_string(lua_tointeger(L, -1));
        }
        else if (lua_type(L, -1) == LUA_TNUMBER)
        {
            char str[64];
            sprintf(str, "%g", lua_tonumber(L, -1));

            keyValue.sValue = str;
        }
        else if (lua_type(L, -1) == LUA_TBOOLEAN)
        {
            keyValue.sValue = std::to_string(lua_toboolean(L, -1));
        }

        keyValue.fieldType = lua_type(L, -1);

    	lua_pop(L, 1);

        m_vValueLists.push_back(keyValue);
    }

    return true;
}