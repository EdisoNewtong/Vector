#!/bin/bash

buildflag := -O3

intdisplay:stringNumber.o myException.o charutil.o intdisplay.o
	g++   -Wall   --std=c++11  $(buildflag)       -o  intdisplay   *.o


stringNumber.o:stringNumber.cpp stringNumber.h charutil.h
	g++   -Wall   --std=c++11  $(buildflag)    -c -o  stringNumber.o   stringNumber.cpp

myException.o:myException.cpp myException.h
	g++   -Wall   --std=c++11  $(buildflag)    -c -o  myException.o   myException.cpp

charutil.o:charutil.cpp charutil.h
	g++   -Wall   --std=c++11  $(buildflag)    -c -o  charutil.o   charutil.cpp


intdisplay.o:intdisplay.cpp charutil.h  myException.h stringNumber.h
	g++   -Wall   --std=c++11  $(buildflag)    -c -o  intdisplay.o   intdisplay.cpp




.PHONY:clean
clean:
	-rm   -Rf   *.o*   *.exe   *.dSYM   intdisplay
