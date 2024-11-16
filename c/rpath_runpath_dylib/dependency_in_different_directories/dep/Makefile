#!/bin/bash

build/libmyadd.so:build/myadd.o build/libmymax.so
	gcc  -Wall -shared -fPIC -o build/libmyadd.so   build/myadd.o  -Lbuild -lmymax  -Wl,-rpath,"\$$ORIGIN"  -Wl,-rpath-link,"\$$ORIGIN"

build/myadd.o:myadd.c
	gcc  -Wall  -c -o  build/myadd.o   myadd.c

build/libmymax.so:build/mymax.o
	gcc  -Wall -shared -fPIC -o build/libmymax.so   build/mymax.o

build/mymax.o:mymax.c
	gcc  -Wall  -c -o  build/mymax.o   mymax.c


.PHONY:clean
clean:
	-rm   -Rf    build/*.o  build/*.so




