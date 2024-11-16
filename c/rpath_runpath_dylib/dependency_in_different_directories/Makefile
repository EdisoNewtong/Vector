#!/bin/bash

another_link := 
# another_link := -lmymax

# rp := -Wl,-rpath,dep/build   
rp := -Wl,-rpath,"\$$ORIGIN/../dep/build/"

rl :=
# rl := -Wl,-rpath-link,dep/build 
# rl := -Wl,-rpath-link,"\$$ORIGIN/../dep/build/"


build/main:build/main.o
	gcc -Wall -o build/main   build/main.o  -Ldep/build  -lmyadd   $(another_link)  $(rp)    $(rl)


build/main.o:main.c
	gcc -Wall -c -o build/main.o   main.c



.PHONY:clean
clean:
	-rm   -Rf     build/*.o  build/main   


