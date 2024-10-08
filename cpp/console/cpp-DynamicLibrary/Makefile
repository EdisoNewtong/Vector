#!/bin/bash

##########################################################################################################################################################################################################################################################
#
# -rpath indicate the the dynamic library should not placed at the same path of the execuable file
# it can be searched at the path where -rpath specified 
#
##########################################################################################################################################################################################################################################################
search_opt := -Wl,-rpath=myDylib

main:myDylib/libUtil.so   main.o 
	g++ -Wall -std=c++11   $(search_opt)  -o main   main.o     -LmyDylib   -lUtil 


main.o:main.cpp  myDylib/util.h
	g++ -Wall -std=c++11 -ImyDylib -c  -o main.o   main.cpp 

##########################################################################################################################################################################################################################################################
#
# Notes that the xxx.cpp -->  xxx.o  must use the  -shared  -fPIC  to build the object file
# Otherwise the linker will raise an error like the following : 
#
##########################################################################################################################################################################################################################################################
#
#            /usr/bin/ld: myDylib/util.o: warning: relocation against `_ZTV4Util' in read-only section `.text'
#            /usr/bin/ld: myDylib/util.o: relocation R_X86_64_PC32 against symbol `_ZTV4Util' can not be used when making a shared object; recompile with -fPIC
#            /usr/bin/ld: 最后的链结失败: bad value
#            collect2: 错误：ld 返回 1
#            make: *** [Makefile:18：myDylib/libUtil.so] 错误 1
#
##########################################################################################################################################################################################################################################################
myDylib/libUtil.so:myDylib/util.cpp  myDylib/util.h
	g++  -Wall -std=c++11  -shared  -fPIC  -ImyDylib  -c  -o   myDylib/util.o       myDylib/util.cpp
	g++  -Wall -std=c++11  -shared  -fPIC  -ImyDylib      -o   myDylib/libUtil.so   myDylib/util.o


.PHONY:clean
clean:
	-rm   -Rf  myDylib/*.so   myDylib/*.o     *.o   *.exe  *.dSYM   main 


