cmake_minimum_required(VERSION 3.10)
project(MapReduce)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-DBOOST_ERROR_CODE_HEADER_ONLY -DBOOST_SYSTEM_NO_DEPRECATED -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-lboost_system -lboost_filesystem")

find_package(Boost 1.65.1 REQUIRED COMPONENTS filesystem)

add_executable(mapreduce main.cpp mapreduce.h mapreduce.cpp constants.h)

add_executable(map map.cpp constants.h)

add_executable(reduce reduce.cpp constants.h)
