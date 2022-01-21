#!/bin/bash


all:binarywrite
	

binarywrite:binarywrite.cpp
	g++   -Wall  -std=c++11 -O3  -o   binarywrite   binarywrite.cpp


.PHONY:clean
clean:
	-rm   -Rf  *.o *.dSYM  binarywrite
