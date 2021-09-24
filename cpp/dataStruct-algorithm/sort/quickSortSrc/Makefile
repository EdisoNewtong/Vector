#!/bin/bash

# -Wno-narrowing
# g++  -Wall -Wno-narrowing  -std=c++11   -o main      main.cpp


all:main

main:main.cpp 
	g++   -Wall  -std=c++11   -o   main    main.cpp

v2:V2_simpleCode.cpp
	g++   -Wall  -std=c++11   -o   v2    V2_simpleCode.cpp

.PHONY:clean
clean:
	-rm  -Rf  main v2  *.o *.exe main.dSYM v2.dSYM
