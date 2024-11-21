#!/bin/bash

all:main_no_print  main_print1  main_print2  main_print12

main_no_print:main_no_print.rs
	rustc  -o   main_no_print   main_no_print.rs

main_print1:main_print1.rs
	rustc  -o   main_print1   main_print1.rs

main_print2:main_print2.rs
	rustc  -o   main_print2   main_print2.rs

main_print12:main_print12.rs
	rustc  -o   main_print12   main_print12.rs



.PHONY:clean
clean:
	-rm   -Rf   *.o*  main_no_print   main_print1 main_print2  main_print12

