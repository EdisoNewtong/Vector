# How to build a lua code as a shared libary 

//
// Under mingw build system
//
$ make mingw
Key Build Options:
- ==**-DLUA_BUILD_AS_DLL**==
- ==**-DLUA_BUILD_AS_DLL**==
- ==**gcc -std=gnu99 -shared -o lua53.dll**==
- ==**strip --strip-unneeded lua53.dll**==
- ==**gcc -std=gnu99 -o lua.exe -s  lua.o lua53.dll -lm**==


```bash
Edison@Edison-i7 UCRT64 /e/work/doc/txt/lua-5.3.5
------------------------------------------------------------------------------------------------------------------------------------------------------
$ make mingw
cd src && make mingw
make[1]: Entering directory '/e/work/doc/txt/lua-5.3.5/src'
make "LUA_A=lua53.dll" "LUA_T=lua.exe" \
"AR=gcc -std=gnu99 -shared -o" "RANLIB=strip --strip-unneeded" \
"SYSCFLAGS=-DLUA_BUILD_AS_DLL" "SYSLIBS=" "SYSLDFLAGS=-s" lua.exe
make[2]: Entering directory '/e/work/doc/txt/lua-5.3.5/src'
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lua.o lua.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lapi.o lapi.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lcode.o lcode.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lctype.o lctype.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o ldebug.o ldebug.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o ldo.o ldo.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o ldump.o ldump.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lfunc.o lfunc.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lgc.o lgc.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o llex.o llex.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lmem.o lmem.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lobject.o lobject.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lopcodes.o lopcodes.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lparser.o lparser.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lstate.o lstate.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lstring.o lstring.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o ltable.o ltable.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o ltm.o ltm.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lundump.o lundump.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lvm.o lvm.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lzio.o lzio.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lauxlib.o lauxlib.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lbaselib.o lbaselib.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lbitlib.o lbitlib.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lcorolib.o lcorolib.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o ldblib.o ldblib.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o liolib.o liolib.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lmathlib.o lmathlib.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o loslib.o loslib.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lstrlib.o lstrlib.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o ltablib.o ltablib.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o lutf8lib.o lutf8lib.c
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2   -DLUA_BUILD_AS_DLL    -c -o loadlib.o loadlib.c
loadlib.c: In function 'lsys_sym':
loadlib.c:209:21: warning: cast between incompatible function types from 'FARPROC' {aka 'long long int (*)()'} to 'int (*)(lua_State *)' [-Wcast-funct
ion-type]
  209 |   lua_CFunction f = (lua_CFunction)GetProcAddress((HMODULE)lib, sym);
      |                     ^
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2 -DLUA_BUILD_AS_DLL    -c -o linit.o linit.c


gcc -std=gnu99 -shared -o lua53.dll lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o ltm.o lundump.o lvm.o lzio.o lauxlib.o lbaselib.o lbitlib.o lcorolib.o ldblib.o liolib.o lmathlib.o loslib.o lstrlib.o ltablib.o lutf8lib.o loadlib.o linit.o

strip --strip-unneeded lua53.dll

gcc -std=gnu99 -o lua.exe -s  lua.o lua53.dll -lm
make[2]: Leaving directory '/e/work/doc/txt/lua-5.3.5/src'
make "LUAC_T=luac.exe" luac.exe
make[2]: Entering directory '/e/work/doc/txt/lua-5.3.5/src'
gcc -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2     -c -o luac.o luac.c
ar rcu liblua.a lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o ltm.o lundump.o lvm.o lzio.o lauxlib.o lbaselib.o lbitlib.o lcorolib.o ldblib.o liolib.o lmathlib.o loslib.o lstrlib.o ltablib.o lutf8lib.o loadlib.o linit.o
D:\Soft\msys64\ucrt64\bin\ar.exe: `u' modifier ignored since `D' is the default (see `U')
ranlib liblua.a
gcc -std=gnu99 -o luac.exe   luac.o liblua.a -lm
make[2]: Leaving directory '/e/work/doc/txt/lua-5.3.5/src'
make[1]: Leaving directory '/e/work/doc/txt/lua-5.3.5/src'

Edison@Edison-i7 UCRT64 /e/work/doc/txt/lua-5.3.5
```


