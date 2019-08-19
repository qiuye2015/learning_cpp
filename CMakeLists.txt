cmake_minimum_required(VERSION 2.8)
project(http_server)

## ------------C++11支持-----CMake 3.1后可用CMAKE_CXX_STANDARD代替
# 检查c++编译器标志，设置c++11支持变量
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

# 使用变量设置编译标志
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#设定编译参数 cmake3.1之后才有 CMAKE_CXX_STANDARD
#set(CMAKE_CXX_STANDARD 11)
## -------------------------------------------------------------

# set make cmd
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS}  -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE  -O0 -W -g -lpthread")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS}  -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE -O3 -w -lpthread")

set(CMAKE_BUILD_TYPE "Debug")

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    MESSAGE(STATUS "Debug Mode")
    MESSAGE(STATUS "Flags:" ${CMAKE_CXX_FLAGS_DEBUG} )
ELSEIF (CMAKE_BUILD_TYPE STREQUAL "Release")
    MESSAGE(STATUS "Release Mode")
    MESSAGE(STATUS "Flags:" ${CMAKE_CXX_FLAGS_RELEASE})
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# set boost libraries, static xxx.a in [boost/lib/]
set(BOOST_LIBS
boost_thread.a
boost_regex.a
)
# 使用gperftools对程序进行性能分析需要连接tcmalloc和profiler库
set(LD_LIBS
tcmalloc
profiler
glog
)

#设定源码列表.cpp
#set(SOURCE_FILES ./main.cc)
#设定所有源码列表 ：
aux_source_directory(. DIR_SRCS)
aux_source_directory(./common COMMON_SRCS)
aux_source_directory(./server3 HTTP_SERVER_SRCS)
 
#设定头文件路径 相当于指定gcc的-I参数 
include_directories(.)
#include_directories("路径1"  “路径2”...)
 
#设定链接库的路径（一般使用第三方非系统目录下的库）相当于gcc的-L参数 
link_directories($ENV{BOOST_HOME}\\lib)
#link_directories("路径1"  “路径2”...)
 
#添加子目录,作用相当于进入子目录里面，展开子目录的CMakeLists.txt
#同时执行，子目录中的CMakeLists.txt一般是编译成一个库，作为一个模块
#在父目录中可以直接引用子目录生成的库
#add_subdirectory(math)
 
#生成动/静态库
#add_library(    动/静态链接库名称      SHARED/STATIC(可选，默认STATIC)      源码列表    )
#可以单独生成多个模块
 
#生成可执行文件
#add_executable(myLevealDB   ${SOURCE_FILES} )
add_executable(http_server 
${DIR_SRCS}
${COMMON_SRCS}
${HTTP_SERVER_SRCS}
)

# 添加链接库，相同于指定-l参数  
target_link_libraries(http_server ${BOOST_LIBS})
target_link_libraries(http_server ${LD_LIBS})

#SET(EXECUTABLE_OUTPUT_PATH ./bin)
