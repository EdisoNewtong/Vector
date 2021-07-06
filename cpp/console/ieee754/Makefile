#!/bin/bash

demo:demo.cpp floatNumberConverter.cpp floatNumberConverter.h
	g++   -Wall   -std=c++11   -c -o    demo.o    demo.cpp
	g++   -Wall   -std=c++11   -c -o    floatNumberConverter.o    floatNumberConverter.cpp
	g++   -Wall   -std=c++11      -o    demo    demo.o floatNumberConverter.o

.PHONY:clean
clean:
	-rm    -Rf    *.o   ieee754   demo *.dSYM
