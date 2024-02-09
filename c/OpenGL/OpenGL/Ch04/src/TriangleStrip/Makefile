# makefile

linux:mainTriangleStrip.c
	gcc  -Wall  -o  mainTriangleStrip  mainTriangleStrip.c    -lglut  -lGL  -lGLU

macosx:mainTriangleStrip.c
	gcc -g -Wall -o mainTriangleStrip mainTriangleStrip.c -framework OPENGL -framework GLUT

.PHONY:clean
clean:
	rm -Rf *.o *.dSYM mainTriangleStrip

