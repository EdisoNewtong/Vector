cmake_minimum_required(VERSION 3.1.0)

project(myMain)
set(CMAKE_CXX_STANDARD 11)                     # compile with c++11 standard

add_executable(myMain main.cpp)

target_compile_definitions(myMain PUBLIC HAPPY_NEW_YEAR)
# target_compile_definitions(myMain PUBLIC <other-definition-1>)
# target_compile_definitions(myMain PUBLIC <other-definition-2>)

target_include_directories(myMain PUBLIC util)
# target_include_directories(myMain PUBLIC <other-include-dir-1>)
# target_include_directories(myMain PUBLIC <other-include-dir-2>)
