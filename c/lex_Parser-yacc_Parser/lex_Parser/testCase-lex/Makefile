#!/bin/bash


# must link the library   "libfl.a"   or  "libfl.so"
# otherwise build failed at link phase
myLexProgram:example.yy.c
	gcc  -o  myLexProgram    example.yy.c  -lfl


# Must specify the output file name by the following option : -o example.yy.c 
# otherwise the command will output a default file named "lex.yy.c"
example.yy.c:example.l
	flex  -o example.yy.c  example.l


.PHONY:clean
clean:
	-rm  -Rf   *.yy.c  myLexProgram


