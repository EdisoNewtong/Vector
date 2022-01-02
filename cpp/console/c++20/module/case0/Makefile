#!/bin/bash

# SHELL := /bin/bash

# preBuildFileName := iostream.gcm
preBuildFileName := iostream.gcm


newFolderName := gcm.cache/opt/rh/devtoolset-11/root/usr/include/c++/11
newPreBuildOutputName := $(newFolderName)/$(preBuildFileName)


# CHECK_DIR = $(shell if [ -d $(newFolderName) ]; then : ; else echo "[INFO] : Make New Folder Now ..."; mkdir -p $(newFolderName); fi)
# $(info $(CHECK_DIR))


systemHeaderFile := /opt/rh/devtoolset-11/root/usr/include/c++/11/iostream

option0 := -Wall
option1 := -Wall -std=c++20
option2 := -fmodules-ts

allopt := $(option0) $(option1) $(option2)
# allopt := $(option2) 




main:hello.cpp $(newPreBuildOutputName) main.cpp
	g++  -Wall  $(allopt)  -c -o hello.o     hello.cpp
	g++  -Wall  $(allopt)  -c -o main.o      main.cpp
	g++  -Wall  $(allopt)     -o main        main.o hello.o

$(newPreBuildOutputName):$(systemHeaderFile)
	g++  -Wall -std=c++20 -x  c++-system-header  -std=c++20   -fmodule-header  $(systemHeaderFile)



# .ONESHELL:



.PHONY:clean
clean:
	-rm   -Rf   gcm.cache    *.o  *.obj  *.exe *.dSYM *.a  main   

