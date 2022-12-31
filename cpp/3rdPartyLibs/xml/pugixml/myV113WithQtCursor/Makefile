#!/bin/bash

DBG_FLG := -g3
#DBG_FLG := 

CMP_FLG := -Wall -std=c++11




main:build/pugixml.o build/main.o
	g++  $(CMP_FLG)      $(DBG_FLG)  -o   main   build/pugixml.o build/main.o



build/main.o:main.cpp
	g++  $(CMP_FLG)  -c  $(DBG_FLG)  -o   build/main.o   main.cpp


build/pugixml.o:pugixml.cpp
	g++  $(CMP_FLG)  -c  $(DBG_FLG)  -o   build/pugixml.o   pugixml.cpp




.PHONY:clean
clean:
	-rm   -Rf   main  *.exe   build/*

