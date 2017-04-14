cmake_minimum_required(VERSION 3.7)
project(Warp10Server)

set(CMAKE_CXX_STANDARD 11)
include_directories(SYSTEM ulib/include)
include_directories(SYSTEM warp10server/include)

set(SOURCE_FILES
        ulib/include/
        ulib/src/
        warp10server/app/warp10server.cpp
        warp10server/include/
        warp10server/src/)

add_executable(Warp10Server ${SOURCE_FILES})