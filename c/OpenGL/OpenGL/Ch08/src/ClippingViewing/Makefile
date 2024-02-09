# makefile

linux:main.cpp
	g++  -Wall  -o  main  main.cpp    -lglut  -lGL  -lGLU

macosx:main.cpp
	g++ -g -Wall -o main main.cpp -framework OPENGL -framework GLUT


.PHONY:clean
clean:
	rm -Rf *.o *.dSYM main


