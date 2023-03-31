#!/bin/bash

#DEFINED_MACRO := -DUSE_INT_INSTEAD_OF_ENUM 
DEFINED_MACRO := 



INC_PATH := -Icore -Icore/util -Icore/token -Icore/parser -Icore/parser/blank -Icore/parser/comment -Icore/parser/operator -Icore/parser/sequence -Icore/parser/semicolon -Icore/variblePool


#COMPILE_OPT :=  -Wall --std=c++11 $(DEFINED_MACRO) $(INC_PATH)   -c -g3
COMPILE_OPT :=  -Wall --std=c++11 $(DEFINED_MACRO) $(INC_PATH) -O3  -c

####################################################################################################
####################################################################################################
####################################################################################################

#LINK_OPT := -Wall --std=c++11
LINK_OPT := -Wall --std=c++11  -O3



#
# build/stringNumber.o
#
build/arithMetic:build/arithMetic.o build/buff.o build/enumUtil.o build/keywordList.o  build/dataTypeUtil.o  build/stringNumber.o  build/opUtil.o    build/myException.o build/buff.o build/globalDirector.o   build/tokenBase.o build/tokenMgr.o    build/cmdOptions.o   build/parserBase.o   build/blankParser.o build/singlelineCommentParser.o   build/multilineCommentParser.o  build/operatorParser.o  build/sequenceParser.o build/semicolonParser.o    build/variblePool.o build/dataValue.o
	@echo -e "\n----------------------------------------------------------------------------------------------------"
	g++    $(LINK_OPT)    -o    build/arithMetic    build/*.o
	cp build/arithMetic .
	@echo "----------------------------------------------------------------------------------------------------"


build/arithMetic.o:arithMetic.cpp core/commonEnum.h core/myException.h core/buff.h core/globalDirector.h core/cmdOptions.h  core/token/tokenMgr.h  core/variblePool/variblePool.h  core/util/dataTypeUtil.h  
	g++    $(COMPILE_OPT)  -o  build/arithMetic.o   arithMetic.cpp



build/myException.o:core/myException.cpp core/myException.h core/buff.h core/commonEnum.h core/util/enumUtil.h  core/util/charUtil.h
	g++    $(COMPILE_OPT)  -o  build/myException.o   core/myException.cpp

build/buff.o:core/buff.cpp core/buff.h
	g++    $(COMPILE_OPT)  -o  build/buff.o   core/buff.cpp


build/dataValue.o:core/dataValue.cpp   core/dataValue.h   core/commonEnum.h   core/myException.h  core/util/dataTypeUtil.h  core/cmdOptions.h
	g++    $(COMPILE_OPT)  -o  build/dataValue.o   core/dataValue.cpp


build/variblePool.o:core/variblePool/variblePool.cpp  core/variblePool/variblePool.h  core/myException.h core/token/tokenBase.h core/dataValue.h  core/util/enumUtil.h
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
build/tokenBase.o:core/token/tokenBase.cpp core/token/tokenBase.h core/commonEnum.h core/util/keywordList.h core/myException.h core/buff.h core/dataValue.h  core/util/charUtil.h  core/util/stringNumber.h
	g++    $(COMPILE_OPT)  -o  build/tokenBase.o    core/token/tokenBase.cpp

build/tokenMgr.o:core/token/tokenMgr.cpp  core/token/tokenMgr.h core/token/tokenBase.h core/myException.h   core/variblePool/variblePool.h core/util/opUtil.h core/util/dataTypeUtil.h core/cmdOptions.h   core/util/charUtil.h
	g++    $(COMPILE_OPT)  -o  build/tokenMgr.o   core/token/tokenMgr.cpp



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










.PHONY:clean
clean:
	-rm   -Rf   arithMetic *.exe *.dSYM   build/*.o  build/arithMetic*

