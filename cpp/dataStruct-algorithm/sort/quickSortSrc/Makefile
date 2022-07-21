#!/bin/bash

# -Wno-narrowing
# g++  -Wall -Wno-narrowing  -std=c++11   -o main      main.cpp



main:main.cpp sortUtil.h  oldSortingVersion.cpp     sortUtil_safe.cpp sortUtil_safe.h    oldSortingVersion.cpp
	g++   -Wall  -std=c++11   -c -o   sortUtil.o         sortUtil.cpp
	g++   -Wall  -std=c++11   -c -o   sortUtil_safe.o    sortUtil_safe.cpp
	g++   -Wall  -std=c++11   -c -o   oldSortingVersion.o    oldSortingVersion.cpp
	g++   -Wall  -std=c++11   -c -o   main.o    main.cpp
	g++   -Wall  -std=c++11      -o   main     main.o oldSortingVersion.o sortUtil.o  sortUtil_safe.o





.PHONY:clean
clean:
	-rm  -Rf  *.o*    *.exe   *.dSYM    main
