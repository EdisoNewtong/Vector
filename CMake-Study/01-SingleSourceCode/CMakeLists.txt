cmake_minimum_required(VERSION 3.10)

#
# After Build , The compiler && linker will generate a binary program named 'MyFirstCMake_Prj'
#
project(MyFirstCMake_Prj LANGUAGES CXX)

# the project MyFirstCMake_Prj contain only one source code file : main.cpp
add_executable(MyFirstCMake_Prj main.cpp)


