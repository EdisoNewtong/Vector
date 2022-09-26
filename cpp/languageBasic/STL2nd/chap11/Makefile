#!/bin/bash


main:build/main.o  build/NonModifying.o   build/Modifying.o  build/Removing.o  build/Mutating.o  build/Sorting.o  build/SortedRange.o    build/Numeric.o
	g++  -Wall  -std=c++11   -o    main  build/*.o

build/main.o:main.cpp
	g++  -Wall  -std=c++11   -c  -o   build/main.o   main.cpp

build/NonModifying.o:NonModifying.cpp  NonModifying.h
	g++  -Wall  -std=c++11   -c  -o   build/NonModifying.o   NonModifying.cpp

build/Modifying.o:Modifying.cpp  Modifying.h
	g++  -Wall  -std=c++11   -c  -o   build/Modifying.o   Modifying.cpp

build/Removing.o:Removing.cpp  Removing.h
	g++  -Wall  -std=c++11   -c  -o   build/Removing.o   Removing.cpp

build/Mutating.o:Mutating.cpp  Mutating.h
	g++  -Wall  -std=c++11   -c  -o   build/Mutating.o   Mutating.cpp

build/Sorting.o:Sorting.cpp  Sorting.h
	g++  -Wall  -std=c++11   -c  -o   build/Sorting.o   Sorting.cpp

build/SortedRange.o:SortedRange.cpp  SortedRange.h
	g++  -Wall  -std=c++11   -c  -o   build/SortedRange.o   SortedRange.cpp

build/Numeric.o:Numeric.cpp  Numeric.h
	g++  -Wall  -std=c++11   -c  -o   build/Numeric.o   Numeric.cpp




.PHONY:clean
clean:
	-rm   -Rf   build/*.o*   *.exe   main
