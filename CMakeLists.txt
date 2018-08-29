cmake_minimum_required(VERSION 2.8)

project(SERV)

set(CMAKE_BUILD_TYPE "Debug") 
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -Wall")

add_subdirectory(src)
# message(STATUS "This is BINARY dir: " ${PROJECT_BINARY_DIR})
# message(STATUS "This is SOURCE dir: "  ${PROJECT_SOURCE_DIR})