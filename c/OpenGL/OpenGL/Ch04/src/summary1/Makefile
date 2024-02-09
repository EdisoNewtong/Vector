# makefile


linux:summary1.c
	gcc  -Wall  -o  summary1  summary1.c    -lglut  -lGL  -lGLU

macosx:summary1.c
	gcc -g -Wall -o summary1 summary1.c -framework OPENGL -framework GLUT


.PHONY:clean
clean:
	rm -Rf *.o *.dSYM summary1

