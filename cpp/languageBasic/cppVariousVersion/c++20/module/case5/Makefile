#!/bin/bash


cpp_module_flag := -Wall  -std=c++20  -fmodules-ts  


main:datamodel-m.o  datamodel.address-m.o   datamodel.address.o   datamodel.person-m.o  main.o
	g++ $(cpp_module_flag) -o main datamodel-m.o  datamodel.address-m.o   datamodel.address.o   datamodel.person-m.o  main.o


datamodel-m.o:datamodel.person-m.o   datamodel.address-m.o datamodel.address.o  datamodel-m.cpp
	g++ $(cpp_module_flag)  -c -x c++-system-header  vector
	g++ $(cpp_module_flag)  -c datamodel-m.cpp

datamodel.address-m.o:datamodel.address-m.cpp
	g++ $(cpp_module_flag)  -c datamodel.address-m.cpp

datamodel.address.o:datamodel.address.cpp
	g++ $(cpp_module_flag)  -c -x c++-system-header  iostream
	g++ $(cpp_module_flag)  -c datamodel.address.cpp

datamodel.person-m.o:datamodel.person-m.cpp
	g++ $(cpp_module_flag)  -c datamodel.person-m.cpp

main.o:main.cpp
	g++ $(cpp_module_flag)  -c main.cpp



.PHONY:clean
clean:
	-rm   -Rf   gcm.cache    *.o*   *.dSYM  *.exe   main

