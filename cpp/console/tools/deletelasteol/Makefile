#!/bin/bash

all:deletelasteol

deletelasteol:deletelasteol.cpp
	g++  -Wall -std=c++11  -O3 -o  deletelasteol  deletelasteol.cpp


.PHONY:clean
clean:
	-rm  -Rf  *.o  deletelasteol *.dSYM
