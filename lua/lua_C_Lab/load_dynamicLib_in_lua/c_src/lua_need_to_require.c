#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


static int c_language_divide(lua_State* L)
{
    int n = lua_gettop(L);
    if ( n != 2 ) {
        return luaL_error(L, "C Language divide operator require 2 operands. ");
    }

    /* c = a / b  */
    int a = luaL_checkint(L, 1);  /* a is the   Dividend */
    int b = luaL_checkint(L, 2);  /* b is the   Divisor  */
    int c = a / b;                /* c is the   Quotient */

    /* push the result c into the lua stack as the result */
    lua_pushinteger(L, c);
    return 1;    // c is the only 1 result of the return value of function "c_language_divide"
}

static const luaL_Reg    my_exported_functionTable[] = {
  { "dylib_divide",  c_language_divide },
  { NULL, NULL }
};





/***************************************************

    There should be only 1 exported dynamic library header function

"Whose name must has the "luaopen_" prefix , otherwise the lua loader will failed at finding the specified dynamic-library header

Also see the beginning part of the source code "loadlib.c"


// prefix for open functions in C libraries 
#define LUA_POF		"luaopen_"
#define POF		    LUA_POF


// In MacOS  the exported prefix should be    "_luaopen_"
// Mac appends a `_' before C function names 
#define POF	"_" LUA_POF


--------------------------------------------------
Prefix Summary:

        Windows / Linux     "luaopen_"
                  MacOS     "_luaopen_"

--------------------------------------------------


outputDynamicLib/CDivide.so   => outputDynamicLib_CDivide

****************************************************/
int luaopen_outputDynamicLib_CDivide(lua_State* L)
{
    luaL_register(L, "C_LanguageDyLib", my_exported_functionTable);
    return 1;
}


