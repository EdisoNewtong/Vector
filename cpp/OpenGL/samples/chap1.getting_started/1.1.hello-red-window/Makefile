#!/bin/bash

####################################################################################################
#
# Also see url :    https://learnopengl-cn.github.io/01%20Getting%20started/02%20Creating%20a%20window/
#
####################################################################################################


link_flag := -lglfw            -lGL -lX11 -pthread  -lXrandr  -lXi -ldl
# To specfy a library name , please use the following format    -l:<your-library-name>
# link_flag := -l:libglfw.so.3   -lGL -lX11 -pthread  -lXrandr  -lXi -ldl      



main:glad.o main.o 
	g++  -Wall -std=c++11  -o main glad.o  main.o   $(link_flag)

glad.o:../gladLib/src/glad.c
	g++  -Wall -std=c++11  -I../gladLib/include -c -o glad.o  ../gladLib/src/glad.c

main.o:main.cpp
	g++  -Wall -std=c++11  -I../gladLib/include  -c  -o main.o  main.cpp   



.PHONY:clean
clean:
	-rm   -Rf  *.o*  *.exe  *.dSYM    main 


