/*
** $Id: loadlib.c,v 1.52.1.4 2009/09/09 13:17:16 roberto Exp $
** Dynamic library loader for Lua
** See Copyright Notice in lua.h
**
** This module contains an implementation of loadlib for Unix systems
** that have dlfcn, an implementation for Darwin (Mac OS X), an
** implementation for Windows, and a stub for other systems.
*/


#include <stdlib.h>
#include <string.h>


#define loadlib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


/* prefix for open functions in C libraries */
#define LUA_POF		"luaopen_"

/* separator for open functions in C libraries */
#define LUA_OFSEP	"_"


#define LIBPREFIX	"LOADLIB: "

#define POF		LUA_POF
#define LIB_FAIL	"open"


/* error codes for ll_loadfunc */
#define ERRLIB		1
#define ERRFUNC		2

#define setprogdir(L)		((void)0)


static void ll_unloadlib (void *lib);
static void *ll_load (lua_State *L, const char *path);
static lua_CFunction ll_sym (lua_State *L, void *lib, const char *sym);



#if defined(LUA_DL_DLOPEN)
/*
** {========================================================================
** This is an implementation of loadlib based on the dlfcn interface.
** The dlfcn interface is available in Linux, SunOS, Solaris, IRIX, FreeBSD,
** NetBSD, AIX 4.2, HPUX 11, and  probably most other Unix flavors, at least
** as an emulation layer on top of native functions.
** =========================================================================
*/

#include <dlfcn.h>

static void ll_unloadlib (void *lib) {
  dlclose(lib);
}


static void *ll_load (lua_State *L, const char *path) {
  void *lib = dlopen(path, 0x00002);
  if (lib == ((void *)0)) {
    lua_pushstring(L, dlerror());
  }
  return lib;
}


static lua_CFunction ll_sym (lua_State *L, void *lib, const char *sym) {
  lua_CFunction f = (lua_CFunction)dlsym(lib, sym);
  if (f == ((void *)0)) {
    lua_pushstring(L, dlerror());
  }
  return f;
}

#endif
/* }====================================================== */


static void **ll_register (lua_State *L, const char *path) {
  void **plib;
  lua_pushfstring(L, "%s%s", "LOADLIB: ", path);
  lua_gettable(L, (-10000)); /* check library in registry? */
  if (!(lua_type(L, (-1)) == 0)) { /* is there an entry? */
    plib = (void **)lua_touserdata(L, -1);
  } else { /* no entry yet; create one */
    lua_settop(L, -(1)-1);
    plib = (void **)lua_newuserdata(L, sizeof(const void *));
    *plib = ((void *)0);
    (lua_getfield(L, (-10000), ("_LOADLIB")));
    lua_setmetatable(L, -2);
    lua_pushfstring(L, "%s%s", "LOADLIB: ", path);
    lua_pushvalue(L, -2);
    lua_settable(L, (-10000));
  }
  return plib;
}


/*
** __gc tag method: calls library's `ll_unloadlib' function with the lib
** handle
*/
static int gctm (lua_State *L) {
  void **lib = (void **)luaL_checkudata(L, 1, "_LOADLIB");
  if (*lib) {
    ll_unloadlib(*lib);
  }
  *lib = ((void *)0); /* mark library as closed */
  return 0;
}


static int ll_loadfunc (lua_State *L, const char *path, const char *sym) {
  void **reg = ll_register(L, path);
  if (*reg == ((void *)0)) {
    *reg = ll_load(L, path);
  }
  if (*reg == ((void *)0)) {
    return 1; /* unable to load library */
  } else {
    lua_CFunction f = ll_sym(L, *reg, sym);
    if (f == ((void *)0)) {
      return 2; /* unable to find function */
    }
    lua_pushcclosure(L, (f), 0);
    return 0; /* return function */
  }
}


static int ll_loadlib (lua_State *L) {
  const char *path = (luaL_checklstring(L, (1), ((void *)0)));
  const char *init = (luaL_checklstring(L, (2), ((void *)0)));
  int stat = ll_loadfunc(L, path, init);
  if (stat == 0) { /* no errors? */
    return 1; /* return the loaded function */
  } else { /* error; error message is on stack top */
    lua_pushnil(L);
    lua_insert(L, -2);
    lua_pushstring(L, (stat == 1) ? "open" : "init");
    return 3; /* return nil, error message, and where */
  }
}


/*
** {======================================================
** 'require' function
** =======================================================
*/
static int readable (const char *filename) {
  FILE *f = fopen(filename, "r"); /* try to open file */
  if (f == ((void *)0)) {
    return 0; /* open failed */
  }
  fclose(f);
  return 1;
}


