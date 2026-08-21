#!/bin/bash


calc:calc.tab.c calc.tab.h lex.yy.c 
	gcc -Wall calc.tab.c lex.yy.c  -o calc -L/usr/lib -lfl


calc.tab.c:calc.y
	bison -d calc.y


lex.yy.c:calc.l
	flex calc.l




.PHONY:clean
clean:
	-rm -Rf lex.yy.c calc.tab* *.o* *.exe *.dSYM  calc 


