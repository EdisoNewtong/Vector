#!/bin/bash

build/gen:gen.cpp
	g++   -Wall  -std=c++11    -O3  -o   build/gen   gen.cpp



.PHONY:clean
clean:
	-rm   -Rf   build/*.o*  build/*.exe   build/*.dSYM    build/gen