static const char *pushnexttemplate (lua_State *L, const char *path) {
  const char *l;
  while (*path == *";") {
      path++; /* skip separators */
  }
  if (*path == '\0') {
    return ((void *)0); /* no more templates */
  }
  l = strchr(path, *";"); /* find next separator */
  if (l == ((void *)0)) {
    l = path + strlen(path);
  }
  lua_pushlstring(L, path, l - path); /* template */
  return l;
}


static const char *findfile (lua_State *L, const char *name,
                                           const char *pname) {
  const char *path;
  name = luaL_gsub(L, name, ".", "/");
  lua_getfield(L, (-10001), pname);
  path = lua_tolstring(L, (-1), ((void *)0));
  if (path == ((void *)0)) {
    luaL_error(L, "'" "package.%s" "'" " must be a string", pname);
  }
  lua_pushlstring(L, "" "", (sizeof("")/sizeof(char))-1); /* error accumulator */
  while ((path = pushnexttemplate(L, path)) != ((void *)0)) {
    const char *filename;
    filename = luaL_gsub(L, lua_tolstring(L, (-1), ((void *)0)), "?", name);
    lua_remove(L, -2); /* remove path template */
    if (readable(filename)) { /* does file exist and is readable? */
      return filename; /* return that file name */
    }
    lua_pushfstring(L, "\n\tno file " "'" "%s" "'", filename);
    lua_remove(L, -2); /* remove file name */
    lua_concat(L, 2); /* add entry to possible error message */
  }
  return ((void *)0); /* not found */
}


static void loaderror (lua_State *L, const char *filename) {
  luaL_error(L, "error loading module " "'" "%s" "'" " from file " "'" "%s" "'" ":\n\t%s",
                lua_tolstring(L, (1), ((void *)0)), filename, lua_tolstring(L, (-1), ((void *)0)));
}


static int loader_Lua (lua_State *L) {
  const char *filename;
  const char *name = (luaL_checklstring(L, (1), ((void *)0)));
  filename = findfile(L, name, "path");
  if (filename == ((void *)0)) {
    return 1; /* library not found in this path */
  }
  if (luaL_loadfile(L, filename) != 0) {
    loaderror(L, filename);
  }
  return 1; /* library loaded successfully */
}


static const char *mkfuncname (lua_State *L, const char *modname) {
  const char *funcname;
  const char *mark = strchr(modname, *"-");
  if (mark) {
    modname = mark + 1;
  }
  funcname = luaL_gsub(L, modname, ".", "_");
  funcname = lua_pushfstring(L, "luaopen_""%s", funcname);
  lua_remove(L, -2); /* remove 'gsub' result */
  return funcname;
}


static int loader_C (lua_State *L) {
  const char *funcname;
  const char *name = (luaL_checklstring(L, (1), ((void *)0)));
  const char *filename = findfile(L, name, "cpath");
  if (filename == ((void *)0)) {
    return 1; /* library not found in this path */
  }
  funcname = mkfuncname(L, name);
  if (ll_loadfunc(L, filename, funcname) != 0) {
    loaderror(L, filename);
  }
  return 1; /* library loaded successfully */
}


static int loader_Croot (lua_State *L) {
  const char *funcname;
  const char *filename;
  const char *name = (luaL_checklstring(L, (1), ((void *)0)));
  const char *p = strchr(name, '.');
  int stat;
  if (p == ((void *)0)) {
    return 0; /* is root */
  }
  lua_pushlstring(L, name, p - name);
  filename = findfile(L, lua_tolstring(L, (-1), ((void *)0)), "cpath");
  if (filename == ((void *)0)) {
    return 1; /* root not found */
  }
  funcname = mkfuncname(L, name);
  if ((stat = ll_loadfunc(L, filename, funcname)) != 0) {
    if (stat != 2) {
      loaderror(L, filename); /* real error */
    }
    lua_pushfstring(L, "\n\tno module " "'" "%s" "'" " in file " "'" "%s" "'",
                       name, filename);
    return 1; /* function not found */
  }
  return 1;
}


static int loader_preload (lua_State *L) {
  const char *name = (luaL_checklstring(L, (1), ((void *)0)));
  lua_getfield(L, (-10001), "preload");
  if (!(lua_type(L, (-1)) == 5)) {
    luaL_error(L, "'" "package.preload" "'" " must be a table");
  }
  lua_getfield(L, -1, name);
  if ((lua_type(L, (-1)) == 0)) { /* not found? */
    lua_pushfstring(L, "\n\tno field package.preload['%s']", name);
  }
  return 1;
}


