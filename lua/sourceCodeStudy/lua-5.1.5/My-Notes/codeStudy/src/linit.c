/*
** $Id: linit.c,v 1.14.1.1 2007/12/27 13:02:25 roberto Exp $
** Initialization of libraries for lua.c
** See Copyright Notice in lua.h
*/


#define linit_c
#define LUA_LIB

#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"


/* lauxlib.h:32

typedef struct luaL_Reg {
  const char *name;
  lua_CFunction func;
} luaL_Reg;

*/


static const luaL_Reg lualibs[] = {
  /*   luaopen_base              =>   lbaselib.c:780 */
  {"", luaopen_base},
  /* "package"   LUA_LOADLIBNAME => loadlib.c:760   */   
  {LUA_LOADLIBNAME, luaopen_package},  	
  /* "table"     LUA_TABLIBNAME  => ltablic.c:343   */   
  {LUA_TABLIBNAME, luaopen_table},      
  /* "io"        LUA_IOLIBNAME   => liolib.c:633    */
  {LUA_IOLIBNAME, luaopen_io},          
  /* "os"        LUA_OSLIBNAME   => loslib.c:291    */
  {LUA_OSLIBNAME, luaopen_os},          
  /* "string"    LUA_STRLIBNAME	 => lstrlib.c:1168  */
  {LUA_STRLIBNAME, luaopen_string},     
  /* "math"      LUA_MATHLIBNAME => lmathlib.c:284  */
  {LUA_MATHLIBNAME, luaopen_math},
  /* "debug "    LUA_DBLIBNAME	 => ldblib.c:471    */
  {LUA_DBLIBNAME, luaopen_debug},
  {NULL, NULL}
};


LUALIB_API void luaL_openlibs (lua_State *L) {
  const luaL_Reg *lib = lualibs;
  for (; lib->func; lib++) {
    // step-1: push  function
    lua_pushcfunction(L, lib->func);
    // step-2: push  argument
    lua_pushstring(L, lib->name);

    // 1: 1 argument 
    // 0: 0 result 
    lua_call(L, 1, 0);
  }
}

