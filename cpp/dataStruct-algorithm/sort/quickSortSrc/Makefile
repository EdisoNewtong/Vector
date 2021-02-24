#!/bin/bash

# -Wno-narrowing
# g++  -Wall -Wno-narrowing  -std=c++11   -o main      main.cpp


all:main

main:main.cpp 
	g++   -Wall  -std=c++11   -o   main    main.cpp

.PHONY:clean
clean:
	-rm  -Rf  main   *.o   main.dSYM
