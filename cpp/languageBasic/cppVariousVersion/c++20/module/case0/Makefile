#!/bin/bash


preBuildFileName := iostream.gcm


# newFolderName := gcm.cache/opt/rh/devtoolset-11/root/usr/include/c++/11
# newFolderName := gcm.cache/usr/include/c++/12.1.0

newPreBuildOutputName := $(preBuildFileName)


# systemHeaderFile := /opt/rh/devtoolset-11/root/usr/include/c++/11/iostream
systemHeaderFile := /usr/include/c++/12.1.0/iostream


option1 := -Wall -std=c++20
option2 := -fmodules-ts

allopt := $(option1) $(option2)




main:$(newPreBuildOutputName)  hello.cpp   main.cpp
	g++  $(allopt)  -c -o hello.o     hello.cpp
	g++  $(allopt)  -c -o main.o      main.cpp
	g++  $(allopt)     -o main        main.o hello.o


$(newPreBuildOutputName):$(systemHeaderFile)
	g++  -Wall -std=c++20 -x  c++-system-header -fmodule-header  $(systemHeaderFile)


.PHONY:clean
clean:
	-rm   -Rf   gcm.cache    *.o  *.obj  *.exe *.dSYM *.a  main   

