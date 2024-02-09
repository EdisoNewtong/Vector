# makefile


linux:mainQuads.c
	gcc  -Wall  -o  mainQuads  mainQuads.c    -lglut  -lGL  -lGLU

macosx:mainQuads.c
	gcc -g -Wall -o mainQuads mainQuads.c -framework OPENGL -framework GLUT

.PHONY:clean
clean:
	rm -Rf *.o *.dSYM mainQuads

