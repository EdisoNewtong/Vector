tolua 5.2.4 with   source code lua 5.1.5


--------------------------------------------------
tolua 5.2.4 Dir Struct
--------------------------------------------------

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Header file requirement
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


1. luaconf.h // lua.h need    will include   "luaconf.h"
2. lualib.h
3. lua.h
4. lauxlib.h

5. tolua.h


####################################################################################################
####################################################################################################
####################################################################################################

tolua-5.2.4/src/lib/
--tolua_event.c
--tolua_event.h
--tolua_is.c
--tolua_map.c
--tolua_push.c
--tolua_to.c

=> build 1 library output 
1. "lib/libtolua.a"



####################################################################################################
####################################################################################################
####################################################################################################

dependency    
1. libtolua.a   ( has already build by previous lib)
2. liblua.a     ( need lua source code , I use lua 5.1.5 )
3. libm.a       ( system library )


tolua-5.2.4/src/bin/
--tolua.c
--toluabind.c

=> build 2 binary files
1. "bin/tolua"  
2. "bin/tolua_lua"



####################################################################################################
How to Build with lua-5.1.5 ( if lua-5.3.5 , there will be no error after Step 1. )
####################################################################################################
Core Step

1. make a dir named "build_essential" under dir "tolua-5.2.4"
    with sub dir "include"
    with sub dir "lib"

    => build_essential/include
    => build_essential/lib

2. copy 4 header files  under  lua-5.1.5/src/    into     build_essential/include
    1. luaconf.h // lua.h need    will include   "luaconf.h"
    2. lualib.h
    3. lua.h
    4. lauxlib.h


3.  modified the content of   file  named    "tolua-5.2.4/config"
comment 1 line 
    # LUA=/usr/local

add a line
    LUA=../../build_essential

comment 1 line 
    # CPPFLAGS=  -g $(WARN) $(INC)

4. then execute the following command
 $ make


        The following Error will be raised

        ../../bin/tolua_lua -o toluabind.c tolua.pkg

        ** tolua internal error: ../../bin/../src/bin/lua/compat.lua:190: attempt to call field 'unpack' (a nil value).


        stack traceback:
            ../../bin/../src/bin/lua/basic.lua:94: in function 'tolua_error'
            ../../bin/../src/bin/lua/all.lua:25: in main chunk
        gcc -g -ansi -Wall -g -I../../include -I../../build_essential/include    -c -o toluabind.o toluabind.c
        toluabind.c:236:81: error: use of undeclared identifier 'LUA_OK'
          if (luaL_loadbuffer(tolua_S,(char*)B,sizeof(B),"tolua: embedded Lua code") == LUA_OK)
                                                                                        ^

        => 'LUA_OK' is an undeclared identifier

5. modified the  build_essential/include/lua.h
    add the following lines before line     "#define LUA_YIELD	1"
        /*
           Added by Edison
        --------------------------------------------------
           Copy this line from lua-5.3.5/lua.h here to make Compile successful
        --------------------------------------------------
        */
        #define LUA_OK		0
        #define LUA_YIELD	1


6. then execute the following command
 $ make

        The following Error will be raised

     ../../bin/tolua_lua -o toluabind.c tolua.pkg

    ** tolua internal error: ../../bin/../src/bin/lua/compat.lua:190: attempt to call field 'unpack' (a nil value).


    stack traceback:
        ../../bin/../src/bin/lua/basic.lua:94: in function 'tolua_error'
        ../../bin/../src/bin/lua/all.lua:25: in main chunk



    Because under lua-5.1.5  unpack is a global function rather than wrapped under library "table"
    So modified the lua script    src/bin/lua/compat.lua

    changed  from  table.unpack(...)  to    unpack(...)


7. then execute the following command
 $ make

        The following Error will be raised

     ../../bin/tolua_lua -o toluabind.c tolua.pkg

    ** tolua internal error: ../../bin/../src/bin/lua/compat.lua:40: bad argument #1 to 'load' (function expected, got string).


    stack traceback:
        ../../bin/../src/bin/lua/basic.lua:94: in function 'tolua_error'
        ../../bin/../src/bin/lua/all.lua:25: in main chunk

    Because under lua-5.1.5  load(...) only support one arg whose type is a function
                  lua-5.3.5  load(...) can support one arg whose type is either function or a string

    So modified the lua script    src/bin/lua/compat.lua
            from  load(...)  to   loadstring(...) 
    like the following
        function dostring(s) return do_(load(s)) end
        function dostring(s) return do_(loadstring(s)) end


8. then execute the following command
 $ make
--------------------------------------------------
Everything is OK
--------------------------------------------------


####################################################################################################
Analysize the 2 Compile/Link
Part 1:
    cd src/lib; make all
    gcc -g -ansi -Wall -g -I../../include -I../../build_essential/include    -c -o tolua_event.o tolua_event.c
    gcc -g -ansi -Wall -g -I../../include -I../../build_essential/include    -c -o tolua_is.o tolua_is.c
    gcc -g -ansi -Wall -g -I../../include -I../../build_essential/include    -c -o tolua_map.o tolua_map.c
    gcc -g -ansi -Wall -g -I../../include -I../../build_essential/include    -c -o tolua_push.o tolua_push.c
    gcc -g -ansi -Wall -g -I../../include -I../../build_essential/include    -c -o tolua_to.o tolua_to.c
    ar rcu ../../lib/libtolua.a tolua_event.o tolua_is.o tolua_map.o tolua_push.o tolua_to.o
    ranlib ../../lib/libtolua.a

--------------------------------------------------
    tolua_event.c => tolua_event.o
    tolua_is.c    => tolua_is.o
    tolua_map.c   => tolua_map.o
    tolua_push.c  => tolua_push.o
    tolua_to.c    => tolua_to.o
    *.o   =>  libtolua.a
--------------------------------------------------


Part 2:
    cd src/bin; make all
    gcc -g -ansi -Wall -g -I../../include -I../../build_essential/include    -c -o tolua.o tolua.c
    gcc -c -DLUA_SOURCE -I../../include -I../../build_essential/include  -o tolua_lua.o tolua.c
    gcc -o ../../bin/tolua_lua tolua_lua.o -L../../lib -L../../build_essential/lib -ltolua -llua -lm
    ../../bin/tolua_lua -o toluabind.c tolua.pkg
    gcc -g -ansi -Wall -g -I../../include -I../../build_essential/include    -c -o toluabind.o toluabind.c
    gcc -o ../../bin/tolua tolua.o toluabind.o -L../../lib -L../../build_essential/lib -ltolua -llua -lm


--------------------------------------------------

    tolua.c => tolua.o      // with option -ansi
    tolua.c => tolua_lua.o  // with option -DLUA_SOURCE
    tolua_lua.o => tolua_lua // link with libtolua.a liblua.a libm   // -ltolua -llua -lm

execute the following command to update  the existed  source  code file    "toluabind.c"
    ../../bin/tolua_lua -o toluabind.c tolua.pkg // with config file tolua.pkg

    toluabind.c            => toluabind.o
    tolua.o toluabind.o    => tolua


binary files Detail
1. tolua     //  copy "toluabind.c" 's  code  into   C main , it can run without lua code
2. tolua_lua // import  src/bin/lua/all.lua  ...  use  doit.lua to do Real Logic

--------------------------------------------------

Notes :  toluabind.c    will be  <Updated> by   the following command , be careful about the content of  toluabind.c
$ tolua_lua -o toluabind.c   tolua.pkg


