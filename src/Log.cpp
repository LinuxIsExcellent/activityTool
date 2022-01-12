#include <exception>

#include "Log.h"

Log* Log::_mLogger = Log::getInstance();    //初始化静态变量
Log* Log::logger = Log::getInstance();

Log* Log::getInstance()
{
    if(_mLogger == nullptr)
    {
        _mLogger = new Log();
    }

    return _mLogger;
}

Log::Log()
{
    this->init();
}

void Log::init()
{
    try
    {
        log4cpp::PropertyConfigurator::configure("../config/Mylog.conf");
    }
    catch(log4cpp::ConfigureFailure & f)
    {
        std::cerr << "configure problem " << f.what() << std::endl;
    }

    _mCate = &log4cpp::Category::getInstance("rollingFile");
    // _mCate->info("myLogger init");
}

void Log::error(string& msg)
{
    _mCate->error(msg);
}

void Log::warn(string& msg)
{
    _mCate->warn(msg);
}
void Log::debug(string& msg)
{
    _mCate->debug(msg);
}
void Log::info(string& msg)
{
    _mCate->info(msg);
}

void Log::destroy()
{
    if(_mLogger)
    {
        // _mLogger->_mCate->info("Mylog destroyed");
        _mLogger->_mCate->shutdown();

        delete _mLogger;
        _mLogger = nullptr;
    }
}
