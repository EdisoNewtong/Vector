# makefile


linux:mainPolygon.c
	gcc  -Wall  -o  mainPolygon  mainPolygon.c    -lglut  -lGL  -lGLU

macosx:mainPolygon.c
	gcc -g -Wall -o mainPolygon mainPolygon.c -framework OPENGL -framework GLUT

.PHONY:clean
clean:
	rm -Rf *.o *.dSYM mainPolygon

