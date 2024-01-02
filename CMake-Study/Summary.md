# About Target 
Probably the most important item is targets. 
Targets represent executables, libraries, and utilities built by CMake. 
Every `add_library(...)`, `add_executable(...)`, and `add_custom_target(...)` command creates a target. 



# Specify the build compiler
You can specify the compilers directly on the command line by using -DCMAKE_CXX_COMPILER=cl
```bash
# Use command 'cl' as the default compiler
$ cmake <some_source_path>  -DCMAKE_CXX_COMPILER=cl
```


# Specify the build mode Debug / Release
- Debug
- Release
- RelWithDebInfo
- MinSizeRel


```bash
# With source code in the directory MyProject
# to build MyProject-debug create that directory, cd into it and
$ ccmake ../MyProject -DCMAKE_BUILD_TYPE=Debug
# the same idea is used for the release tree MyProject-release
$ ccmake ../MyProject -DCMAKE_BUILD_TYPE=Release
```

