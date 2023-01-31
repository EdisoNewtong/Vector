#!/bin/bash

all:main calcNextAry


main:main.c
	gcc   -Wall   -o    main    main.c

calcNextAry:calcNextAry.c
	gcc   -Wall   -o    calcNextAry    calcNextAry.c


.PHONY:clean
clean:
	-rm    -Rf   *.o*   *.exe   *.dSYM   main  calcNextAry
