#include "LuaExtInfoContainer.h"


LuaExtInfoContainer::LuaExtInfoContainer(string sLuaFileName, string sLuaFilePath) : m_LuaFileName(sLuaFileName), m_LuaFilePath(sLuaFilePath)
{

}

LuaExtInfoContainer::~LuaExtInfoContainer()
{
	// free data
}

 void LuaExtInfoContainer::SplitSequenceKeyToNumVector(const std::string str, std::vector<string>& v, const char* delimiter /*= "_"*/)
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

void LuaExtInfoContainer::DumpTableInfoToConfigFile()
{
	ofstream ofs;
    //3.打开文件，如果没有，会在同级目录下自动创建该文件
    ofs.open(m_LuaFilePath, ios::out);//采取追加的方式写入文件

    string sLuaTableName = m_LuaFileName + "_TABLE_INFO";

    ofs << sLuaTableName << " =" << endl;
    ofs << "{" << endl;

    // 写入表的字段顺序信息
    for (auto data : m_vFieldSquences)
    {
    	std::string vSFieldSquencesKey = "field_sequence";
    	if (data.vNLevels.size() > 0)
    	{
    		for (auto nData : data.vNLevels)
    		{
    			vSFieldSquencesKey = vSFieldSquencesKey + "_" + std::to_string(nData);
    		}
    	}

    	ofs << TAB << vSFieldSquencesKey << " =" << endl;

    	ofs << TAB << "{" << endl;
    	for (int i = 1; i <= data.vSFieldSquences.size(); ++i)
    	{
    		ofs << TAB << TAB << "[" << i << "]" << " = " << "\"" << data.vSFieldSquences[i - 1] << "\","<< endl;
    	}

    	ofs << TAB << "}," << endl;
    }

    ofs << "}" << endl;

    //5.关闭流
    ofs.close();
}

bool LuaExtInfoContainer::LoadTableInfoData(lua_State* L)
{
    
    if (!L) return false;

    int ret = luaL_dofile(L, m_LuaFilePath.c_str());
    if (ret)
    {
        string error = lua_tostring(L,-1);
        // LOG_ERROR(error);
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

void LuaExtInfoContainer::UpdateData(const test_2::client_save_table_info_request& quest)
{
	m_vFieldSquences.clear();

	for (int i = 0; i < quest.filed_sequences_size(); i++)
	{
		FIELDSQUENCE fieldSquence;
		test_2::field_squence field_squence = quest.filed_sequences(i);

		for (int j = 0; j < field_squence.levels_size(); j++)
		{
			uint16_t nLevel = field_squence.levels(j);

			fieldSquence.vNLevels.push_back(nLevel);
		}

		for (int j = 0; j < field_squence.fields_size(); j++)
		{
			std::string sField = field_squence.fields(j);

			fieldSquence.vSFieldSquences.push_back(sField);
		}

		m_vFieldSquences.push_back(fieldSquence);
	}

	DumpTableInfoToConfigFile();

	// 重新给二维表的最外层数据排序
	std::map<string, LuaTableDataContainer*>* tableDataMap = LuaConfigManager::GetInstance()->GetTableDataMap();
	if(tableDataMap)
	{
		auto iter = tableDataMap->find(m_LuaFileName);
		if (iter != tableDataMap->end())
		{
			iter->second->SortFieldSquence();
		}
	}
}