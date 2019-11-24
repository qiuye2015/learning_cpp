cmake_minimum_required(VERSION 2.8)
project(TestSharedLib)

# C++11 编译
set(CMAKE_CXX_STANDARD 11)

# 头文件路径
set(INC_DIR ../)
# 库文件路径
set(LIB_DIR ../)

include_directories(${INC_DIR})
link_directories(${LIB_DIR})
link_libraries(MySharedLib)

add_executable(TestSharedLib main.cpp)

# 链接 MySharedLib 库
target_link_libraries(TestSharedLib MySharedLib)
