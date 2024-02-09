# makefile

linux:mainRect.c
	gcc  -Wall  -o  mainRect  mainRect.c    -lglut  -lGL  -lGLU

macosx:mainRect.c
	gcc -g -Wall -o mainRect mainRect.c -framework OPENGL -framework GLUT

.PHONY:clean
clean:
	rm -Rf *.o *.dSYM mainRect

