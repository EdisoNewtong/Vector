#!/bin/bash

# COMPILE_OPTIONS := -g3

popStackSeq:popStackSeq.cpp
	g++  -Wall  -std=c++11  $(COMPILE_OPTIONS) -o   popStackSeq   popStackSeq.cpp


.PHONY:clean
clean:
	-rm   -Rf   *.o*   *.dSYM  *.exe    popStackSeq
