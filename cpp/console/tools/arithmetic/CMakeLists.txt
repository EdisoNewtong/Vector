cmake_minimum_required(VERSION 3.1.0)

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


# core/functionPool
source_group(arithMetic/core/functionPool               FILES ${funcpool_root})

source_group(arithMetic/core/functionPool/basic         FILES ${funcpool_basic})
source_group(arithMetic/core/functionPool/explog        FILES ${funcpool_explog})
source_group(arithMetic/core/functionPool/powSqrt       FILES ${funcpool_powsqrt})
source_group(arithMetic/core/functionPool/trigonometric FILES ${funcpool_trigonometric})
source_group(arithMetic/core/functionPool/nearest       FILES ${funcpool_nearest})

# core/parser
source_group(arithMetic/core/parser           FILES ${parser_root})
source_group(arithMetic/core/parser/blank     FILES ${parser_blank})
source_group(arithMetic/core/parser/comment   FILES ${parser_comment})
source_group(arithMetic/core/parser/operator  FILES ${parser_operator})
source_group(arithMetic/core/parser/semicolon FILES ${parser_semicolon})
source_group(arithMetic/core/parser/sequence  FILES ${parser_sequence})


# core/util
source_group(arithMetic/core/util  FILES ${util_fd})

# core/token
source_group(arithMetic/core/token  FILES ${token_fd})

# core/variblePool
source_group(arithMetic/core/variblePool  FILES ${variblepool_fd})

# core
source_group(arithMetic/core              FILES ${core_fd})

# root
source_group(arithMetic                  FILES  ${root_src})


add_executable(arith 
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


