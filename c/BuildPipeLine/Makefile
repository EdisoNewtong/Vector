#!/bin/bash


# 1. PreCompile : -E    Pre-Compile                                                    : main.c    -->    main.i
# 2. Compile    : -S    C/C++ source Code -> Assemble Code                             : main.i    -->    main.s
# 3. Assemble   : -c    Assemble Code -> Machine Code                                  : main.s    -->    main.o
# 4. link       : -o    link Machine Code with system lib and generate execuable file  : main.s    -->    main ( a binary executeable file with machine code )

main:main.c
	gcc -Wall    -E    -o   main.i      main.c
	gcc -Wall    -S    -o   main.s      main.i
	gcc -Wall    -c    -o   main.o      main.s
	gcc -Wall          -o   main        main.o


.PHONY:clean
clean:
	-rm  -Rf  *.i *.s *.o *.exe   main