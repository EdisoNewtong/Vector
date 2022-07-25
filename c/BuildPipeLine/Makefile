#!/bin/bash


# -E    Pre-Compile                                                    : main.c    -->    main.i
# -S    C/C++ source Code -> Assemble Code                             : main.i    -->    main.s
# -c    Assemble Code -> Machine Code                                  : main.s    -->    main.o
# -o    link Machine Code with system lib and generate execuable file  : main.s    -->    main.o

main:main.c
	gcc -Wall    -E    -o   main.i      main.c
	gcc -Wall    -S    -o   main.s      main.i
	gcc -Wall    -c    -o   main.o      main.s
	gcc -Wall          -o   main        main.o


.PHONY:clean
clean:
	-rm  -Rf  *.i *.s *.o main
