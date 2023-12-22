#!/bin/bash

#DEFINED_MACRO := -DUSE_INT_INSTEAD_OF_ENUM 
DEFINED_MACRO := 



INC_PATH := -Icore -Icore/util -Icore/token -Icore/parser -Icore/parser/blank -Icore/parser/comment -Icore/parser/operator -Icore/parser/sequence -Icore/parser/semicolon -Icore/variblePool   -Icore/functionPool   -Icore/functionPool/basic  -Icore/functionPool/explog      -Icore/functionPool/powSqrt    -Icore/functionPool/trigonometric    -Icore/functionPool/nearest    


#COMPILE_OPT :=  -Wall --std=c++11 $(DEFINED_MACRO) $(INC_PATH)   -c -g3
COMPILE_OPT :=  -Werror -Wall --std=c++11 $(DEFINED_MACRO) $(INC_PATH) -O3  -c

####################################################################################################
####################################################################################################
####################################################################################################

#LINK_OPT := -Wall --std=c++11
LINK_OPT := -Werror  -Wall --std=c++11  -O3



#
# build/stringNumber.o
#
build/arithMetic:build/arithMetic.o build/buff.o build/enumUtil.o build/keywordList.o  build/dataTypeUtil.o  build/stringNumber.o  build/opUtil.o    build/myException.o build/buff.o build/globalDirector.o   build/tokenBase.o build/tokenMgr.o   build/expEvaluation.o    build/cmdOptions.o   build/parserBase.o   build/blankParser.o build/singlelineCommentParser.o   build/multilineCommentParser.o  build/operatorParser.o  build/sequenceParser.o build/semicolonParser.o    build/variblePool.o build/dataValue.o   build/functionBase.o   build/functionMgr.o   build/MyAbs.o   build/MyFmod.o   build/MyRemainder.o  build/MyFmin.o    build/MyFmax.o   build/MyExp.o  build/MyExp2.o   build/MyLog.o  build/MyLog2.o   build/MyLog10.o    build/MyPow.o   build/MySqrt.o  build/MyCbrt.o  build/MyHypot.o  build/MySin.o  build/MyCos.o  build/MyTan.o   build/MyAsin.o   build/MyAcos.o  build/MyAtan.o   build/MyAtan2.o     build/MyCeil.o  build/MyFloor.o   build/MyTrunc.o  build/MyRound.o

	@echo -e "\n----------------------------------------------------------------------------------------------------"
	g++    $(LINK_OPT)    -o    build/arithMetic    build/*.o
	cp build/arithMetic .
	@echo "----------------------------------------------------------------------------------------------------"


build/arithMetic.o:arithMetic.cpp core/commonEnum.h core/myException.h core/buff.h core/globalDirector.h core/cmdOptions.h  core/token/tokenMgr.h   core/token/expEvaluation.h   core/functionPool/functionMgr.h    core/variblePool/variblePool.h  core/util/dataTypeUtil.h  
	g++    $(COMPILE_OPT)  -o  build/arithMetic.o   arithMetic.cpp



build/myException.o:core/myException.cpp core/myException.h core/buff.h core/commonEnum.h core/util/enumUtil.h  core/util/charUtil.h
	g++    $(COMPILE_OPT)  -o  build/myException.o   core/myException.cpp

build/buff.o:core/buff.cpp core/buff.h
	g++    $(COMPILE_OPT)  -o  build/buff.o   core/buff.cpp


build/dataValue.o:core/dataValue.cpp   core/dataValue.h   core/commonEnum.h   core/myException.h  core/util/dataTypeUtil.h  core/cmdOptions.h
	g++    $(COMPILE_OPT)  -o  build/dataValue.o   core/dataValue.cpp


build/variblePool.o:core/variblePool/variblePool.cpp  core/variblePool/variblePool.h  core/myException.h core/token/tokenBase.h core/dataValue.h  core/util/enumUtil.h  core/util/charUtil.h   core/functionPool/functionMgr.h  
	g++    $(COMPILE_OPT)  -o  build/variblePool.o   core/variblePool/variblePool.cpp

build/enumUtil.o:core/util/enumUtil.cpp core/util/enumUtil.h core/parser/parserBase.h  core/commonEnum.h  core/myException.h
	g++    $(COMPILE_OPT)  -o  build/enumUtil.o   core/util/enumUtil.cpp

build/keywordList.o:core/util/keywordList.cpp core/util/keywordList.h
	g++    $(COMPILE_OPT)  -o  build/keywordList.o   core/util/keywordList.cpp

build/dataTypeUtil.o:core/util/dataTypeUtil.cpp core/util/dataTypeUtil.h core/commonEnum.h core/myException.h
	g++    $(COMPILE_OPT)  -o  build/dataTypeUtil.o   core/util/dataTypeUtil.cpp

build/stringNumber.o:core/util/stringNumber.cpp core/util/stringNumber.h 
	g++    $(COMPILE_OPT)  -o  build/stringNumber.o   core/util/stringNumber.cpp

build/opUtil.o:core/util/opUtil.cpp core/util/opUtil.h  core/commonEnum.h  core/myException.h
	g++    $(COMPILE_OPT)  -o  build/opUtil.o   core/util/opUtil.cpp

build/globalDirector.o:core/globalDirector.cpp  core/globalDirector.h   core/parser/parserBase.h   core/parser/blank/blankParser.h  core/parser/comment/singlelineCommentParser.h core/parser/comment/multilineCommentParser.h core/parser/sequence/sequenceParser.h core/util/charUtil.h   core/parser/blank/blankParser.h core/parser/comment/singlelineCommentParser.h core/parser/comment/multilineCommentParser.h core/parser/operator/operatorParser.h core/parser/sequence/sequenceParser.h core/parser/semicolon/semicolonParser.h   core/token/tokenBase.h core/cmdOptions.h core/util/enumUtil.h core/myException.h core/token/tokenMgr.h core/variblePool/variblePool.h
	g++    $(COMPILE_OPT)  -o  build/globalDirector.o    core/globalDirector.cpp

####################################################################################################
####################################################################################################
####################################################################################################
build/tokenBase.o:core/token/tokenBase.cpp core/token/tokenBase.h core/commonEnum.h core/util/keywordList.h core/myException.h core/buff.h core/dataValue.h  core/util/charUtil.h  core/util/stringNumber.h  core/functionPool/functionBase.h 
	g++    $(COMPILE_OPT)  -o  build/tokenBase.o    core/token/tokenBase.cpp

build/tokenMgr.o:core/token/tokenMgr.cpp  core/token/tokenMgr.h core/token/tokenBase.h core/myException.h   core/variblePool/variblePool.h core/util/opUtil.h core/util/dataTypeUtil.h core/cmdOptions.h   core/util/charUtil.h   core/token/expEvaluation.h
	g++    $(COMPILE_OPT)  -o  build/tokenMgr.o   core/token/tokenMgr.cpp


build/expEvaluation.o:core/token/expEvaluation.cpp  core/token/expEvaluation.h   core/token/tokenBase.h  core/commonEnum.h core/variblePool/variblePool.h core/cmdOptions.h core/util/charUtil.h  core/util/opUtil.h  core/myException.h  core/functionPool/functionMgr.h   core/functionPool/functionBase.h   core/util/dataTypeUtil.h   core/util/enumUtil.h
	g++    $(COMPILE_OPT)  -o  build/expEvaluation.o   core/token/expEvaluation.cpp


####################################################################################################
####################################################################################################
####################################################################################################
build/parserBase.o:core/parser/parserBase.cpp   core/parser/parserBase.h  core/token/tokenBase.h
	g++    $(COMPILE_OPT)  -o  build/parserBase.o    core/parser/parserBase.cpp


build/blankParser.o:core/parser/blank/blankParser.cpp   core/parser/blank/blankParser.h    core/parser/parserBase.h  core/util/charUtil.h
	g++    $(COMPILE_OPT)  -o  build/blankParser.o    core/parser/blank/blankParser.cpp

build/singlelineCommentParser.o:core/parser/comment/singlelineCommentParser.cpp core/parser/comment/singlelineCommentParser.h core/parser/parserBase.h core/util/charUtil.h
	g++    $(COMPILE_OPT)  -o  build/singlelineCommentParser.o    core/parser/comment/singlelineCommentParser.cpp

build/multilineCommentParser.o:core/parser/comment/multilineCommentParser.cpp core/parser/comment/multilineCommentParser.h core/parser/parserBase.h core/util/charUtil.h
	g++    $(COMPILE_OPT)  -o  build/multilineCommentParser.o    core/parser/comment/multilineCommentParser.cpp


build/sequenceParser.o:core/parser/sequence/sequenceParser.cpp  core/parser/sequence/sequenceParser.h core/parser/parserBase.h core/util/charUtil.h core/util/keywordList.h core/util/stringNumber.h
	g++    $(COMPILE_OPT)  -o  build/sequenceParser.o    core/parser/sequence/sequenceParser.cpp

build/operatorParser.o:core/parser/operator/operatorParser.cpp  core/parser/operator/operatorParser.h core/parser/parserBase.h core/util/charUtil.h
	g++    $(COMPILE_OPT)  -o  build/operatorParser.o    core/parser/operator/operatorParser.cpp


build/semicolonParser.o:core/parser/semicolon/semicolonParser.cpp  core/parser/semicolon/semicolonParser.h core/parser/parserBase.h core/util/charUtil.h  core/commonEnum.h
	g++    $(COMPILE_OPT)  -o  build/semicolonParser.o    core/parser/semicolon/semicolonParser.cpp 


build/cmdOptions.o:core/cmdOptions.cpp  core/cmdOptions.h core/util/charUtil.h  core/dataValue.h
	g++    $(COMPILE_OPT)  -o  build/cmdOptions.o    core/cmdOptions.cpp


build/functionBase.o:core/functionPool/functionBase.cpp  core/functionPool/functionBase.h  core/token/tokenBase.h  core/commonEnum.h  core/myException.h
	g++    $(COMPILE_OPT)  -o  build/functionBase.o    core/functionPool/functionBase.cpp

build/functionMgr.o:core/functionPool/functionMgr.cpp  core/functionPool/functionMgr.h  core/functionPool/functionBase.h  core/functionPool/basic/MyAbs.h  core/functionPool/basic/MyFmod.h  core/functionPool/basic/MyRemainder.h  core/functionPool/basic/MyFmax.h  core/functionPool/basic/MyFmin.h  core/functionPool/explog/MyExp.h core/functionPool/explog/MyExp2.h core/functionPool/explog/MyLog.h   core/functionPool/explog/MyLog2.h  core/functionPool/explog/MyLog10.h  core/functionPool/powSqrt/MyPow.h  core/functionPool/powSqrt/MySqrt.h   core/functionPool/powSqrt/MyCbrt.h   core/functionPool/powSqrt/MyHypot.h   core/functionPool/trigonometric/MySin.h    core/functionPool/trigonometric/MyCos.h    core/functionPool/trigonometric/MyTan.h    core/functionPool/trigonometric/MyAsin.h     core/functionPool/trigonometric/MyAcos.h   core/functionPool/trigonometric/MyAtan.cpp   core/functionPool/trigonometric/MyAtan2.h     core/functionPool/nearest/MyCeil.h      core/functionPool/nearest/MyFloor.h     core/functionPool/nearest/MyTrunc.h     core/functionPool/nearest/MyRound.h   
	g++    $(COMPILE_OPT)  -o  build/functionMgr.o    core/functionPool/functionMgr.cpp




##################################################
#  abs / fmod / remainder / fmin / fmax  
##################################################
build/MyAbs.o:core/functionPool/basic/MyAbs.cpp   core/functionPool/basic/MyAbs.h   core/functionPool/functionBase.h    core/functionPool/functionMgr.h    core/token/expEvaluation.h 
	g++    $(COMPILE_OPT)  -o  build/MyAbs.o    core/functionPool/basic/MyAbs.cpp

build/MyFmod.o:core/functionPool/basic/MyFmod.cpp   core/functionPool/basic/MyFmod.h   core/functionPool/functionBase.h    core/functionPool/functionMgr.h    core/token/expEvaluation.h  
	g++    $(COMPILE_OPT)  -o  build/MyFmod.o    core/functionPool/basic/MyFmod.cpp

build/MyRemainder.o:core/functionPool/basic/MyRemainder.cpp   core/functionPool/basic/MyRemainder.h   core/functionPool/functionBase.h     core/functionPool/functionMgr.h    core/token/expEvaluation.h 
	g++    $(COMPILE_OPT)  -o  build/MyRemainder.o    core/functionPool/basic/MyRemainder.cpp

build/MyFmin.o:core/functionPool/basic/MyFmin.cpp   core/functionPool/basic/MyFmin.h   core/functionPool/functionBase.h     core/functionPool/functionMgr.h    core/token/expEvaluation.h 
	g++    $(COMPILE_OPT)  -o  build/MyFmin.o    core/functionPool/basic/MyFmin.cpp

build/MyFmax.o:core/functionPool/basic/MyFmax.cpp   core/functionPool/basic/MyFmax.h   core/functionPool/functionBase.h     core/functionPool/functionMgr.h    core/token/expEvaluation.h 
	g++    $(COMPILE_OPT)  -o  build/MyFmax.o    core/functionPool/basic/MyFmax.cpp





##################################################
#  exp / log 
##################################################
build/MyExp.o:core/functionPool/explog/MyExp.cpp   core/functionPool/explog/MyExp.h   core/functionPool/functionBase.h       core/functionPool/functionMgr.h    core/token/expEvaluation.h 
	g++    $(COMPILE_OPT)  -o  build/MyExp.o    core/functionPool/explog/MyExp.cpp

build/MyExp2.o:core/functionPool/explog/MyExp2.cpp   core/functionPool/explog/MyExp2.h   core/functionPool/functionBase.h     core/functionPool/functionMgr.h    core/token/expEvaluation.h 
	g++    $(COMPILE_OPT)  -o  build/MyExp2.o    core/functionPool/explog/MyExp2.cpp

build/MyLog.o:core/functionPool/explog/MyLog.cpp   core/functionPool/explog/MyLog.h   core/functionPool/functionBase.h        core/functionPool/functionMgr.h    core/token/expEvaluation.h 
	g++    $(COMPILE_OPT)  -o  build/MyLog.o    core/functionPool/explog/MyLog.cpp

build/MyLog2.o:core/functionPool/explog/MyLog2.cpp   core/functionPool/explog/MyLog2.h   core/functionPool/functionBase.h      core/functionPool/functionMgr.h    core/token/expEvaluation.h 
	g++    $(COMPILE_OPT)  -o  build/MyLog2.o    core/functionPool/explog/MyLog2.cpp

build/MyLog10.o:core/functionPool/explog/MyLog10.cpp   core/functionPool/explog/MyLog10.h   core/functionPool/functionBase.h    core/functionPool/functionMgr.h    core/token/expEvaluation.h 
	g++    $(COMPILE_OPT)  -o  build/MyLog10.o    core/functionPool/explog/MyLog10.cpp



##################################################
#  Pow / Sqrt / cbrt 
##################################################
build/MyPow.o:core/functionPool/powSqrt/MyPow.cpp   core/functionPool/powSqrt/MyPow.h   core/functionPool/functionBase.h        core/functionPool/functionMgr.h    core/token/expEvaluation.h 
	g++    $(COMPILE_OPT)  -o  build/MyPow.o    core/functionPool/powSqrt/MyPow.cpp

build/MySqrt.o:core/functionPool/powSqrt/MySqrt.cpp   core/functionPool/powSqrt/MySqrt.h   core/functionPool/functionBase.h      core/functionPool/functionMgr.h    core/token/expEvaluation.h 
	g++    $(COMPILE_OPT)  -o  build/MySqrt.o    core/functionPool/powSqrt/MySqrt.cpp

build/MyCbrt.o:core/functionPool/powSqrt/MyCbrt.cpp   core/functionPool/powSqrt/MyCbrt.h   core/functionPool/functionBase.h      core/functionPool/functionMgr.h    core/token/expEvaluation.h 
	g++    $(COMPILE_OPT)  -o  build/MyCbrt.o    core/functionPool/powSqrt/MyCbrt.cpp

build/MyHypot.o:core/functionPool/powSqrt/MyHypot.cpp   core/functionPool/powSqrt/MyHypot.h   core/functionPool/functionBase.h   core/functionPool/functionMgr.h    core/token/expEvaluation.h 
	g++    $(COMPILE_OPT)  -o  build/MyHypot.o    core/functionPool/powSqrt/MyHypot.cpp




##################################################
#  sin / cos / tan / asin / acos / atan / atan2 
##################################################
build/MySin.o:core/functionPool/trigonometric/MySin.cpp   core/functionPool/trigonometric/MySin.h   core/functionPool/functionBase.h    core/functionPool/functionMgr.h    core/token/expEvaluation.h   
	g++    $(COMPILE_OPT)  -o  build/MySin.o    core/functionPool/trigonometric/MySin.cpp

build/MyCos.o:core/functionPool/trigonometric/MyCos.cpp   core/functionPool/trigonometric/MyCos.h   core/functionPool/functionBase.h    core/functionPool/functionMgr.h    core/token/expEvaluation.h   
	g++    $(COMPILE_OPT)  -o  build/MyCos.o    core/functionPool/trigonometric/MyCos.cpp


build/MyTan.o:core/functionPool/trigonometric/MyTan.cpp   core/functionPool/trigonometric/MyTan.h   core/functionPool/functionBase.h    core/functionPool/functionMgr.h    core/token/expEvaluation.h   
	g++    $(COMPILE_OPT)  -o  build/MyTan.o    core/functionPool/trigonometric/MyTan.cpp
	

build/MyAsin.o:core/functionPool/trigonometric/MyAsin.cpp   core/functionPool/trigonometric/MyAsin.h   core/functionPool/functionBase.h    core/functionPool/functionMgr.h    core/token/expEvaluation.h   
	g++    $(COMPILE_OPT)  -o  build/MyAsin.o    core/functionPool/trigonometric/MyAsin.cpp


build/MyAcos.o:core/functionPool/trigonometric/MyAcos.cpp   core/functionPool/trigonometric/MyAcos.h   core/functionPool/functionBase.h    core/functionPool/functionMgr.h    core/token/expEvaluation.h   
	g++    $(COMPILE_OPT)  -o  build/MyAcos.o    core/functionPool/trigonometric/MyAcos.cpp


build/MyAtan.o:core/functionPool/trigonometric/MyAtan.cpp   core/functionPool/trigonometric/MyAtan.h   core/functionPool/functionBase.h    core/functionPool/functionMgr.h    core/token/expEvaluation.h   
	g++    $(COMPILE_OPT)  -o  build/MyAtan.o    core/functionPool/trigonometric/MyAtan.cpp

build/MyAtan2.o:core/functionPool/trigonometric/MyAtan2.cpp   core/functionPool/trigonometric/MyAtan2.h   core/functionPool/functionBase.h    core/functionPool/functionMgr.h    core/token/expEvaluation.h   
	g++    $(COMPILE_OPT)  -o  build/MyAtan2.o    core/functionPool/trigonometric/MyAtan2.cpp





##################################################
#  ceil / floor / trunc / round
##################################################
build/MyCeil.o:core/functionPool/nearest/MyCeil.cpp   core/functionPool/nearest/MyCeil.h   core/functionPool/functionBase.h    core/functionPool/functionMgr.h    core/token/expEvaluation.h   
	g++    $(COMPILE_OPT)  -o  build/MyCeil.o    core/functionPool/nearest/MyCeil.cpp

build/MyFloor.o:core/functionPool/nearest/MyFloor.cpp   core/functionPool/nearest/MyFloor.h   core/functionPool/functionBase.h    core/functionPool/functionMgr.h    core/token/expEvaluation.h      
	g++    $(COMPILE_OPT)  -o  build/MyFloor.o    core/functionPool/nearest/MyFloor.cpp

build/MyTrunc.o:core/functionPool/nearest/MyTrunc.cpp   core/functionPool/nearest/MyTrunc.h   core/functionPool/functionBase.h    core/functionPool/functionMgr.h    core/token/expEvaluation.h     
	g++    $(COMPILE_OPT)  -o  build/MyTrunc.o    core/functionPool/nearest/MyTrunc.cpp

build/MyRound.o:core/functionPool/nearest/MyRound.cpp   core/functionPool/nearest/MyRound.h   core/functionPool/functionBase.h    core/functionPool/functionMgr.h    core/token/expEvaluation.h   
	g++    $(COMPILE_OPT)  -o  build/MyRound.o    core/functionPool/nearest/MyRound.cpp



.PHONY:clean
clean:
	-rm   -Rf   arithMetic *.exe *.dSYM   build/*.o  build/arithMetic*