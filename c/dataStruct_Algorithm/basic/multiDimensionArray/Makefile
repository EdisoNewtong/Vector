#!/bin/bash

main:dynamicArray.o staticArray.o  main.o
	gcc  -std=c99  -Wall  -o   main               dynamicArray.o  staticArray.o  main.o



dynamicArray.o:dynamicArray.c
	gcc  -std=c99  -Wall  -c   -o dynamicArray.o  dynamicArray.c

staticArray.o:staticArray.c
	gcc  -std=c99  -Wall  -c   -o staticArray.o  staticArray.c

main.o:main.c
	gcc  -std=c99  -Wall  -c   -o main.o          main.c



.PHONY:clean
clean:
	-rm  -Rf *.o*  *.exe   *.dYSM  main
