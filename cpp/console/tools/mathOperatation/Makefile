#!/bin/bash

INC_PATH := -Icore -Icore/token -Icore/util -Icore/token/interger -Icore/token/floatnumber -Icore/token/varible -Icore/token/operator -Icore/token/comment -Icore/token/blank -Icore/token/expevaluation

COMPILE_OPT  :=  -Wall  -std=c++11
#
#COMPILE_OPT :=  -Wall  -std=c++11   -DUSE_INT_INSTEAD_OF_ENUM

mathCalc:build/parser.o build/parserOptions.o build/parsedCharInfo.o build/parserException.o  build/keywordList.o build/typeUtil.o build/enum2name.o build/charUtil.o build/stringNumber.o build/tokenInfo.o  build/decimalParser.o build/octalParser.o build/hexParser.o build/floatParser.o build/varibleParser.o  build/tokenParserBase.o build/blankParser.o build/singleLineCommentParser.o build/multiLineCommentParser.o build/operatorParser.o  build/expevaluation.o build/tokenParserMgr.o  build/tokenListAnalyzer.o  build/mathCalc.o
	g++   $(COMPILE_OPT) -o mathCalc build/*.o



build/mathCalc.o:mathCalc.cpp core/commonEnum.h core/parser.h core/parserOptions.h core/parserException.h core/util/charUtil.h core/util/typeUtil.h core/token/tokenParserMgr.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/mathCalc.o mathCalc.cpp


build/parserOptions.o:core/parserOptions.cpp  core/parserOptions.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/parserOptions.o core/parserOptions.cpp


build/parser.o:core/parser.cpp core/parser.h core/parserOptions.h core/commonEnum.h core/token/tokenParserBase.h  core/token/tokenParserMgr.h  core/token/tokenListAnalyzer.h core/util/enum2name.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/parser.o core/parser.cpp

build/parsedCharInfo.o:core/parsedCharInfo.cpp core/parsedCharInfo.h core/util/charUtil.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/parsedCharInfo.o core/parsedCharInfo.cpp 


build/parserException.o:core/parserException.cpp core/parserException.h core/commonEnum.h  core/parsedCharInfo.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/parserException.o  core/parserException.cpp

build/keywordList.o:core/util/keywordList.cpp  core/util/keywordList.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/keywordList.o  core/util/keywordList.cpp

build/charUtil.o:core/util/charUtil.cpp  core/util/charUtil.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/charUtil.o  core/util/charUtil.cpp

build/typeUtil.o:core/util/typeUtil.cpp  core/util/typeUtil.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/typeUtil.o  core/util/typeUtil.cpp


build/enum2name.o:core/util/enum2name.cpp  core/util/enum2name.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/enum2name.o  core/util/enum2name.cpp

build/stringNumber.o:core/util/stringNumber.cpp  core/util/stringNumber.h core/commonEnum.h   core/parserException.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/stringNumber.o  core/util/stringNumber.cpp 

#
# Base Part
#
build/tokenParserBase.o:core/token/tokenParserBase.cpp core/token/tokenParserBase.h core/commonEnum.h core/parserException.h core/parsedCharInfo.h  core/token/interger/decimalParser.h    core/token/floatnumber/floatParser.h 
	g++   $(COMPILE_OPT)   $(INC_PATH)  -c -o build/tokenParserBase.o  core/token/tokenParserBase.cpp

build/tokenInfo.o:core/token/tokenInfo.cpp core/token/tokenInfo.h core/commonEnum.h  core/parsedCharInfo.h
	g++   $(COMPILE_OPT)   $(INC_PATH)  -c -o build/tokenInfo.o  core/token/tokenInfo.cpp

#
# Number Part
#
build/decimalParser.o:core/token/interger/decimalParser.cpp core/token/interger/decimalParser.h core/token/tokenParserBase.h core/parserException.h core/parsedCharInfo.h
	g++   $(COMPILE_OPT)   $(INC_PATH)  -c -o build/decimalParser.o  core/token/interger/decimalParser.cpp

build/octalParser.o:core/token/interger/octalParser.cpp core/token/interger/octalParser.h core/token/tokenParserBase.h core/parserException.h core/parsedCharInfo.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/octalParser.o  core/token/interger/octalParser.cpp

build/hexParser.o:core/token/interger/hexParser.cpp core/token/interger/hexParser.h  core/token/tokenParserBase.h core/parserException.h core/parsedCharInfo.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/hexParser.o  core/token/interger/hexParser.cpp






#
# Float Part     
#
build/floatParser.o:core/token/floatnumber/floatParser.cpp core/token/floatnumber/floatParser.h core/token/tokenParserBase.h core/parserException.h core/parsedCharInfo.h
	g++   $(COMPILE_OPT)   $(INC_PATH)  -c -o build/floatParser.o  core/token/floatnumber/floatParser.cpp

#
# Varible Part
#
build/varibleParser.o:core/token/varible/varibleParser.cpp  core/token/varible/varibleParser.h  core/token/tokenParserBase.h core/parserException.h core/parsedCharInfo.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/varibleParser.o  core/token/varible/varibleParser.cpp


#
# Operator Part
#
build/operatorParser.o:core/token/operator/operatorParser.cpp   core/token/operator/operatorParser.h  core/token/tokenParserBase.h core/parserException.h core/parsedCharInfo.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/operatorParser.o  core/token/operator/operatorParser.cpp



#
# Comment Part     //     or       /*    ...  */
#
build/singleLineCommentParser.o:core/token/comment/singleLineCommentParser.cpp core/token/comment/singleLineCommentParser.h core/token/tokenParserBase.h core/parserException.h core/parsedCharInfo.h
	g++   $(COMPILE_OPT)   $(INC_PATH)  -c -o build/singleLineCommentParser.o  core/token/comment/singleLineCommentParser.cpp

