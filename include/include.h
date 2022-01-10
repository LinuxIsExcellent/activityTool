// #pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <stringbuffer.h>

#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/PropertyConfigurator.hh>

#include <enum.h>
#include <Log.h>
#include <globalConfig.h>
#include <msg.pb.h>
#include <LuaConfigManager.h>
#include <LuaDataContainer.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>

using namespace std;

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
