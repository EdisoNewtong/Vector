#!/bin/bash


NO_POINTER_VERSION_FLAG := -DUSE_POINTER_VERSION_DATA_STRUCT


replaceChinesePunctuation:replaceChinesePunctuation.cpp
	g++   -Wall   -std=c++11  $(NO_POINTER_VERSION_FLAG)  -O3   -o   replaceChinesePunctuation    replaceChinesePunctuation.cpp


.PHONY:clean
clean:
	-rm    -Rf    *.o*  *.exe   replaceChinesePunctuation