build/multiLineCommentParser.o:core/token/comment/multiLineCommentParser.cpp core/token/comment/multiLineCommentParser.h core/token/tokenParserBase.h core/parserException.h core/parsedCharInfo.h
	g++   $(COMPILE_OPT)   $(INC_PATH)  -c -o build/multiLineCommentParser.o  core/token/comment/multiLineCommentParser.cpp



#
# Blank Part
#
build/blankParser.o:core/token/blank/blankParser.cpp core/token/blank/blankParser.h core/token/tokenParserBase.h core/parserException.h core/parsedCharInfo.h
	g++   $(COMPILE_OPT)   $(INC_PATH)  -c -o build/blankParser.o  core/token/blank/blankParser.cpp


build/expevaluation.o:core/token/expevaluation/expevaluation.cpp  core/token/expevaluation/expevaluation.h   core/token/tokenParserBase.h core/parserException.h core/parsedCharInfo.h
	g++   $(COMPILE_OPT)   $(INC_PATH)  -c -o build/expevaluation.o  core/token/expevaluation/expevaluation.cpp

#
# Parser-Mgr Part
#
build/tokenParserMgr.o:core/token/tokenParserMgr.cpp  core/token/tokenParserMgr.h core/token/tokenParserBase.h core/commonEnum.h core/token/blank/blankParser.h  core/token/comment/singleLineCommentParser.h  core/token/comment/multiLineCommentParser.h   core/token/interger/decimalParser.h  core/token/interger/octalParser.h core/token/interger/hexParser.h  core/token/floatnumber/floatParser.h  core/token/operator/operatorParser.h   core/token/varible/varibleParser.h core/token/expevaluation/expevaluation.h
	g++   $(COMPILE_OPT)   $(INC_PATH)  -c -o build/tokenParserMgr.o  core/token/tokenParserMgr.cpp



build/tokenListAnalyzer.o:core/token/tokenListAnalyzer.cpp  core/token/tokenListAnalyzer.h core/commonEnum.h core/token/tokenInfo.h core/util/charUtil.h  core/parsedCharInfo.h
	g++   $(COMPILE_OPT)   $(INC_PATH)  -c -o build/tokenListAnalyzer.o  core/token/tokenListAnalyzer.cpp








####################################################################################################
####################################################################################################
####################################################################################################
.PHONY:clean
clean:
	-rm   -Rf  *.o build/*.o  *.exe *.dSYM  mathCalc  
