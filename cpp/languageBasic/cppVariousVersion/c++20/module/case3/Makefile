#!/bin/bash



option1 := -Wall -std=c++20
option2 := -fmodules-ts

allopt := $(option1) $(option2)


main:hello.cpp   main.cpp
	g++  $(allopt)  -c -o hello.o     hello.cpp
	g++  $(allopt)  -c -o main.o      main.cpp
	g++  $(allopt)     -o main        main.o hello.o


.PHONY:clean
clean:
	-rm   -Rf   gcm.cache    *.o  *.obj  *.exe *.dSYM *.a  main   

