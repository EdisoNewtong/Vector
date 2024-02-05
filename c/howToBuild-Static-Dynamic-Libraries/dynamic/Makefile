#!/bin/bash

####################################################################################################
#
# -Lmy-dynamic-lib    :  add folder 'my-dynamic-lib' into libraries search path
# -lMyAddMinus        :  add library file :  lib +  "MyAddMinus" + ( ".a" / ".so"  ) into the final link phase  => Here the relative path : my-dynamic-lib/libMyAddMinus.a
# library path        :  my-dynamic-lib/libMyAddMinus.so
#
####################################################################################################
# The Last build command can be one of the follwoing commands :
# 
# 1.  
#     $ gcc   -Wall   -o   main   main.o  my-dynamic-lib/libMyAddMinus.so
#
# 2.  // Build Successful But runtime error : /main: error while loading shared libraries: libMyAddMinus.so: cannot open shared object file: No such file or directory
#     $ gcc  -Wall  -Wl,-rpath=my-dynamic-lib  -o   main   main.o  -Lmy-dynamic-lib  -lMyAddMinus
#
#
####################################################################################################



main:my-dynamic-lib/libMyAddMinus.so main.o
	gcc  --verbose  -Wall  -Wl,-rpath=my-dynamic-lib  -o   main   main.o  -Lmy-dynamic-lib  -lMyAddMinus

main.o:main.c
	gcc   -Wall  -c  -o  main.o  main.c

##################################################
#
# the Key Option :   -shared -fPIC
#
##################################################
my-dynamic-lib/libMyAddMinus.so:my-dynamic-lib/my_add.o   my-dynamic-lib/my_minus.o
	gcc  -Wall -shared -fPIC -o  my-dynamic-lib/libMyAddMinus.so  my-dynamic-lib/my_add.o   my-dynamic-lib/my_minus.o

my-dynamic-lib/my_add.o:my-dynamic-lib/myAdd.c
	gcc   -Wall  -c  -o  my-dynamic-lib/my_add.o    my-dynamic-lib/myAdd.c


my-dynamic-lib/my_minus.o:my-dynamic-lib/myMinus.c
	gcc   -Wall  -c  -o  my-dynamic-lib/my_minus.o    my-dynamic-lib/myMinus.c


.PHONY:clean
clean:
	-rm   -Rf   my-dynamic-lib/*.so   my-dynamic-lib/*.o   *.exe   *.o*  *.dSYM  main

