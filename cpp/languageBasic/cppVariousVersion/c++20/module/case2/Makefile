#!/bin/bash

cpp_module_flag := -Wall  -std=c++20  -fmodules-ts  

main:person.o  main.o
	g++   $(cpp_module_flag)   -o  main   person.o  main.o



person.o:personModule.cpp
	g++   $(cpp_module_flag)  -c -x c++-system-header  string
	g++   $(cpp_module_flag)  -c  -o  person.o  personModule.cpp


main.o:main.cpp
	g++   $(cpp_module_flag)  -c -x c++-system-header  iostream
	g++   $(cpp_module_flag)  -c  -o  main.o  main.cpp


.PHONY:clean
clean:
	-rm   -Rf   gcm.cache   *.o   *.dSYM  *.exe  main



