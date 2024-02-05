#!/bin/bash


####################################################################################################
# The Last build command can be one of the follwoing commands
# 
# 1. 
#     $ g++  -Wall  -o   main   main.o -lmy-static-lib  -lMyAddMinus
#
# 2.
#     $ g++  -Wall  -Wl,-Lmylib -o   main   main.o   -lMyAddMinus
#
# 3.
#     $ g++  -Wall   -o   main   main.o  mylib/libMyAddMinus.a
#
#
####################################################################################################


# g++  -Wall  -o   main   main.o -Lmy-static-lib  -lMyAddMinus
# g++  -Wall  -Wl,-Lmy-static-lib -o   main   main.o   -lMyAddMinus
# g++  -Wall   -o   main   main.o  my-static-lib/libMyAddMinus.a



# -Lmy-static-lib       :  add folder 'my-static-lib' into libraries search path
# -lMyAddMinus  :  add library file :  lib +  "MyAddMinus" + ( ".a" / ".so"  ) into the final link phase  => Here the relative path : my-static-lib/libMyAddMinus.a
# library path  :  my-static-lib/libMyAddMinus.a
main:my-static-lib/libMyAddMinus.a main.o
	gcc   -Wall   -o   main   main.o  my-static-lib/libMyAddMinus.a

main.o:main.c
	gcc   -Wall  -c  -o  main.o  main.c


my-static-lib/libMyAddMinus.a:my-static-lib/my_add.o my-static-lib/my_minus.o
	ar  qc  my-static-lib/libMyAddMinus.a  my-static-lib/my_add.o   my-static-lib/my_minus.o
	ranlib  my-static-lib/libMyAddMinus.a


my-static-lib/my_add.o:my-static-lib/myAdd.c
	gcc   -Wall  -c  -o  my-static-lib/my_add.o    my-static-lib/myAdd.c


my-static-lib/my_minus.o:my-static-lib/myMinus.c
	gcc   -Wall  -c  -o  my-static-lib/my_minus.o    my-static-lib/myMinus.c


.PHONY:clean
clean:
	-rm   -Rf   my-static-lib/*.a   my-static-lib/*.o   *.exe   *.o*  *.dSYM  main

