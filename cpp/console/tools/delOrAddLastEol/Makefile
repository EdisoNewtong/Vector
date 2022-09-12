#!/bin/bash

all:delOrAddLastEol

delOrAddLastEol:delOrAddLastEol.cpp
	g++  -Wall -std=c++11  -O3 -o  delOrAddLastEol   delOrAddLastEol.cpp


.PHONY:clean
clean:
	-rm  -Rf  *.o*  *.exe  *.dSYM  delOrAddLastEol
