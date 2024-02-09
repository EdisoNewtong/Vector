# makefile

linux:mainCube.c
	gcc  -Wall  -o  mainCube    mainCube.c  -lglut  -lGL  -lGLU

macosx:mainCube.c
	gcc -g -Wall -o mainCube mainCube.c -framework OPENGL -framework GLUT

.PHONY:clean
clean:
	rm -Rf *.o *.dSYM mainCube

