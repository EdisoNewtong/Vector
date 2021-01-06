#!/bin/bash

#
# first of all , you need to build lua offical source code to generate a lib named  liblua.a
# And then , copy the liblua.a to this directory
#



##################################################
#  Linux flag 
#     Compile : -DLUA_USE_LINUX
#     Link    : -Wl,-E -ldl -lreadline -lhistory -lncurses     // also see lua-5.1.5/src/Makefile      $(MAKE) all MYCFLAGS=-DLUA_USE_LINUX MYLIBS="-Wl,-E -ldl -lreadline -lhistory -lncurses"
##################################################
#  MacOS flag
#     Compile : -DLUA_USE_LINUX
#     Link    : gcc -o lua  lua.o liblua.a -lm -lreadline
##################################################


all:main

main:main.cpp luainc/luaconf.h luainc/lua.h luainc/lauxlib.h luainc/lualib.h lualib/liblua.a
	g++  -Wall -std=c++11   -Iluainc -o main   main.cpp -Llualib -llua


.PHONY:clean
clean:
	-rm   -Rf  *.o   main     *.dSYM