static const int sentinel_ = 0;
#define sentinel	((void *)&sentinel_)


static int ll_require (lua_State *L) {
  const char *name = (luaL_checklstring(L, (1), ((void *)0)));
  int i;
  lua_settop(L, 1); /* _LOADED table will be at index 2 */
  lua_getfield(L, (-10000), "_LOADED");
  lua_getfield(L, 2, name);
  if (lua_toboolean(L, -1)) { /* is it there? */
    if (lua_touserdata(L, -1) == ((void *)&sentinel_)) { /* check loops */
      luaL_error(L, "loop or previous error loading module " "'" "%s" "'", name);
    }
    return 1; /* package is already loaded */
  }
  /* else must load it; iterate over available loaders */
  lua_getfield(L, (-10001), "loaders");
  if (!(lua_type(L, (-1)) == 5)) {
    luaL_error(L, "'" "package.loaders" "'" " must be a table");
  }
  lua_pushlstring(L, "" "", (sizeof("")/sizeof(char))-1); /* error message accumulator */
  for (i=1; ; i++) {
    lua_rawgeti(L, -2, i); /* get a loader */
    if ((lua_type(L, (-1)) == 0)) {
      luaL_error(L, "module " "'" "%s" "'" " not found:%s",
                    name, lua_tolstring(L, (-2), ((void *)0)));
    }
    lua_pushstring(L, name);
    lua_call(L, 1, 1); /* call it */
    if ((lua_type(L, (-1)) == 6)) { /* did it find module? */
      break; /* module loaded successfully */
    } else if (lua_isstring(L, -1)) { /* loader returned error message? */
      lua_concat(L, 2); /* accumulate it */
    } else {
      lua_settop(L, -(1)-1);
    }
  }
  lua_pushlightuserdata(L, ((void *)&sentinel_));
  lua_setfield(L, 2, name); /* _LOADED[name] = sentinel */
  lua_pushstring(L, name); /* pass name as argument to module */
  lua_call(L, 1, 1); /* run loaded module */
  if (!(lua_type(L, (-1)) == 0)) { /* non-nil return? */
    lua_setfield(L, 2, name); /* _LOADED[name] = returned value */
  }
  lua_getfield(L, 2, name);
  if (lua_touserdata(L, -1) == ((void *)&sentinel_)) { /* module did not set a value? */
    lua_pushboolean(L, 1); /* use true as result */
    lua_pushvalue(L, -1); /* extra copy to be returned */
    lua_setfield(L, 2, name); /* _LOADED[name] = true */
  }
  return 1;
}



/* }====================================================== */



/*
** {======================================================
** 'module' function
** =======================================================
*/



static void setfenv (lua_State *L) {
  lua_Debug ar;
  if (lua_getstack(L, 1, &ar) == 0 ||
      lua_getinfo(L, "f", &ar) == 0 || /* get calling function */
      lua_iscfunction(L, -1))
  {
    luaL_error(L, "'" "module" "'" " not called from a Lua function");
  }
  lua_pushvalue(L, -2);
  lua_setfenv(L, -2);
  lua_settop(L, -(1)-1);
}


static void dooptions (lua_State *L, int n) {
  int i;
  for (i = 2; i <= n; i++) {
    lua_pushvalue(L, i); /* get option (a function) */
    lua_pushvalue(L, -2); /* module */
    lua_call(L, 1, 0);
  }
}


static void modinit (lua_State *L, const char *modname) {
  const char *dot;
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "_M"); /* module._M = module */
  lua_pushstring(L, modname);
  lua_setfield(L, -2, "_NAME");
  dot = strrchr(modname, '.'); /* look for last dot in module name */
  if (dot == ((void *)0)) {
    dot = modname;
  } else {
    dot++;
  }
  /* set _PACKAGE as package name (full module name minus last part) */
  lua_pushlstring(L, modname, dot - modname);
  lua_setfield(L, -2, "_PACKAGE");
}


