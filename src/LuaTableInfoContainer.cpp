#include "LuaTableInfoContainer.h"


LuaTableInfoContainer::LuaTableInfoContainer(string sLuaFileName, string sLuaFilePath) : m_LuaFileName(sLuaFileName), m_LuaFilePath(sLuaFilePath)
{

}

LuaTableInfoContainer::~LuaTableInfoContainer()
{
	// free data
}

string LuaTableInfoContainer::GetStrData()
{
    string output;
    _table_info.SerializeToString(&output);
    
    return output;
}

 void LuaTableInfoContainer::SplitSequenceKeyToNumVector(const std::string str, std::vector<string>& v, const char* delimiter /*= "_"*/)
{
	if (delimiter == nullptr || str.empty())
		return;

	string buf(str);

	size_t pos = std::string::npos;
	std::string subStr;

	int delimiterLength = strlen(delimiter);

	while(true)
	{
		pos = buf.find(delimiter);
		if (pos != std::string::npos)
		{
			subStr = buf.substr(0, pos);
			if (!subStr.empty())
				v.push_back(subStr);

			buf = buf.substr(pos + delimiterLength);
		}
		else
		{
			if (!buf.empty())
				v.push_back(buf);
			break;
		}
	}
}

bool LuaTableInfoContainer::LoadTableInfoData(lua_State* L)
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

    string sLuaTableName = m_LuaFileName + "_TABLE_INFO";

    lua_getglobal(L, sLuaTableName.c_str());
    if (!lua_istable(L, -1))
    {
        LOG_ERROR("file data is not a table : " + m_LuaFilePath);
    	return false;
    }

    //置空栈顶
    lua_pushnil(L);

    while(lua_next(L, -2))
    {

    	FIELDSQUENCE fieldSquence;

    	string sKey = lua_tostring(L, -2);

    	std::vector<string> sVector;
    	SplitSequenceKeyToNumVector(sKey, sVector);

    	// 如果field_sequence后面没有数字,则就是最外层的二维表的顺序
    	if(sVector.size() > 2)
    	{
    		for(int i = 2; i < sVector.size(); ++i)
    		{
    			fieldSquence.vNLevels.push_back(atoi(sVector[i].c_str()));
    			LOG_INFO("string key num = " + sVector[i]);
    		}
    	}

    	// 读取table里面的field
    	if (lua_type(L, -1) == LUA_TTABLE)
        {
        	lua_pushnil(L);

        	while(lua_next(L, -2))
    		{
    			if (lua_type(L, -1) == LUA_TSTRING)
    			{
    				fieldSquence.vSFieldSquences.push_back(lua_tostring(L, -1));
    			}
    			lua_pop(L, 1);
    		}
    	}

    	m_vFieldSquences.push_back(fieldSquence);

    	lua_pop(L, 1);
    }

    return true;
}
