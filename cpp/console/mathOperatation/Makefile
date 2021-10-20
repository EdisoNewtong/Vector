#!/bin/bash

main:build/parser.o build/parserFlag.o build/parserExpection.o build/decimalParser.o build/octalParser.o build/octalParser.o build/floatParser.o build/varibleParser.o build/main.o 
	g++ -Wall -std=c++11 -Icore -o main build/*.o



build/main.o:main.cpp core/commonEnum.h core/parser.h
	g++ -Wall -std=c++11 -Icore -Icore/token -Icore/token/interger -Icore/token/floatnumber -Icore/token/varible   -c -o build/main.o main.cpp

build/parser.o:core/parser.cpp core/parser.h core/commonEnum.h
	g++ -Wall -std=c++11 -Icore -Icore/token -Icore/token/interger -Icore/token/floatnumber -Icore/token/varible   -c -o build/parser.o core/parser.cpp

build/parserFlag.o:core/parserFlag.cpp
	g++ -Wall -std=c++11 -Icore -Icore/token -Icore/token/interger -Icore/token/floatnumber -Icore/token/varible   -c -o build/parserFlag.o core/parserFlag.cpp 

build/parserExpection.o:core/parserExpection.cpp
	g++ -Wall -std=c++11 -Icore -Icore/token -Icore/token/interger -Icore/token/floatnumber -Icore/token/varible   -c -o build/parserExpection.o  core/parserExpection.cpp




build/decimalParser.o:core/token/interger/decimalParser.cpp core/token/interger/decimalParser.h core/commonEnum.h
	g++ -Wall -std=c++11 -Icore -Icore/token -Icore/token/interger -Icore/token/floatnumber -Icore/token/varible   -c -o build/decimalParser.o  core/token/interger/decimalParser.cpp

build/octalParser.o:core/token/interger/octalParser.cpp core/token/interger/octalParser.h core/commonEnum.h
	g++ -Wall -std=c++11 -Icore -Icore/token -Icore/token/interger -Icore/token/floatnumber -Icore/token/varible   -c -o build/octalParser.o  core/token/interger/octalParser.cpp

build/hexParser.o:core/token/interger/hexParser.cpp core/token/interger/hexParser.h  core/commonEnum.h
	g++ -Wall -std=c++11 -Icore -Icore/token -Icore/token/interger -Icore/token/floatnumber -Icore/token/varible   -c -o build/hexParser.o  core/token/interger/hexParser.cpp

build/floatParser.o:core/token/floatnumber/floatParser.cpp core/token/floatnumber/floatParser.h core/commonEnum.h
	g++ -Wall -std=c++11 -Icore -Icore/token -Icore/token/interger -Icore/token/floatnumber -Icore/token/varible   -c -o build/floatParser.o  core/token/floatnumber/floatParser.cpp

build/varibleParser.o:core/token/varible/varibleParser.cpp  core/token/varible/varibleParser.h  core/commonEnum.h
	g++ -Wall -std=c++11 -Icore -Icore/token -Icore/token/interger -Icore/token/floatnumber -Icore/token/varible   -c -o build/varibleParser.o  core/token/varible/varibleParser.cpp



.PHONY:clean
clean:
	-rm   -Rf  *.o build/*.o  *.exe *.dSYM  main  
