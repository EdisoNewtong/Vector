
$ cat /etc/centos-release
CentOS Linux release 7.9.2009 (Core)

/////////////////////////////////////////////////////////////////////
// switch gcc to the version which support c++20    
//
$ scl enable devtoolset-11 bash
or 
$ source /opt/rh/devtoolset-11/enable
/////////////////////////////////////////////////////////////////////
 

$ g++ -v
Using built-in specs.
COLLECT_GCC=g++
COLLECT_LTO_WRAPPER=/opt/rh/devtoolset-11/root/usr/libexec/gcc/x86_64-redhat-linux/11/lto-wrapper
Target: x86_64-redhat-linux
Configured with: ../configure --enable-bootstrap --enable-languages=c,c++,fortran,lto --prefix=/opt/rh/devtoolset-11/root/usr --mandir=/opt/rh/devtoolset-11/root/usr/share/man --infodir=/opt/rh/devtoolset-11/root/usr/share/info --with-bugurl=http://bugzilla.redhat.com/bugzilla --enable-shared --enable-threads=posix --enable-checking=release --enable-multilib --with-system-zlib --enable-__cxa_atexit --disable-libunwind-exceptions --enable-gnu-unique-object --enable-linker-build-id --with-gcc-major-version-only --with-linker-hash-style=gnu --with-default-libstdcxx-abi=gcc4-compatible --enable-plugin --enable-initfini-array --with-isl=/builddir/build/BUILD/gcc-11.2.1-20210728/obj-x86_64-redhat-linux/isl-install --enable-gnu-indirect-function --with-tune=generic --with-arch_32=x86-64 --build=x86_64-redhat-linux
Thread model: posix
Supported LTO compression algorithms: zlib
gcc version 11.2.1 20210728 (Red Hat 11.2.1-1) (GCC) 


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
$ make
g++  -Wall -std=c++20 -x  c++-system-header  -std=c++20   -fmodule-header  /opt/rh/devtoolset-11/root/usr/include/c++/11/iostream
g++  -Wall -std=c++20 -fmodules-ts  -c -o hello.o     hello.cpp
g++  -Wall -std=c++20 -fmodules-ts  -c -o main.o      main.cpp
g++  -Wall -std=c++20 -fmodules-ts     -o main        main.o hello.o


