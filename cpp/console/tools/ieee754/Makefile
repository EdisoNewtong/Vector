#!/bin/bash


all:ieee754 demo genTools unitTest

floatNumberConverter.o:core/floatNumberConverter.cpp core/floatNumberConverter.h
	g++   -Wall   -std=c++11   -Icore -c -o    floatNumberConverter.o    core/floatNumberConverter.cpp

ieee754:floatNumberConverter.o ieee754.cpp
	@echo "=============== Build <unitTest> project ==============="
	g++   -Wall   -std=c++11   -Icore -c -o    ieee754.o    ieee754.cpp
	g++   -Wall   -std=c++11      -o    ieee754    ieee754.o floatNumberConverter.o


unitTest:floatNumberConverter.o  unitTest.cpp
	@echo "=============== Build <unitTest> project ==============="
	g++   -Wall   -std=c++11   -Icore -c -o    unitTest.o    unitTest.cpp
	g++   -Wall   -std=c++11      -o    unitTest    unitTest.o floatNumberConverter.o



genTools:floatNumberConverter.o  genTools.cpp
	@echo "=============== Build <genTools> project ==============="
	g++   -Wall   -std=c++11   -Icore   -c -o    genTools.o    genTools.cpp
	g++   -Wall   -std=c++11      -o    genTools    genTools.o floatNumberConverter.o

genCode:floatNumberConverter.o   genCode.cpp 
	@echo "=============== Build <genCode> project ==============="
	g++   -Wall   -std=c++11   -Icore -c -o    genCode.o    genCode.cpp
	g++   -Wall   -std=c++11      -o    genCode      genCode.o floatNumberConverter.o


demo:floatNumberConverter.o   demo.cpp
	@echo "=============== Build <Demo> project ==============="
	g++   -Wall   -std=c++11   -Icore   -c -o    demo.o    demo.cpp
	g++   -Wall   -std=c++11      -o    demo    demo.o floatNumberConverter.o

.PHONY:clean
clean:
	-rm    -Rf    *.o*    *.exe   ieee754 unitTest   demo genCode  genTools *.dSYM
