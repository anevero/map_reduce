cmake_minimum_required(VERSION 3.10)
project(MapReduce)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-DBOOST_ERROR_CODE_HEADER_ONLY -DBOOST_SYSTEM_NO_DEPRECATED -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-pthread -lboost_system -lboost_filesystem")

find_package(Boost 1.65.1 REQUIRED COMPONENTS filesystem)

add_executable(mapreduce mapreduce.cpp buffered_reader.cpp buffered_writer.cpp process_manager.cpp temp_files_manager.cpp threadpool.cpp utils.cpp)

add_executable(map map.cpp buffered_reader.cpp buffered_writer.cpp utils.cpp)

add_executable(reduce reduce.cpp buffered_reader.cpp buffered_writer.cpp utils.cpp)
