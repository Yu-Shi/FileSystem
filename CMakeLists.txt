cmake_minimum_required(VERSION 3.8)
project(FileSystem)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp structs.h operations.h)
add_executable(FileSystem ${SOURCE_FILES})