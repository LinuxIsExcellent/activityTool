#include "LuaDataContainer.h"


LuaDataContainer::LuaDataContainer()
{

}

LuaDataContainer::~LuaDataContainer()
{

}

// 读取lua配置到一个容器中
bool LuaDataContainer::LoadLuaConfigData(lua_State* L, string path, string name)
{
    if (!L) return false;

    int ret = luaL_dofile(L, path.c_str());
    if (ret)
    {
        cout << "load file error" << endl;
        return false;
    }


    return false;
}
