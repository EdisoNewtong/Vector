#!/bin/bash

inc_header_path :=  -I../../commonHeader

####################################################################################################
# the program who use the function declared in <pthread.h> must be link with the    libpthread.a     
# LinkFlag : -lpthread
#
# else :  
#	The Following Link Error will occur :
#
####################################################################################################
#
# gcc -Wall -o  alarm_thread  alarm_thread.o
# alarm_thread.o：在函数‘alarm_thread’中：
# alarm_thread.c:(.text+0x1d)：对‘pthread_detach’未定义的引用
# alarm_thread.o：在函数‘main’中：
# alarm_thread.c:(.text+0x1d8)：对‘pthread_create’未定义的引用
# collect2: 错误：ld 返回 1
# make: *** [alarm_thread] 错误 1
#
####################################################################################################
thread_link_flag := -lpthread

all:alarm alarm_fork alarm_thread
	
	
alarm.o:alarm.c
	gcc  -Wall $(inc_header_path) -c -o alarm.o alarm.c

alarm:alarm.o
	gcc -Wall -o  alarm  alarm.o



alarm_fork.o:alarm_fork.c
	gcc -Wall $(inc_header_path) -c -o  alarm_fork.o  alarm_fork.c

alarm_fork:alarm_fork.o
	gcc  -Wall  -o alarm_fork  alarm_fork.o
	

alarm_thread.o:alarm_thread.c
	gcc  -Wall  $(inc_header_path) -c -o alarm_thread.o alarm_thread.c



alarm_thread:alarm_thread.o
	gcc -Wall -o  alarm_thread  $(thread_link_flag) alarm_thread.o



.PHONY:clean
clean:
	-rm  -Rf   *.o   alarm  alarm_fork  alarm_thread
