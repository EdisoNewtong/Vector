# makefile

linux:summary3.c
	gcc  -Wall  -o  summary3  summary3.c    -lglut  -lGL  -lGLU

macosx:summary3.cpp
	g++ -g -Wall -o summary3 summary3.cpp -framework OPENGL -framework GLUT


.PHONY:clean
clean:
	rm -Rf *.o *.dSYM summary3

