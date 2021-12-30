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


static const luaL_Reg lualibs[] = {
  {"", luaopen_base},
  /* "package"   LUA_LOADLIBNAME    */   
  {LUA_LOADLIBNAME, luaopen_package},  	
  /* "table"     LUA_TABLIBNAME     */   
  {LUA_TABLIBNAME, luaopen_table},      
  /* "io"        LUA_IOLIBNAME      */
  {LUA_IOLIBNAME, luaopen_io},          
  /* "os"        LUA_OSLIBNAME      */
  {LUA_OSLIBNAME, luaopen_os},          
  /* "string"    LUA_STRLIBNAME	    */
  {LUA_STRLIBNAME, luaopen_string},     
  /* "math"      LUA_MATHLIBNAME    */
  {LUA_MATHLIBNAME, luaopen_math},
  /* "debug "    LUA_DBLIBNAME	    */
  {LUA_DBLIBNAME, luaopen_debug},
  {NULL, NULL}
};


LUALIB_API void luaL_openlibs (lua_State *L) {
  const luaL_Reg *lib = lualibs;
  for (; lib->func; lib++) {
    lua_pushcfunction(L, lib->func);
    lua_pushstring(L, lib->name);
    lua_call(L, 1, 0);
  }
}

