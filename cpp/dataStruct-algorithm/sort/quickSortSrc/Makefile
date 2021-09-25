#!/bin/bash

# -Wno-narrowing
# g++  -Wall -Wno-narrowing  -std=c++11   -o main      main.cpp



main:main.cpp 
	g++   -Wall  -std=c++11   -c -o   main.o    main.cpp
	g++   -Wall  -std=c++11   -c -o   oldSortingVersion.o    oldSortingVersion.cpp
	g++   -Wall  -std=c++11   -o main  main.o oldSortingVersion.o




.PHONY:clean
clean:
	-rm  -Rf  main *.o *.exe main.dSYM
