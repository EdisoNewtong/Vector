# makefile

linux:summary2.c
	gcc  -Wall  -o  summary2  summary2.c    -lglut  -lGL  -lGLU

macosx:summary2.c
	gcc -g -Wall -o summary2 summary2.c -framework OPENGL -framework GLUT


.PHONY:clean
clean:
	rm -Rf *.o *.dSYM summary2

