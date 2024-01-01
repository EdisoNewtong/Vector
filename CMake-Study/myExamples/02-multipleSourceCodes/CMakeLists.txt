cmake_minimum_required(VERSION 3.1.0)

project(mySqrt)
set(CMAKE_CXX_STANDARD 11)                     # compile with c++11 standard

add_executable(mySqrt main.cpp)

add_subdirectory(util)

target_include_directories(mySqrt PUBLIC util)
