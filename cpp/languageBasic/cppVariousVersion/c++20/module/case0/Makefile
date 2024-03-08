#!/bin/bash

cpp_module_flag := -Wall  -std=c++20  -fmodules-ts  

main:myFirstModule.o  main.o
	g++   $(cpp_module_flag)   -o  main myFirstModule.o  main.o



myFirstModule.o:myFirstModule.cpp
	g++   $(cpp_module_flag)  -c  -o  myFirstModule.o  myFirstModule.cpp


main.o:main.cpp
	g++   $(cpp_module_flag)  -xc++-system-header  iostream
	g++   $(cpp_module_flag)  -c  -o  main.o  main.cpp


.PHONY:clean
clean:
	-rm   -Rf   gcm.cache   *.o   *.dSYM  *.exe  main


