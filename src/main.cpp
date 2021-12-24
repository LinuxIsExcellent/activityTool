#include "include.h"
#include "globalConfig.h"
#include "IOManager.h"

// 全局配置单例
GlobalConfig* GlobalConfig::m_instance = NULL;
// 全局唯一虚拟机
static lua_State *L = NULL;
// 全局I/O管理器
IOManager* IOManager::m_instance = NULL;

int main()
{
    // 一些初始化的工作
    L = luaL_newstate();
    if(L == NULL)
    {
        return 0;
    }

    //加载全局配置表
    GlobalConfig::GetInstance()->LoadConfig(L, "../config/global_config.lua");

    // 初始化IO管理器
    IOManager::GetInstance()->InitIOManager();
    // 监听ip和端口
    IOManager::GetInstance()->AddListeningFd(GlobalConfig::GetInstance()->getListeningIp(), GlobalConfig::GetInstance()->getListeningPort());
    // 开始IO循环
    IOManager::GetInstance()->Loop();

    return 0;
}
