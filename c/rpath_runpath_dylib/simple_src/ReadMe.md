# Directory Tree Struct
simple_src/
├── build
│   ├── libmyAdd.so
│   ├── main
│   ├── main.o
│   ├── myAdd.o
│   └── tmp
├── main.c
├── Makefile
├── myAdd.c
└── myAdd.h

1 directory, 9 files


# Link Flag by Relative path or Absolute path
if the linker flag use  **==relative path==**

    **==-Wl,-rpath,build==**

The program will search all dynamic libraries from the path build/  ( the loader path will be written into the binary program inside itself )


if the linker flag use  **==absolute path==**

    **==-Wl,-rpath,"\$$ORIGIN"**

The program will search all dynamic libraries from the path of the binary program's path



## Relative Path  **==-Wl,-rpath,build==**

```bash
$ pwd
/home/edison/work/gitrepo/myrepo/Vector/c/rpath_runpath_dylib/simple_src
edison@mxlinux:~/work/gitrepo/myrepo/Vector/c/rpath_runpath_dylib/simple_src
$ ls
build  main.c  Makefile  myAdd.c  myAdd.h

#################################################################################################################################
#
# Run-Time execute Successfully at path : simple_src/
#
#################################################################################################################################
edison@mxlinux:~/work/gitrepo/myrepo/Vector/c/rpath_runpath_dylib/simple_src
$ ./build/main 
1+2 = 3, my_add(1,2) = 30. Why the 2 values are not same ?
Because the implementation of function int my_add(a,b) is very special.


#################################################################################################################################
#
# Run-Time execute Failed #1 :  at path : build/
#
#################################################################################################################################
edison@mxlinux:~/work/gitrepo/myrepo/Vector/c/rpath_runpath_dylib/simple_src
$ cd build/
edison@mxlinux:~/work/gitrepo/myrepo/Vector/c/rpath_runpath_dylib/simple_src/build
$ ./main
./main: error while loading shared libraries: libmyAdd.so: cannot open shared object file: No such file or directory


#################################################################################################################################
#
# Run-Time execute Failed #2 :  at the parent directory of  simple_src/  ( At path  rpath_runpath_dylib/ )
#
#################################################################################################################################
edison@mxlinux:~/work/gitrepo/myrepo/Vector/c/rpath_runpath_dylib/simple_src/build
$ cd ../../
edison@mxlinux:~/work/gitrepo/myrepo/Vector/c/rpath_runpath_dylib
$ ./simple_src/build/main 
./simple_src/build/main: error while loading shared libraries: libmyAdd.so: cannot open shared object file: No such file or directory


```




## Absolute Path **==-Wl,-rpath,"\$$ORIGIN"**

```bash
$ pwd
/home/edison/work/gitrepo/myrepo/Vector/c/rpath_runpath_dylib/simple_src
edison@mxlinux:~/work/gitrepo/myrepo/Vector/c/rpath_runpath_dylib/simple_src
$ ls
build  main.c  Makefile  myAdd.c  myAdd.h  ReadMe.md
edison@mxlinux:~/work/gitrepo/myrepo/Vector/c/rpath_runpath_dylib/simple_src

#################################################################################################################################
#
# Run-Time execute Successfully at path#1 : simple_src/
#
#################################################################################################################################
$ ./build/main
1+2 = 3, my_add(1,2) = 30. Why the 2 values are not same ?
Because the implementation of function int my_add(a,b) is very special.


#################################################################################################################################
#
# Run-Time execute Successfully at path#2 : build/
#
#################################################################################################################################
edison@mxlinux:~/work/gitrepo/myrepo/Vector/c/rpath_runpath_dylib/simple_src
$ cd build/
edison@mxlinux:~/work/gitrepo/myrepo/Vector/c/rpath_runpath_dylib/simple_src/build
$ ./main 
1+2 = 3, my_add(1,2) = 30. Why the 2 values are not same ?
Because the implementation of function int my_add(a,b) is very special.


#################################################################################################################################
#
# Run-Time execute Successfully at path#3 : the parent path of simple_src/ ( At path  rpath_runpath_dylib/ )
#
#################################################################################################################################
edison@mxlinux:~/work/gitrepo/myrepo/Vector/c/rpath_runpath_dylib/simple_src/build
$ cd ../../
edison@mxlinux:~/work/gitrepo/myrepo/Vector/c/rpath_runpath_dylib
$ ./simple_src/build/main 
1+2 = 3, my_add(1,2) = 30. Why the 2 values are not same ?
Because the implementation of function int my_add(a,b) is very special.

```



