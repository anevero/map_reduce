cmake_minimum_required(VERSION 3.15)
project(MapReduce)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-DBOOST_ERROR_CODE_HEADER_ONLY -DBOOST_SYSTEM_NO_DEPRECATED -Wall -Wextra")

add_executable(mapreduce main.cpp mapreduce.h mapreduce.cpp)

add_executable(map map.cpp)

add_executable(reduce reduce.cpp)