static int ll_module (lua_State *L) {
  const char *modname = (luaL_checklstring(L, (1), ((void *)0)));
  int loaded = lua_gettop(L) + 1; /* index of _LOADED table */
  lua_getfield(L, (-10000), "_LOADED");
  lua_getfield(L, loaded, modname); /* get _LOADED[modname] */
  if (!(lua_type(L, (-1)) == 5)) { /* not found? */
    lua_settop(L, -(1)-1); /* remove previous result */
    /* try global variable (and create one if it does not exist) */
    if (luaL_findtable(L, (-10002), modname, 1) != ((void *)0)) {
      return luaL_error(L, "name conflict for module " "'" "%s" "'", modname);
    }
    lua_pushvalue(L, -1);
    lua_setfield(L, loaded, modname); /* _LOADED[modname] = new table */
  }
  /* check whether table already has a _NAME field */
  lua_getfield(L, -1, "_NAME");
  if (!(lua_type(L, (-1)) == 0)) { /* is table an initialized module? */
    lua_settop(L, -(1)-1);
  } else { /* no; initialize it */
    lua_settop(L, -(1)-1);
    modinit(L, modname);
  }
  lua_pushvalue(L, -1);
  setfenv(L);
  dooptions(L, loaded - 1);
  return 0;
}


static int ll_seeall (lua_State *L) {
  luaL_checktype(L, 1, 5);
  if (!lua_getmetatable(L, 1)) {
    lua_createtable(L, 0, 1); /* create new metatable */
    lua_pushvalue(L, -1);
    lua_setmetatable(L, 1);
  }
  lua_pushvalue(L, (-10002));
  lua_setfield(L, -2, "__index"); /* mt.__index = _G */
  return 0;
}


/* }====================================================== */



/* auxiliary mark (for internal use) */
#define AUXMARK		"\1"

static void setpath (lua_State *L, const char *fieldname, const char *envname,
                                   const char *def) {
  const char *path = getenv(envname);
  if (path == ((void *)0)) { /* no environment variable? */
    lua_pushstring(L, def); /* use default */
  } else {
    /* replace ";;" by ";AUXMARK;" and then AUXMARK by default path */
    path = luaL_gsub(L, path, ";" ";",
                              ";" "\1" ";");
    luaL_gsub(L, path, "\1", def);
    lua_remove(L, -2);
  }
  ((void)0);
  lua_setfield(L, -2, fieldname);
}


static const luaL_Reg pk_funcs[] = {
  {"loadlib", ll_loadlib},
  {"seeall", ll_seeall},
  {((void *)0), ((void *)0)}
};


static const luaL_Reg ll_funcs[] = {
  {"module", ll_module},
  {"require", ll_require},
  {((void *)0), ((void *)0)}
};


static const lua_CFunction loaders[] =
  {loader_preload, loader_Lua, loader_C, loader_Croot, ((void *)0)};


extern int luaopen_package (lua_State *L) {
  int i;
  /* create new type _LOADLIB */
  luaL_newmetatable(L, "_LOADLIB");
  lua_pushcclosure(L, (gctm), 0);
  lua_setfield(L, -2, "__gc");
  /* create `package' table */
  luaL_register(L, "package", pk_funcs);
  lua_pushvalue(L, -1);
  lua_replace(L, (-10001));
  /* create `loaders' table */
  lua_createtable(L, sizeof(loaders)/sizeof(loaders[0]) - 1, 0);
  /* fill it with pre-defined loaders */
  for (i=0; loaders[i] != ((void *)0); i++) {
    lua_pushcclosure(L, (loaders[i]), 0);
    lua_rawseti(L, -2, i+1);
  }
  lua_setfield(L, -2, "loaders"); /* put it in field `loaders' */
  setpath(L, "path", "LUA_PATH", "./?.lua;" "/usr/local/" "share/lua/5.1/""?.lua;" "/usr/local/" "share/lua/5.1/""?/init.lua;" "/usr/local/" "lib/lua/5.1/""?.lua;" "/usr/local/" "lib/lua/5.1/""?/init.lua"); /* set field `path' */
  setpath(L, "cpath", "LUA_CPATH", "./?.so;" "/usr/local/" "lib/lua/5.1/""?.so;" "/usr/local/" "lib/lua/5.1/""loadall.so"); /* set field `cpath' */
  /* store config information */
  lua_pushlstring(L, "" "/" "\n" ";" "\n" "?" "\n" "!" "\n" "-", (sizeof("/" "\n" ";" "\n" "?" "\n" "!" "\n" "-")/sizeof(char))-1);
  lua_setfield(L, -2, "config");
  /* set field `loaded' */
  luaL_findtable(L, (-10000), "_LOADED", 2);
  lua_setfield(L, -2, "loaded");
  /* set field `preload' */
  lua_createtable(L, 0, 0);
  lua_setfield(L, -2, "preload");
  lua_pushvalue(L, (-10002));
  luaL_register(L, ((void *)0), ll_funcs); /* open lib into global table */
  lua_settop(L, -(1)-1);
  return 1; /* return 'package' table */
}

