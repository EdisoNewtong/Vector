#!/bin/bash

all:eolconvert
	

eolconvert:eolconvert.cpp
	g++   -Wall  -std=c++11  -O3 -o   eolconvert    eolconvert.cpp



.PHONY:clean
clean:
	-rm   -Rf   *.o*   *.exe   *.dSYM   eolconvert
