#!/bin/bash

# -Wno-narrowing
# g++  -Wall -Wno-narrowing  -std=c++11   -o main      main.cpp



# DBG_FLAG := -g3
DBG_FLAG :=








main:main.cpp  sortUtil1.h  sortUtil1.cpp  sortUtil2.h  sortUtil2.cpp   sortUtil3.h  sortUtil3.cpp   sortUtilRandom.h  sortUtilRandom.cpp  common.h   common.cpp
	g++   -Wall  -std=c++11  $(DBG_FLAG)   -c -o   sortUtil1.o sortUtil1.cpp
	g++   -Wall  -std=c++11  $(DBG_FLAG)   -c -o   sortUtil2.o sortUtil2.cpp
	g++   -Wall  -std=c++11  $(DBG_FLAG)   -c -o   sortUtil3.o sortUtil3.cpp
	g++   -Wall  -std=c++11  $(DBG_FLAG)   -c -o   sortUtilRandom.o  sortUtilRandom.cpp
	g++   -Wall  -std=c++11  $(DBG_FLAG)   -c -o   common.o    common.cpp
	g++   -Wall  -std=c++11  $(DBG_FLAG)   -c -o   main.o      main.cpp
	g++   -Wall  -std=c++11  $(DBG_FLAG)      -o   main        *.o


.PHONY:clean
clean:
	-rm  -Rf  *.o*    *.exe   *.dSYM    main1  main2  main3  mainSafe  TestCase_*.txt
