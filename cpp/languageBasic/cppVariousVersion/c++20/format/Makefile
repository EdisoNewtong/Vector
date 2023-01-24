#!/bin/bash

COMPILE_OPT := -Wall -std=c++20

#DBG_OPT := -g3
DBG_OPT := 


LINK_OPT := -lfmt


main:main.o
	g++ $(COMPILE_OPT)   $(LINK_OPT)   -o main  main.o


main.o:main.cpp
	g++ $(COMPILE_OPT) -c $(DBG_OPT) -o main.o main.cpp 


.PHONY:clean
clean:
	-rm  -Rf   build/*  *.o*   *.exe  *.dSYM  main
