#!/bin/bash

optimize_flag := -O3

all:ieee754 demo genTools unitTest

floatNumberConverter.o:core/floatNumberConverter.cpp core/floatNumberConverter.h
	g++   -Wall   -std=c++11 $(optimize_flag)   -Icore -c -o    floatNumberConverter.o    core/floatNumberConverter.cpp

ieee754:floatNumberConverter.o ieee754.cpp
	@echo "=============== Build <ieee754> project ==============="
	g++   -Wall   -std=c++11   $(optimize_flag) -Icore -c -o    ieee754.o    ieee754.cpp
	g++   -Wall   -std=c++11   $(optimize_flag)   -o    ieee754    ieee754.o floatNumberConverter.o


unitTest:floatNumberConverter.o  unitTest.cpp
	@echo "=============== Build <unitTest> project ==============="
	g++   -Wall   -std=c++11   $(optimize_flag) -Icore -c -o    unitTest.o    unitTest.cpp
	g++   -Wall   -std=c++11   $(optimize_flag)   -o    unitTest    unitTest.o floatNumberConverter.o



genTools:floatNumberConverter.o  genTools.cpp
	@echo "=============== Build <genTools> project ==============="
	g++   -Wall   -std=c++11   $(optimize_flag) -Icore   -c -o    genTools.o    genTools.cpp
	g++   -Wall   -std=c++11   $(optimize_flag)   -o    genTools    genTools.o floatNumberConverter.o

genCode:floatNumberConverter.o   genCode.cpp 
	@echo "=============== Build <genCode> project ==============="
	g++   -Wall   -std=c++11  $(optimize_flag) -Icore -c -o    genCode.o    genCode.cpp
	g++   -Wall   -std=c++11  $(optimize_flag)    -o    genCode      genCode.o floatNumberConverter.o


demo:floatNumberConverter.o   demo.cpp
	@echo "=============== Build <demo> project ==============="
	g++   -Wall   -std=c++11  $(optimize_flag) -Icore   -c -o    demo.o    demo.cpp
	g++   -Wall   -std=c++11  $(optimize_flag)    -o    demo    demo.o floatNumberConverter.o

.PHONY:clean
clean:
	-rm    -Rf    *.o*    *.exe   ieee754 unitTest   demo genCode  genTools *.dSYM
