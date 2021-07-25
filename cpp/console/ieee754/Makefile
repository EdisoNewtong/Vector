#!/bin/bash


all:ieee754 demo genTools

ieee754:ieee754.cpp core/floatNumberConverter.cpp core/floatNumberConverter.h
	@echo "=============== Build <ieee754> project ==============="
	g++   -Wall   -std=c++11   -Icore -c -o    floatNumberConverter.o    core/floatNumberConverter.cpp
	g++   -Wall   -std=c++11   -Icore -c -o    ieee754.o    ieee754.cpp
	g++   -Wall   -std=c++11      -o    ieee754    ieee754.o floatNumberConverter.o



genTools:genTools.cpp core/floatNumberConverter.cpp core/floatNumberConverter.h
	@echo "=============== Build <genTools> project ==============="
	g++   -Wall   -std=c++11   -Icore   -c -o    floatNumberConverter.o    core/floatNumberConverter.cpp
	g++   -Wall   -std=c++11   -Icore   -c -o    genTools.o    genTools.cpp
	g++   -Wall   -std=c++11      -o    genTools    genTools.o floatNumberConverter.o

genCode:genCode.cpp core/floatNumberConverter.cpp core/floatNumberConverter.h
	@echo "=============== Build <genCode> project ==============="
	g++   -Wall   -std=c++11   -Icore -c -o    floatNumberConverter.o    core/floatNumberConverter.cpp
	g++   -Wall   -std=c++11   -Icore -c -o    genCode.o    genCode.cpp
	g++   -Wall   -std=c++11      -o    genCode      genCode.o floatNumberConverter.o


demo:demo.cpp core/floatNumberConverter.cpp core/floatNumberConverter.h
	@echo "=============== Build <Demo> project ==============="
	g++   -Wall   -std=c++11   -Icore   -c -o    floatNumberConverter.o    core/floatNumberConverter.cpp
	g++   -Wall   -std=c++11   -Icore   -c -o    demo.o    demo.cpp
	g++   -Wall   -std=c++11      -o    demo    demo.o floatNumberConverter.o

.PHONY:clean
clean:
	-rm    -Rf    *.o   ieee754   demo genCode  genTools *.dSYM
