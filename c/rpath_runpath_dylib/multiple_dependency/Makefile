#!/bin/bash

noset_Error:main.c libutil.so
	-rm  -Rf   *.exe  main build/*.o*
	gcc -Wall -c -o build/main.o  main.c
	gcc -Wall -o main    build/main.o  -L. -lutil


noset_1:main.c libutil.so
	-rm  -Rf   *.exe  main build/*.o*
	gcc -Wall -c -o build/main.o  main.c
	gcc -Wall -o main    build/main.o  -L. -lutil -Wl,-rpath,build

noset_2:main.c libutil.so
	-rm  -Rf   *.exe  main build/*.o*
	gcc -Wall -c -o build/main.o  main.c
	gcc -Wall -o main    build/main.o  -L. -lutil -Wl,-rpath-link,build

# set  RUNPATH 0
myRpath0:main.c build/my_rpath/libutil.so
	-rm  -Rf   *.exe  main build/*.o*
	gcc -Wall -c -o build/main.o  main.c
	gcc -Wall -o main    build/main.o  -Lbuild/my_rpath -lutil  -Wl,-rpath,build  -Wl,-rpath,build/my_rpath   -Wl,-rpath-link,build

# set  RUNPATH 1   with    $ORIGIN   syntax
myRpath1:main.c build/my_rpath/libutil.so
	-rm  -Rf   *.exe  main build/*.o*
	gcc -Wall -c -o build/main.o  main.c
	gcc -Wall -o main    build/main.o  -Lbuild/my_rpath -lutil  -Wl,-rpath,"\$$ORIGIN/build/my_rpath"


# set  rpath   <==    -Wl,--disable-new-dtag
myRpath_dis:main.c build/my_rpath/libutil.so
	-rm  -Rf   *.exe  main build/*.o*
	gcc -Wall -c -o build/main.o  main.c
	gcc -Wall -o main    build/main.o  -Lbuild/my_rpath -lutil -Wl,--disable-new-dtag  -Wl,-rpath,build/my_rpath


# set  RUNPATH   <==    -Wl,--enable-new-dtag
myRunpath_ena:main.c build/my_runpath/libutil.so
	-rm  -Rf   *.exe  main build/*.o*
	gcc -Wall -c -o build/main.o  main.c
	gcc -Wall -o main    build/main.o  -Lbuild/my_runpath -lutil -Wl,--enable-new-dtag  -Wl,-rpath,build/my_runpath


dylib:dylib_src/util.c  dylib_src/util_rpath.c   dylib_src/util_runpath.c  dylib_src/common.c
	gcc -Wall -shared -fPIC -o build/libcommon.so            dylib_src/common.c
	gcc -Wall -shared -fPIC -o libutil.so                    dylib_src/util.c   -Lbuild -lcommon
	gcc -Wall -shared -fPIC -o build/my_rpath/libutil.so     dylib_src/util_rpath.c -Lbuild -lcommon      -Wl,-rpath,build
	gcc -Wall -shared -fPIC -o build/my_runpath/libutil.so   dylib_src/util_runpath.c -Lbuild -lcommon


.PHONY:clean
clean:
	find . -iname '*.so' -exec rm -Rf {} \;
	-rm  -Rf build/*.o*  *.o   *.exe  *.dSYM   main

