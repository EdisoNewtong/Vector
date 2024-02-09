# makefile

linux:mainTriangleFan.c
	gcc  -Wall  -o  mainTriangleFan  mainTriangleFan.c    -lglut  -lGL  -lGLU


macosx:mainTriangleFan.c
	gcc -g -Wall -o mainTriangleFan mainTriangleFan.c -framework OPENGL -framework GLUT

.PHONY:clean
clean:
	rm -Rf *.o *.dSYM mainTriangleFan

