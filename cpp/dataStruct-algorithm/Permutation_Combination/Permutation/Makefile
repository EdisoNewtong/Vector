#!/bin/bash

all:demo permutation

demo:demo.cpp 
	g++ -std=c++11 -Wall -o demo demo.cpp

permutation:permutation.cpp
	g++ -std=c++11 -Wall -o  permutation   permutation.cpp

.PHONY:clean
clean:
	-rm  -Rf *.o *.obj *.exe *.dSYM demo  permutation
