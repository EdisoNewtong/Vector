# makefile

linux:main.c
	gcc  -Wall  -o  main  main.c    -lglut  -lGL  -lGLU

macosx:main.c
	gcc -g -Wall -o main main.c -framework OPENGL -framework GLUT

clean:
	rm -Rf *.o main main.dSYM

