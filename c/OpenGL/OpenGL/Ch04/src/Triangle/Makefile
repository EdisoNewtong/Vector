# makefile

linux:mainTriangle.c
	gcc  -Wall  -o  mainTriangle  mainTriangle.c    -lglut  -lGL  -lGLU


macosx:mainTriangle.c
	gcc -g -Wall -o mainTriangle mainTriangle.c -framework OPENGL -framework GLUT


.PHONY:clean
clean:
	rm -Rf *.o *.dSYM mainTriangle

