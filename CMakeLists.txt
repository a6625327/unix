# 最低cmake版本
cmake_minimum_required(VERSION 2.8)

# 工程名
project(SERV)

# 使gdb可以调试
set(CMAKE_BUILD_TYPE "Debug") 
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -Wall")

# 增加子文件夹
add_subdirectory(src)
# message(STATUS "This is BINARY dir: " ${PROJECT_BINARY_DIR})
# message(STATUS "This is SOURCE dir: "  ${PROJECT_SOURCE_DIR})