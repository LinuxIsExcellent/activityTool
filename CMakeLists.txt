cmake_minimum_required(VERSION 2.8.12)

#设置工程名字
project(observer)

#设置lua静态库名字
set(target_name "lua")   # lua-5.4.3

#添加lua静态库
add_library(${target_name} STATIC
    thirdparty/lua/lapi.c
    thirdparty/lua/lauxlib.c
    thirdparty/lua/lbaselib.c
    thirdparty/lua/lcode.c
    thirdparty/lua/lcorolib.c
    thirdparty/lua/lctype.c
    thirdparty/lua/ldblib.c
    thirdparty/lua/ldebug.c
    thirdparty/lua/ldo.c
    thirdparty/lua/ldump.c
    thirdparty/lua/lfunc.c
    thirdparty/lua/lgc.c
    thirdparty/lua/linit.c
    thirdparty/lua/liolib.c
    thirdparty/lua/llex.c
    thirdparty/lua/lmathlib.c
    thirdparty/lua/lmem.c
    thirdparty/lua/loadlib.c
    thirdparty/lua/lobject.c
    thirdparty/lua/lopcodes.c
    thirdparty/lua/loslib.c
    thirdparty/lua/lparser.c
    thirdparty/lua/lstate.c
    thirdparty/lua/lstring.c
    thirdparty/lua/lstrlib.c
    thirdparty/lua/ltable.c
    thirdparty/lua/ltablib.c
    thirdparty/lua/ltm.c
    thirdparty/lua/lua.c
    thirdparty/lua/lundump.c
    thirdparty/lua/lutf8lib.c
    thirdparty/lua/lvm.c
    thirdparty/lua/lzio.c
    )

#包含lua文件路径
target_include_directories(lua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
 
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-E")

#添加编译选项
add_compile_options(-std=c++11)

#设置protobuf的文件路径
set(Protobuf_PREFIX_PATH
    "/usr/local/protobuf/include"
    "/usr/local/protobuf/lib"
    "/usr/local/protobuf/bin"
)

#编译的环境变量
list(APPEND CMAKE_PREFIX_PATH "${Protobuf_PREFIX_PATH}")

find_package(Protobuf REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

#指定可执行文件的输出目录，输出到bin下面  
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

#指定库文件输出路径  
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#设置proto文件
file(GLOB PROTOFILES
    "${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto")

#根据proto文件生成c++文件
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTOFILES})

#添加头文件
include_directories(include)
include_directories("thirdparty/rapidjson/")
include_directories("thirdparty/lua/")

#把src文件路径下的所有cpp文件都包含
aux_source_directory(src/ CPP_LIST)

add_executable(observer ${CPP_LIST} ${PROTO_HDRS} ${PROTO_SRCS})

#链接protobuf库
target_link_libraries(observer ${PROTOBUF_LIBRARIES})
#链接lua静态库
target_link_libraries(observer lua)
