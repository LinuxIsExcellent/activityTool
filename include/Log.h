#pragma once

#include <include.h>

class Log
{
public:
    static Log* getInstance();
    static Log* logger;

    static void destroy();

    void error(string& msg);
    void warn(string& msg);
    void debug(string& msg);
    void info(string& msg);
private:
    Log();
    void init();

    static Log* _mLogger;

    log4cpp::Category* _mCate;
};

#define postfix(msg) (string("["))\
    .append(__FILE__).append(":").append(__func__)\
    .append(":").append(to_string(__LINE__)).append("] ").append(msg)


#define LOG_ERROR(msg) Log::logger->error(postfix(msg))
#define LOG_WARN(msg) Log::logger->warn(postfix(msg))
#define LOG_INFO(msg) Log::logger->info(postfix(msg))
#define LOG_DEBUG(msg) Log::logger->debug(postfix(msg))