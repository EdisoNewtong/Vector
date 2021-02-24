#!/bin/bash


######################################################################
#
# 			-lpthread
#
######################################################################

# g++   -Wall  -std=c++11   -g3   -o   main    main.cpp
# You must add  -lpthread  at link-time
#
#
# 	Otherwise , It will issue a runtime-error : 
#
# terminate called after throwing an instance of 'std::system_error'
#   what():  Enable multithreading to use std::thread: Operation not permitted
#   已放弃(吐核)
#
#
#[edison@localhost chap01]$ ./main 
#terminate called after throwing an instance of 'std::system_error'
#  what():  Enable multithreading to use std::thread: Operation not permitted
#		   已放弃(吐核)
#
#




all:main.cpp
	g++   -Wall  -std=c++11  -lpthread  -g3  -c  -o  main.o    main.cpp
	g++   -lpthread -o  main    main.o
	


.PHONY:clean
clean:
	-rm   -Rf    main   *.o    *.exe

