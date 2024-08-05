#!/bin/bash

enable_debug_flag := -g3
# enable_debug_flag := 

main:treeDefine.h main.cpp  treeTestCase.cpp
	g++ -Wall -std=c++11 $(enable_debug_flag)  -c -o treeTestCase.o    treeTestCase.cpp
	g++ -Wall -std=c++11 $(enable_debug_flag)  -c -o   treeDefine.o    treeDefine.cpp
	g++ -Wall -std=c++11 $(enable_debug_flag)  -c -o         main.o    main.cpp
	g++ -Wall -std=c++11 $(enable_debug_flag)  -o main  treeTestCase.o  treeDefine.o  main.o



.PHONY:clean
clean:
	-rm   -Rf   *.o*   *.exe  *.dSYM   main


