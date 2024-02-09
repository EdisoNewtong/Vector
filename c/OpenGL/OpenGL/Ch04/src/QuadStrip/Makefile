# makefile

linux:mainQuadStrip.c
	gcc  -Wall  -o  mainQuadStrip  mainQuadStrip.c    -lglut  -lGL  -lGLU


macosx:mainQuadStrip.c
	gcc -g -Wall -o mainQuadStrip mainQuadStrip.c -framework OPENGL -framework GLUT


.PHONY:clean
clean:
	rm -Rf *.o *.dSYM mainQuadStrip

