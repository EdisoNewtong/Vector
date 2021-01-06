//////////////////////////////////////////////////
// Step by Step, to Build a lua test project
//////////////////////////////////////////////////

1. copy the 4 header files from lua source code directory to this directory
    1. luaconf.h
    
    2. lua.h        // inside lua.h , you will find a preprocessor header file included      :  #include "luaconf.h"
    3. lauxlib.h
    4. lualib.h

2. build the origin source code to generate a library file named     liblua.a (on Linux or Mac)     liblua.lib (on Windows)

3. include the 3 header file like the following code

    extern "C" {
        #include "lua.h"
        #include "lualib.h"
        #include "lauxlib.h"
    }


4.  Link your program with the library file generated in Step2

5. run the lua script code by the following command line
    $ ./main script/test1.lua
