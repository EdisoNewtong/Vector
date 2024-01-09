cmake_minimum_required(VERSION 3.8.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(arith CXX)


#[=[----------------------------------------------------------------------------------------------------

$ cmake -G  "Visual Studio 16 2019"  ../  

$ cmake -S ../arithmetic  -DCMAKE_BUILD_TYPE=Release   -DCMAKE_CXX_COMPILER=g++
$ cmake --build . --verbose

----------------------------------------------------------------------------------------------------]=]


#[[----------------------------------------------------------------------------------------------------
     set this flag can provent cmake from generating a project named "ZERO_CHECK"
     This command must be executed  **before**   the command  add_executable(...)
----------------------------------------------------------------------------------------------------]]
# set(CMAKE_SUPPRESS_REGENERATION  true)

# set_property(GLOBAL PROPERTY USE_FOLDERS ON)


set(all_src_hds
    ##################################################################
    # Root
    ##################################################################
    arithMetic.cpp

    ##################################################################
    # Core 
    ##################################################################
	core/commonEnum.h

    core/buff.cpp
    core/buff.h
    core/cmdOptions.cpp
    core/cmdOptions.h
    core/dataValue.cpp
    core/dataValue.h
    core/globalDirector.cpp
    core/globalDirector.h
    core/myException.cpp
    core/myException.h

    ##################################################################
    # core/util
    ##################################################################
    core/util/charUtil.h
    core/util/dataTypeUtil.h
    core/util/dataTypeUtil.cpp
    core/util/enumUtil.h
    core/util/enumUtil.cpp
    core/util/keywordList.h
    core/util/keywordList.cpp
    core/util/opUtil.h
    core/util/opUtil.cpp
    core/util/stringNumber.h
    core/util/stringNumber.cpp

    ##################################################################
    # core/functionPool
    ##################################################################
    core/functionPool/functionBase.cpp core/functionPool/functionBase.h
    core/functionPool/functionMgr.cpp core/functionPool/functionMgr.h

    core/functionPool/basic/MyAbs.cpp    core/functionPool/basic/MyAbs.h
    core/functionPool/basic/MyFmod.cpp   core/functionPool/basic/MyFmod.h
    core/functionPool/basic/MyFmin.cpp   core/functionPool/basic/MyFmin.h
    core/functionPool/basic/MyFmax.cpp   core/functionPool/basic/MyFmax.h

    core/functionPool/explog/MyExp.cpp   core/functionPool/explog/MyExp.h
    core/functionPool/explog/MyExp2.cpp   core/functionPool/explog/MyExp2.h
    core/functionPool/explog/MyLog.cpp   core/functionPool/explog/MyLog.h
    core/functionPool/explog/MyLog2.cpp   core/functionPool/explog/MyLog2.h
    core/functionPool/explog/MyLog10.cpp   core/functionPool/explog/MyLog10.h

    core/functionPool/powSqrt/MyPow.cpp   core/functionPool/powSqrt/MyPow.h
    core/functionPool/powSqrt/MySqrt.cpp   core/functionPool/powSqrt/MySqrt.h
    core/functionPool/powSqrt/MyCbrt.cpp   core/functionPool/powSqrt/MyCbrt.h
    core/functionPool/powSqrt/MyHypot.cpp   core/functionPool/powSqrt/MyHypot.h

    core/functionPool/trigonometric/MySin.cpp   core/functionPool/trigonometric/MySin.h
    core/functionPool/trigonometric/MyCos.cpp   core/functionPool/trigonometric/MyCos.h
    core/functionPool/trigonometric/MyTan.cpp   core/functionPool/trigonometric/MyTan.h
    core/functionPool/trigonometric/MyAcos.cpp   core/functionPool/trigonometric/MyAcos.h
    core/functionPool/trigonometric/MyAsin.cpp   core/functionPool/trigonometric/MyAsin.h
    core/functionPool/trigonometric/MyAtan.cpp   core/functionPool/trigonometric/MyAtan.h
    core/functionPool/trigonometric/MyAtan2.cpp   core/functionPool/trigonometric/MyAtan2.h

    core/functionPool/nearest/MyCeil.cpp   core/functionPool/nearest/MyCeil.h
    core/functionPool/nearest/MyFloor.cpp   core/functionPool/nearest/MyFloor.h
    core/functionPool/nearest/MyTrunc.cpp   core/functionPool/nearest/MyTrunc.h
    core/functionPool/nearest/MyRound.cpp   core/functionPool/nearest/MyRound.h

    ##################################################################
    # core/parser
    ##################################################################
    core/parser/parserBase.cpp   core/parser/parserBase.h
    core/parser/blank/blankParser.cpp   core/parser/blank/blankParser.h
    core/parser/comment/singlelineCommentParser.cpp   core/parser/comment/singlelineCommentParser.h
    core/parser/comment/multilineCommentParser.cpp   core/parser/comment/multilineCommentParser.h
    core/parser/operator/operatorParser.cpp   core/parser/operator/operatorParser.h
    core/parser/semicolon/semicolonParser.cpp   core/parser/semicolon/semicolonParser.h
    core/parser/sequence/sequenceParser.cpp   core/parser/sequence/sequenceParser.h

    ##################################################################
    # core/token  ( token  &&  expression Evaluation )
    ##################################################################
    core/token/tokenBase.cpp       core/token/tokenBase.h
    core/token/tokenMgr.cpp        core/token/tokenMgr.h
    core/token/expEvaluation.cpp   core/token/expEvaluation.h

    ##################################################################
    # core/variblePool
    ##################################################################
    core/variblePool/variblePool.cpp   core/variblePool/variblePool.h
)


#
# source_group(...) can placed either before or after      `add_executable(...)`
# Both works well
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${all_src_hds})

add_executable(arith ${all_src_hds})

target_include_directories(arith PRIVATE 
    core 
    core/functionPool
    core/functionPool/basic
    core/functionPool/explog
    core/functionPool/powSqrt
    core/functionPool/trigonometric
    core/functionPool/nearest

    core/parser
    core/parser/blank
    core/parser/comment
    core/parser/operator
    core/parser/semicolon
    core/parser/sequence

    core/token
    core/util
    core/variblePool
)



#[====[####################################################################################################

######################################################################################################
#
# The following script is copied from stackoverflow.com
#   https://stackoverflow.com/questions/31422680/how-to-set-visual-studio-filters-for-nested-sub-directory-using-cmake
#
######################################################################################################




# This code sorts the project files as they appear in the root directory

# Generate a list of all .c & .h files in the current directory and sub directores.
file(
     GLOB_RECURSE source_list RELATIVE
     "${CMAKE_CURRENT_SOURCE_DIR}"
     *.c *.cpp *.cppm *.cxx    *.h *.hpp  *.hpp *.cxx
    )
foreach(source IN LISTS source_list)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()  
message(STATUS "Tree reorganized")

####################################################################################################]====]
