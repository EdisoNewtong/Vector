#!/bin/bash

####################################################################################################
#
# The order of -rpath=1stplace    -rpath=2ndplace   is much more critical  
# 1st: search shared library in dir "mylib"
# 2nd: search shared library in dir "testOnly"
#
####################################################################################################

main:mylib/libMyAdd.so main.cpp 
	g++  -Wall -std=c++11  -fPIC -Imylib                                         -c   -o  main.o   main.cpp 
	g++  -Wall -std=c++11  -fPIC -Imylib \
                                          -Wl,-rpath=mylib \
                                          -Wl,-rpath=testOnly \
                            -o  main     main.o    -Lmylib  -lMyAdd

mylib/libMyAdd.so:mylib/myAdd.c
	gcc   -Wall   -shared   -fPIC    -c   -o    mylib/myAdd.o        mylib/myAdd.c
	gcc   -Wall   -shared   -fPIC         -o    mylib/libMyAdd.so    mylib/myAdd.o





.PHONY:clean
clean:
	-rm   -Rf  *.so  *.o  *.exe *.dSYM  mylib/*.o   mylib/*.so    main



