#!/bin/bash


linux:main.c
	gcc -g -Wall -o main main.c -lglut -lGL -lGLU

macosx:main.c
	gcc -g -Wall -o main main.c -framework OPENGL -framework GLUT


clean:
	-rm -Rf *.o* *.exe *.dSYM  main

