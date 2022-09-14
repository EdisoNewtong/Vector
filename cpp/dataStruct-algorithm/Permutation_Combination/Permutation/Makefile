#!/bin/bash

all:demo fullPermutation

demo:demo.cpp 
	g++ -std=c++11 -Wall -o demo demo.cpp

fullPermutation:fullPermutation.cpp
	g++ -std=c++11 -Wall -o  fullPermutation   fullPermutation.cpp

.PHONY:clean
clean:
	-rm  -Rf *.o *.obj *.exe *.dSYM demo 
