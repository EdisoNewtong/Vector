#!/bin/bash

INC_PATH := -Icore -Icore/token -Icore/util -Icore/token/interger -Icore/token/floatnumber -Icore/token/varible -Icore/token/operator -Icore/token/comment -Icore/token/blank

COMPILE_OPT :=  -Wall  -std=c++11  


main:build/parser.o build/parsedCharInfo.o build/parserException.o  build/keywordList.o build/typeUtil.o build/charUtil.o build/tokenInfo.o  build/decimalParser.o build/octalParser.o build/hexParser.o build/floatParser.o build/varibleParser.o build/main.o  build/tokenParserBase.o build/blankParser.o build/singleLineCommentParser.o build/multiLineCommentParser.o build/operatorParser.o
	g++   $(COMPILE_OPT) -o main build/*.o



build/main.o:main.cpp core/commonEnum.h core/parser.h core/parserException.h core/util/charUtil.h core/util/typeUtil.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/main.o main.cpp

build/parser.o:core/parser.cpp core/parser.h core/commonEnum.h core/token/tokenParserBase.h core/token/interger/decimalParser.h core/token/interger/octalParser.h core/token/interger/hexParser.h core/token/floatnumber/floatParser.h core/token/varible/varibleParser.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/parser.o core/parser.cpp

build/parsedCharInfo.o:core/parsedCharInfo.cpp core/parsedCharInfo.h core/util/charUtil.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/parsedCharInfo.o core/parsedCharInfo.cpp 


build/parserException.o:core/parserException.cpp core/parserException.h core/commonEnum.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/parserException.o  core/parserException.cpp

build/keywordList.o:core/util/keywordList.cpp  core/util/keywordList.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/keywordList.o  core/util/keywordList.cpp

build/charUtil.o:core/util/charUtil.cpp  core/util/charUtil.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/charUtil.o  core/util/charUtil.cpp

build/typeUtil.o:core/util/typeUtil.cpp  core/util/typeUtil.h
	g++   $(COMPILE_OPT)   $(INC_PATH)   -c -o build/typeUtil.o  core/util/typeUtil.cpp


#
# Base Part
#
build/tokenParserBase.o:core/token/tokenParserBase.cpp core/token/tokenParserBase.h core/commonEnum.h core/parserException.h core/parsedCharInfo.h
	g++   $(COMPILE_OPT)   $(INC_PATH)  -c -o build/tokenParserBase.o  core/token/tokenParserBase.cpp

build/tokenInfo.o:core/token/tokenInfo.cpp core/token/tokenInfo.h core/commonEnum.h
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






.PHONY:clean
clean:
	-rm   -Rf  *.o build/*.o  *.exe *.dSYM  main  
