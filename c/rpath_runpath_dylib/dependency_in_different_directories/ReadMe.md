# How to build binary program whose dependency is not in the same directory path by the program

```bash
# Build dynamic libraries
$ cd dep
####################################################################################################
#
# You must make sure that the relative path between the 2 dynamic libraries  have to keep relationship by the   -rpath link flag
# The search path of libmyadd.so for the dependency will be written into the libA.so ( the libmyadd.so  depend on libmymax.so  )
#
####################################################################################################
$ make



# Build binary Program
$ cd ../
$ make

####################################################################################################
#
# You must make sure that the relative path from program to the dependency have to keep relationship by the   -rpath link flag
# The search path of libmyadd.so which is the dependency of the binary program will be written into the main ( the name of the binary program )
#
####################################################################################################
$ ./build/main

```


