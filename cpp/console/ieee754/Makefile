#!/bin/bash


all:main

mytest:testCase

gen:genTestCase

testCase:testCase.o floatNumberConverter.o
	g++   -Wall   -std=c++11   -o    testCase    testCase.o floatNumberConverter.o


genTestCase:genTestCase.o
	g++   -Wall   -std=c++11   -o    genTestCase  genTestCase.o


genTestCase.o:genTestCase.cpp
	g++   -Wall   -std=c++11   -c -o    genTestCase.o  genTestCase.cpp


main:ieee754.o floatNumberConverter.o
	g++   -Wall   -std=c++11   -o    ieee754    ieee754.o floatNumberConverter.o


testCase.o:testCase.cpp floatNumberConverter.h
	g++   -Wall   -std=c++11   -c -o    testCase.o    testCase.cpp

ieee754.o:ieee754.cpp floatNumberConverter.h
	g++   -Wall   -std=c++11   -c -o    ieee754.o    ieee754.cpp

floatNumberConverter.o:floatNumberConverter.cpp floatNumberConverter.h
	g++   -Wall   -std=c++11   -c -o    floatNumberConverter.o    floatNumberConverter.cpp



.PHONY:clean
clean:
	-rm    -Rf    *.o   ieee754  main  testCase  genTestCase  *.dSYM
