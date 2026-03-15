#!/bin/bash

build/main:build/math/libmypow.so build/core/libutil.so  core/util.h core/util.h  main.cpp
	g++ -Wall         -c  -o  build/main.o   main.cpp
	g++ -Wall         -Wl,-rpath,"\$$ORIGIN/math" -Wl,-rpath,"\$$ORIGIN/core"   -o  build/main     build/main.o  -Lbuild/core  -Lbuild/math  -lutil -lmypow



build/math/libmypow.so:math/my_pow.cpp math/my_pow.h
	g++ -Wall         -fPIC  -c  -o  build/math/my_pow.o     math/my_pow.cpp
	g++ -Wall -shared -fPIC      -o  build/math/libmypow.so  build/math/my_pow.o   


build/core/libutil.so:build/math/libmypow.so   core/util.cpp core/util.h   math/my_pow.h
	g++ -Wall         -fPIC  -c  -Imath   -o build/core/util.o      core/util.cpp
	g++ -Wall -shared -fPIC  -Wl,-rpath,"\$$ORIGIN/../math"  -Wl,-rpath-link,"\$$ORIGIN/../math"   -o      build/core/libutil.so  build/core/util.o  



.PHONY:clean
clean:
	-rm  -Rf  build/*.o*  build/*.exe  build/main*   build/main  build/core/*.o*  build/core/*.so  build/core/*.dll    build/math/*.o*  build/math/*.so  build/math/*.dll 



