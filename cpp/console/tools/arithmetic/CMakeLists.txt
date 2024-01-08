cmake_minimum_required(VERSION 3.8.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(arith CXX)


#[=[

$ cmake -G  "Visual Studio 16 2019"    ../ 

]=]


#[[

     set this flag can provent cmake from generating a project named "ZERO_CHECK"
     This command must be executed  **before**   the command  add_executable(...)

]]
# set(CMAKE_SUPPRESS_REGENERATION  true)

# set_property(GLOBAL PROPERTY USE_FOLDERS ON)


set(root_src arithMetic.cpp)

set(util_fd
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
)

set(funcpool_root
    core/functionPool/functionBase.cpp core/functionPool/functionBase.h
    core/functionPool/functionMgr.cpp core/functionPool/functionMgr.h
)


set(funcpool_basic
    core/functionPool/basic/MyAbs.cpp    core/functionPool/basic/MyAbs.h
    core/functionPool/basic/MyFmod.cpp   core/functionPool/basic/MyFmod.h
    core/functionPool/basic/MyFmin.cpp   core/functionPool/basic/MyFmin.h
    core/functionPool/basic/MyFmax.cpp   core/functionPool/basic/MyFmax.h
)

set(funcpool_explog
    core/functionPool/explog/MyExp.cpp   core/functionPool/explog/MyExp.h
    core/functionPool/explog/MyExp2.cpp   core/functionPool/explog/MyExp2.h
    core/functionPool/explog/MyLog.cpp   core/functionPool/explog/MyLog.h
    core/functionPool/explog/MyLog2.cpp   core/functionPool/explog/MyLog2.h
    core/functionPool/explog/MyLog10.cpp   core/functionPool/explog/MyLog10.h
)

set(funcpool_powsqrt
    core/functionPool/powSqrt/MyPow.cpp   core/functionPool/powSqrt/MyPow.h
    core/functionPool/powSqrt/MySqrt.cpp   core/functionPool/powSqrt/MySqrt.h
    core/functionPool/powSqrt/MyCbrt.cpp   core/functionPool/powSqrt/MyCbrt.h
    core/functionPool/powSqrt/MyHypot.cpp   core/functionPool/powSqrt/MyHypot.h
)


set(funcpool_trigonometric
    core/functionPool/trigonometric/MySin.cpp   core/functionPool/trigonometric/MySin.h
    core/functionPool/trigonometric/MyCos.cpp   core/functionPool/trigonometric/MyCos.h
    core/functionPool/trigonometric/MyTan.cpp   core/functionPool/trigonometric/MyTan.h
    core/functionPool/trigonometric/MyAcos.cpp   core/functionPool/trigonometric/MyAcos.h
    core/functionPool/trigonometric/MyAsin.cpp   core/functionPool/trigonometric/MyAsin.h
    core/functionPool/trigonometric/MyAtan.cpp   core/functionPool/trigonometric/MyAtan.h
    core/functionPool/trigonometric/MyAtan2.cpp   core/functionPool/trigonometric/MyAtan2.h
)


set(funcpool_nearest
    core/functionPool/nearest/MyCeil.cpp   core/functionPool/nearest/MyCeil.h
    core/functionPool/nearest/MyFloor.cpp   core/functionPool/nearest/MyFloor.h
    core/functionPool/nearest/MyTrunc.cpp   core/functionPool/nearest/MyTrunc.h
    core/functionPool/nearest/MyRound.cpp   core/functionPool/nearest/MyRound.h
)

set(core_fd
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
)

set(parser_root
    core/parser/parserBase.cpp   core/parser/parserBase.h
)

set(parser_blank
    core/parser/blank/blankParser.cpp   core/parser/blank/blankParser.h
)

set(parser_comment
    core/parser/comment/singlelineCommentParser.cpp   core/parser/comment/singlelineCommentParser.h
    core/parser/comment/multilineCommentParser.cpp   core/parser/comment/multilineCommentParser.h
)


set(parser_operator
    core/parser/operator/operatorParser.cpp   core/parser/operator/operatorParser.h
)

set(parser_semicolon
    core/parser/semicolon/semicolonParser.cpp   core/parser/semicolon/semicolonParser.h
)


set(parser_sequence
    core/parser/sequence/sequenceParser.cpp   core/parser/sequence/sequenceParser.h
)

set(token_fd 
    core/token/tokenBase.cpp       core/token/tokenBase.h
    core/token/tokenMgr.cpp        core/token/tokenMgr.h
    core/token/expEvaluation.cpp   core/token/expEvaluation.h
)

set(variblepool_fd
    core/variblePool/variblePool.cpp   core/variblePool/variblePool.h
)


set(all_src_hds 
    ${root_src}

    # functionPool
    ${funcpool_root}
    ${funcpool_basic}
    ${funcpool_explog}
    ${funcpool_powsqrt}
    ${funcpool_trigonometric}
    ${funcpool_nearest}
    # parser
    ${parser_root}
    ${parser_blank}
    ${parser_comment}
    ${parser_operator}
    ${parser_semicolon}
    ${parser_sequence}

    # token
    ${token_fd}

    # util
    ${util_fd}

    # variblePool
    ${variblepool_fd}

    # root
    ${core_fd}
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


#[====[


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


]====]
