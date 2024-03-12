#!/bin/bash

cpp_module_flag := -Wall  -std=c++20  -fmodules-ts  


main:personDeclear.o  personImpl.o  main.o
	g++   $(cpp_module_flag)   -o  main   personDeclear.o  personImpl.o  main.o 


##################################################
#    Module Part Begin
##################################################

####################################################################################### 
#     $ make
###   g++   -Wall  -std=c++20  -fmodules-ts    -c -x c++-system-header  string
###   g++   -Wall  -std=c++20  -fmodules-ts    -c  -o personDeclear.o   personDeclear.ixx
###   g++: 警告：personDeclear.ixx：未使用链接器输入文件，因为链接尚未完成
###   g++   -Wall  -std=c++20  -fmodules-ts    -c  -o personImpl.o      personImpl.cpp
###   In module imported at personImpl.cpp:1:1:
###   person: 错误：failed to read compiled module: 没有那个文件或目录
###   person: 附注：compiled module file is ‘gcm.cache/person.gcm’
###   person: 附注：imports must be built before being imported
###   person: 致命错误：returning to the gate for a mechanical issue
###   编译中断。
###   make: *** [Makefile:28：personImpl.o] 错误 1
### 
### 
####################################################################################### 


#
# Module Declearation code 
#
# Notes : the Declear file name' extension being compiled by g++    can't  .h  or  .cppm  or .ixx
#
#
#                        Compile
# personDeclear.cpp                 gcm.cache/person.gcm
#                     ----------->
personDeclear.o:personDeclear.cpp
	g++   $(cpp_module_flag)  -c -x c++-system-header  string
	g++   $(cpp_module_flag)  -c  -o personDeclear.o   personDeclear.cpp



#
# Module Implementation code
#
personImpl.o:personImpl.cpp
	g++   $(cpp_module_flag)  -c  -o personImpl.o      personImpl.cpp

##################################################
#    Module Part End
##################################################


main.o:main.cpp
	g++   $(cpp_module_flag)  -c -x c++-system-header  iostream
	g++   $(cpp_module_flag)  -c  -o  main.o  main.cpp


.PHONY:clean
clean:
	-rm   -Rf   gcm.cache   *.o   *.dSYM  *.exe  main



