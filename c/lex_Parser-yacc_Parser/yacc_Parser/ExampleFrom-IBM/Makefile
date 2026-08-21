#!/bin/bash

calc:y.tab.c y.tab.h lex.yy.c
	gcc -Wall -o calc   y.tab.c lex.yy.c -L/usr/lib  -lfl


y.tab.c y.tab.h:calc.yacc
	yacc -d calc.yacc


lex.yy.c:calc.lex
	flex calc.lex



.PHONY:clean
clean:
	-rm   -Rf   *.tab.*  lex.yy.*  *.o* *.exe *.dSYM  calc


