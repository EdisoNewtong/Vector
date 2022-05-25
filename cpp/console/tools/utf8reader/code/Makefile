#!/bin/bash

MYFLAG :=  -std=c++11   -Wall  -O3
BINNAME := utf8reader

utf8reader:src/SuperBinaryDisplayUtil.cpp src/SuperBinaryDisplayUtil.h src/$(BINNAME).cpp
	g++  $(MYFLAG)   -o   build/SuperBinaryDisplayUtil.o  -c src/SuperBinaryDisplayUtil.cpp
	g++  $(MYFLAG)   -o   build/$(BINNAME).o  -c src/$(BINNAME).cpp
	g++  $(MYFLAG) build/*.o   -o   build/$(BINNAME)
	cp  build/$(BINNAME)  ./



.PHONY:clean
clean:
	-rm  -Rf  build/*.o*  build/*.exe build/$(BINNAME)  ./$(BINNAME) ./$(BINNAME).exe
