# Compiler prerequisite 

```bash
$ g++ -v
使用内建 specs。
COLLECT_GCC=g++
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-pc-linux-gnu/13.2.1/lto-wrapper
目标：x86_64-pc-linux-gnu
配置为：/build/gcc/src/gcc/configure --enable-languages=ada,c,c++,d,fortran,go,lto,objc,obj-c++ --enable-bootstrap --prefix=/usr --libdir=/usr/lib --libexecdir=/usr/lib --mandir=/usr/share/man --infodir=/usr/share/info --with-bugurl=https://bugs.archlinux.org/ --with-build-config=bootstrap-lto --with-linker-hash-style=gnu --with-system-zlib --enable-__cxa_atexit --enable-cet=auto --enable-checking=release --enable-clocale=gnu --enable-default-pie --enable-default-ssp --enable-gnu-indirect-function --enable-gnu-unique-object --enable-libstdcxx-backtrace --enable-link-serialization=1 --enable-linker-build-id --enable-lto --enable-multilib --enable-plugin --enable-shared --enable-threads=posix --disable-libssp --disable-libstdcxx-pch --disable-werror
线程模型：posix
支持的 LTO 压缩算法：zlib zstd
gcc 版本 13.2.1 20230801 (GCC) 
```

# Compile flag 

```bash
// Generate a module object file :    gcm.cache/HelloModule.gcm
//
// Notes the file name is   myFirstModule.cpp
// inside the source file , the content indicate its module name as followed :
//        export module HelloModule;
// so the output module compiled object file is   HelloModule.gcm
//
$ g++   -Wall  -std=c++20  -fmodules-ts    -c  -o  myFirstModule.o  myFirstModule.cpp



//
// if the source file contain the following statement to import c++'s language basic header file as following :
// import <iostream>;
// must pre-compile the basic header file ( <iostream> )   before compile the source file itself ( main.cpp )

$ g++   -Wall  -std=c++20  -xc++-system-header   iostream  
// or 
$ g++   -Wall  -std=c++20  -x c++-system-header  iostream


```


