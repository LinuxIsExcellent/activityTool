#include "LuaListDataContainer.h"


LuaListDataContainer::LuaListDataContainer(string sLuaFileName, string sLuaFilePath) : m_LuaFileName(sLuaFileName), m_LuaFilePath(sLuaFilePath)
{

}

LuaListDataContainer::~LuaListDataContainer()
{
	// free data
}

void LuaListDataContainer::DumpListDataToConfigFile()
{
    ofstream ofs;
    //1.打开文件，如果没有，会在同级目录下自动创建该文件
    ofs.open(m_LuaFilePath, ios::out);//采取追加的方式写入文件
    
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
        
        ofs << "," << endl;
    }

    ofs << "}";
    //5.关闭流
    ofs.close();
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
            double num = lua_tonumber(L, -1);
            std::string str_num = doubleToString(num);
            keyValue.sValue = str_num;

            // char str[64];
            // sprintf(str, "%g", lua_tonumber(L, -1));
            // keyValue.sValue = str;
        }
        else if (lua_type(L, -1) == LUA_TBOOLEAN)
        {
            keyValue.sValue = std::to_string(lua_toboolean(L, -1));
        }

        keyValue.fieldType = lua_type(L, -1);

    	lua_pop(L, 1);

        m_vValueLists.push_back(keyValue);
    }

    SortValueListsByKeySquence();

    return true;
}

void LuaListDataContainer::SortValueListsByKeySquence()
{
    if (m_vValueLists.size() <= 0) return;

    ifstream ifs;
    //1.打开文件，如果没有，会在同级目录下自动创建该文件
    ifs.open(m_LuaFilePath, ios::in);//采取追加的方式写入文件

    int nRow = 1;
    while(ifs.peek() != EOF)
    {
        nRow++;
        char buffer[1024];
        ifs.getline(buffer, 1024);

        char* cPtr = buffer;
        bool bIsNewKey = true;

        std::cout << "nRow = " << nRow << std::endl;

        while(*cPtr != '\0')
        {
            if (*cPtr != ' ')
            {
                std::cout << "当前行的字符" <<  *cPtr << std::endl;
            }

            if (*(++cPtr) == '=')
            {
                break;
            }
        }

        // std::cout << "当前行的数据: " << buffer << std::endl;
    }

    // ifs.seekg(0, ifs.end);
    // int nLength = ifs.tellg();
    // ifs.seekg(0, ifs.beg);

    // char* buffer = new char [nLength];
    // ifs.read(buffer, nLength);

    // string sFile(buffer, nLength);

    LOG_INFO("全局数据加载成功");

    ifs.close();
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

    DumpListDataToConfigFile();
    return true;
}