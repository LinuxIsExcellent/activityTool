#include "include.h"

// 全局配置单例
GlobalConfig* GlobalConfig::m_instance = NULL;

// Lua配置管理器
LuaConfigManager* LuaConfigManager::m_instance = NULL;
// 全局唯一虚拟机
static lua_State *L = NULL;
// 全局I/O管理器
IOManager* IOManager::m_instance = NULL;

string subreplace(string resource_str, string sub_str, string new_str)
{
    string::size_type last_pos = 0;
    string::size_type pos = resource_str.find_first_of(sub_str, last_pos);

    while(pos != string::npos)   //替换所有指定子串
    {
        // 替换
        resource_str.replace(pos, sub_str.length(), new_str);

        // 移动位置，避免重复循环替换
        last_pos = pos + new_str.length();

        pos = resource_str.find_first_of(sub_str, last_pos);
    }
    return resource_str;
}

bool string_contains(string resource_str, string sub_str)
{
    string::size_type pos = resource_str.find(sub_str);
    if (pos != string::npos)
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::string doubleToString(double price) {
    auto res = std::to_string(price);
    const std::string format("$1");
    try {
        std::regex r("(\\d*)\\.0{6}|");
        std::regex r2("(\\d*\\.{1}0*[^0]+)0*");
        res = std::regex_replace(res, r2, format);
        res = std::regex_replace(res, r, format);
    }
    catch (const std::exception & e) {
        return res;
    }
    return res;
}

vector<string> split(string strtem,char a)
{
    vector<string> strvec;
 
    string::size_type pos1, pos2;
    pos2 = strtem.find(a);
    pos1 = 0;
    while (string::npos != pos2)
    {
        strvec.push_back(strtem.substr(pos1, pos2 - pos1));
 
        pos1 = pos2 + 1;
        pos2 = strtem.find(a, pos1);
    }
    
    strvec.push_back(strtem.substr(pos1));
    return strvec;
}

// 全局5s定时器
void Global5STimer()
{
    // LOG_INFO("global 5s timer");

    if (L)
    {
        IOManager::GetInstance()->ProcessMonitorHandler();
    }
}

// 全局15s定时器
void Global15STimer()
{
    // LOG_INFO("global 15s timer");
    LuaConfigManager::GetInstance()->CheckConfigFileIsChange(L);
}

void daemon_run()
{
    int pid;
    signal(SIGCHLD, SIG_IGN);

    pid = fork();
    if (pid < 0)
    {
        std::cout << "fork error" << std::endl;
        exit(-1);
    }
    else if (pid > 0)
    {
        exit(0);
    }

    setsid();
    int fd;
    fd = open("/dev/null", O_RDWR, 0);
    if(fd != -1)
    {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
    }
    if(fd > 2)
    {
        close(fd);
    }
}

void DumpPidToFile()
{
    int nCurrentPid = getpid();
    ofstream ofs;
    //1.打开文件，如果没有，会在同级目录下自动创建该文件
    ofs.open("./observer.pid", ios::out);

    ofs << nCurrentPid;
    ofs.close();
}

int main()
{
    //daemon_run();
    
    DumpPidToFile();

    // 一些初始化的工作
    L = luaL_newstate();
    if(L == NULL)
    {
        return 0;
    }

    //加载全局配置表
    GlobalConfig::GetInstance()->LoadConfig(L, "../config/global_config.lua");

    //加载中间所有配置的中间文件
    LuaConfigManager::GetInstance()->LoadAllLuaTempConfigData(L);

    //加载所有lua配置数据
    LuaConfigManager::GetInstance()->LoadAllLuaConfigData(L);

    // 加载所有的lua键值对一维表数据
    LuaConfigManager::GetInstance()->LoadLuaListConfigData(L);

    // 初始化IO管理器
    IOManager::GetInstance()->InitIOManager();
    // 监听ip和端口
    IOManager::GetInstance()->AddListeningFd(GlobalConfig::GetInstance()->getListeningIp(), GlobalConfig::GetInstance()->getListeningPort());

    time_t cur_time = time(NULL);

    util_timer* timer_5s = new util_timer();
    timer_5s->expire = cur_time;
    timer_5s->nLoopSec = 5;
    // timer_5s->nLoopMax = 1;
    timer_5s->cb_func = Global5STimer;

    IOManager::GetInstance()->AddTimer(timer_5s);

    util_timer* timer_15s = new util_timer();
    timer_15s->expire = cur_time;
    timer_15s->nLoopSec = 15;
    timer_15s->cb_func = Global15STimer;

    IOManager::GetInstance()->AddTimer(timer_15s);

    // 开始IO循环
    IOManager::GetInstance()->Loop();

    // IO循环结束，释放全局资源
    LuaConfigManager::GetInstance()->FreeData();

    // 释放lua虚拟机
    lua_close(L);
    L = NULL;

    return 0;
}
