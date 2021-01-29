#!/bin/bash

all:main

# -Wl,-rpath,/Users/Edison/work/me/src/github/excel/xlnt/build/source ../source/libxlnt.1.5.0.dylib
main:main.cpp
	g++  -Wall -std=c++14  -I../include -o main main.cpp -Wl,-rpath,../build/source ../build/source/libxlnt.1.5.0.dylib

.PHONY:clean
clean:
	-rm -Rf  *.o main
