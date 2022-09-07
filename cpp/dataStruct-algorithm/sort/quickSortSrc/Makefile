#!/bin/bash

# -Wno-narrowing
# g++  -Wall -Wno-narrowing  -std=c++11   -o main      main.cpp



############################################################################################################################################
# 
# main:main.cpp sortUtil.h  sortUtil.cpp     sortUtil_safe.cpp sortUtil_safe.h    oldSortingVersion.cpp
# 	g++   -Wall  -std=c++11   -c -o   sortUtil.o         sortUtil.cpp
# 	g++   -Wall  -std=c++11   -c -o   sortUtil_safe.o    sortUtil_safe.cpp
# 	g++   -Wall  -std=c++11   -c -o   oldSortingVersion.o    oldSortingVersion.cpp
# 	g++   -Wall  -std=c++11   -c -o   main.o    main.cpp
# 	g++   -Wall  -std=c++11      -o   main     main.o oldSortingVersion.o sortUtil.o  sortUtil_safe.o
# 
############################################################################################################################################






USE_SORT_TYPE_1_FLAG := -DSORT_TYPE=1
USE_SORT_TYPE_2_FLAG := -DSORT_TYPE=2
USE_SORT_SAFELY      := -DINDEX_CHECK_SAFE




main1:main.cpp  sortUtil.h  sortUtil.cpp  
	@echo "Build Quick-Sort Version-1"
	g++   -Wall  -std=c++11   $(USE_SORT_TYPE_1_FLAG)  -c -o   sortUtil.o         sortUtil.cpp
	g++   -Wall  -std=c++11   $(USE_SORT_TYPE_1_FLAG)  -c -o   main.o    main.cpp
	g++   -Wall  -std=c++11   $(USE_SORT_TYPE_1_FLAG)     -o   main1      main.o    sortUtil.o


main2:main.cpp  sortUtil2.h  sortUtil2.cpp  
	@echo "Build Quick-Sort Version-2"
	g++   -Wall  -std=c++11   $(USE_SORT_TYPE_2_FLAG)  -c -o   sortUtil2.o         sortUtil2.cpp
	g++   -Wall  -std=c++11   $(USE_SORT_TYPE_2_FLAG)  -c -o   main.o    main.cpp
	g++   -Wall  -std=c++11   $(USE_SORT_TYPE_2_FLAG)     -o   main2     main.o    sortUtil2.o



mainSafe:main.cpp sortUtil_safe.h  sortUtil_safe.cpp
	@echo "Build Quick-Sort Safely Check Index Version"
	g++   -Wall  -std=c++11   $(USE_SORT_SAFELY)  -c -o   sortUtil_safe.o         sortUtil_safe.cpp
	g++   -Wall  -std=c++11   $(USE_SORT_SAFELY)  -c -o   main.o    main.cpp
	g++   -Wall  -std=c++11   $(USE_SORT_SAFELY)     -o   mainSafe     main.o    sortUtil_safe.o




.PHONY:clean
clean:
	-rm  -Rf  *.o*    *.exe   *.dSYM    main1 main2   mainSafe
