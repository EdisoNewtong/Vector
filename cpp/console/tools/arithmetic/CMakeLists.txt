cmake_minimum_required(VERSION 3.10)

project(arithmetic
        LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# store all *.cpp files relative path ( ${arithmetic_SOURCE_DIR} ) to  varible named 'all_src_list'
file(GLOB_RECURSE all_src_list
        RELATIVE "${arithmetic_SOURCE_DIR}"
        *.cpp)


# store all *.h files absolute path
file(GLOB_RECURSE all_header_list *.h)
# set(allHeadDir "")
        
# process all header files 
foreach(hd IN LISTS all_header_list)
    # 1. get the relative path of xxx.h     e.g.   /xxx/src/aaa/bbb/ccc/inc.h    ==>    calc__rel__path = "aaa/bbb/ccc/inc.h"
    file(RELATIVE_PATH calc__rel__path  "${arithmetic_SOURCE_DIR}" ${hd})
    # 2. get the directory path from   "aaa/bbb/ccc/inc.h"   =>    aaa/bbb/ccc
    get_filename_component(dir ${calc__rel__path} DIRECTORY)
    # 3. push it into list named 'allHeadDir'
    list(APPEND allHeadDir ${dir})
endforeach()



# delete duplicate  include path inside   list  
list(REMOVE_DUPLICATES allHeadDir)
# prepend the absolute path prefix to   all header s
list(TRANSFORM allHeadDir PREPEND "${PROJECT_SOURCE_DIR}/") 





# set the binary file name as   arith
add_executable(arith ${all_src_list})

# set header file search path
target_include_directories(arith PUBLIC
                          ${allHeadDir}
                          )

