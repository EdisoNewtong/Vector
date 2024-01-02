# How to add a marco definination as a compile flag for a target
```cmake
# examgle : 01-SingleSourceCode
project(myMain)

add_executable(myMain main.cpp)

# Notes : the call of function target_compile_definitions(...) must after add_executable(...)
# Because project(...) is just a command to name a project rather than a target
# It's the function  add_executable(...) that created a kind of executeable target
target_compile_definitions(myMain PUBLIC HAPPY_NEW_YEAR)
# target_compile_definitions(myMain PUBLIC <other-definition-1>)
# target_compile_definitions(myMain PUBLIC <other-definition-2>)

```



# How to add include search path for a target
```cmake
# examgle : 01-SingleSourceCode
project(myMain)

add_executable(myMain main.cpp)

# Notes : the call of function target_include_directories(...) must after add_executable(...)
# Because project(...) is just a command to name a project rather than a target
# It's just the function add_executable(...) that created a kind of executeable target
target_include_directories(myMain PUBLIC util)
# target_include_directories(myMain PUBLIC <other-include-dir-1>)
# target_include_directories(myMain PUBLIC <other-include-dir-2>)

```



# How to manage multiple source code in different directories

The folder struct is like the following : 

02-multipleSourceCodes/
├── CMakeLists.txt
├── main.cpp
└── util             // util is a directory
    ├── AAA.cpp
    ├── AAA.h
    └── CMakeLists.txt



```cmake
#  02-multipleSourceCodes/CMakeLists.txt
cmake_minimum_required(VERSION 3.1.0)

project(mySqrt)
add_executable(mySqrt main.cpp)
add_subdirectory(util)

target_include_directories(mySqrt PUBLIC util)
```



```cmake
#  02-multipleSourceCodes/util/CMakeLists.txt
#[=[

CMake Error at util/CMakeLists.txt:4 (target_sources):
  Cannot find source file:

    AAA.cpp
]=]


# [ERROR] :  the following command will raise an error previously mentioned
# target_sources(mySqrt   PUBLIC   AAA.cpp)

  target_sources(mySqrt   PUBLIC   "${CMAKE_CURRENT_SOURCE_DIR}/AAA.cpp")

```

# How to collect the source code files by cmake command ( use api aux_source_directory(...) )
$ tree -C -L 1 core
core/
├── buff.cpp
├── buff.h
├── cmdOptions.cpp
├── cmdOptions.h
├── commonEnum.h
├── dataValue.cpp
├── dataValue.h
├── functionPool
├── globalDirector.cpp
├── globalDirector.h
├── myException.cpp
├── myException.h
├── parser              // directory
├── token               // directory
├── util                // directory
└── variblePool         // directory


```cmake
#[==[

Output : 

-- ##################################################
-- core/buff.cpp
-- core/cmdOptions.cpp
-- core/dataValue.cpp
-- core/globalDirector.cpp
-- core/myException.cpp
-- ##################################################




Notes : aux_source_directory(...) traversal the directory with  ***None*** recursive

]==]

#
# create a new varible named "all_src" to collect the source code files into a list
#
aux_source_directory(core all_src)

message(STATUS "##################################################")
foreach(ele ${all_src})
    message(STATUS ${ele})
endforeach()

message(STATUS "##################################################")

```




