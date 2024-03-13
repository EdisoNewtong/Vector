#!/bin/bash

cpp_module_flag := -Wall  -std=c++20  -fmodules-ts  


all:
	g++ $(cpp_module_flag) -c math-m.cpp
	g++ $(cpp_module_flag) -c math_helpers.cpp
	g++ $(cpp_module_flag) -c math.cpp
	g++ $(cpp_module_flag)  -c -x c++-system-header  iostream
	g++ $(cpp_module_flag) -c main.cpp
	g++ $(cpp_module_flag) -o main  *.o


.PHONY:clean
clean:
	-rm   -Rf   gcm.cache    *.o*   *.dSYM  *.exe   main


