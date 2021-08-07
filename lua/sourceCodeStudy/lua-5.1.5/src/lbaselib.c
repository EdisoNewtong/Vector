/*
** $Id: lbaselib.c,v 1.191.1.6 2008/02/14 16:46:22 roberto Exp $
** Basic library
** See Copyright Notice in lua.h
*/



#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define lbaselib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"




/*
** If your system does not support `stdout', you can just remove this function.
** If you need, you can define your own `print' function, following this
** model but changing `fputs' to put the strings at a proper place
** (a console window or a log file, for instance).
*/
static int luaB_print (lua_State *L) {
  int n = lua_gettop(L); /* number of arguments */
  int i;
  lua_getfield(L, (-10002), ("tostring"));
  for (i=1; i<=n; i++) {
    const char *s;
    lua_pushvalue(L, -1); /* function to be called */
    lua_pushvalue(L, i); /* value to print */
    lua_call(L, 1, 1);
    s = lua_tolstring(L, (-1), ((void *)0)); /* get result */
    if (s == ((void *)0)) {
      return luaL_error(L, "'" "tostring" "'" " must return a string to "
                           "'" "print" "'");
    }
    if (i>1) {
      fputs("\t", stdout);
    }
    fputs(s, stdout);
    lua_settop(L, -(1)-1); /* pop result */
  }
  fputs("\n", stdout);
  return 0;
}


static int luaB_tonumber (lua_State *L) {
  int base = ((int)luaL_optinteger(L, (2), (10)));
  if (base == 10) { /* standard conversion */
    luaL_checkany(L, 1);
    if (lua_isnumber(L, 1)) {
      lua_pushnumber(L, lua_tonumber(L, 1));
      return 1;
    }
  }
  else {
    const char *s1 = (luaL_checklstring(L, (1), ((void *)0)));
    char *s2;
    unsigned long n;
    ((void)((2 <= base && base <= 36) || luaL_argerror(L, (2), ("base out of range"))));
    n = strtoul(s1, &s2, base);
    if (s1 != s2) { /* at least one valid digit? */
      while (((*__ctype_b_loc ())[(int) (((unsigned char)(*s2)))] & (unsigned short int) _ISspace)) {
        s2++; /* skip trailing spaces */
      }
      if (*s2 == '\0') { /* no invalid trailing characters? */
        lua_pushnumber(L, (lua_Number)n);
        return 1;
      }
    }
  }
  lua_pushnil(L); /* else not a number */
  return 1;
}


static int luaB_error (lua_State *L) {
  int level = ((int)luaL_optinteger(L, (2), (1)));
  lua_settop(L, 1);
  if (lua_isstring(L, 1) && level > 0) { /* add extra information? */
    luaL_where(L, level);
    lua_pushvalue(L, 1);
    lua_concat(L, 2);
  }
  return lua_error(L);
}


static int luaB_getmetatable (lua_State *L) {
  luaL_checkany(L, 1);
  if (!lua_getmetatable(L, 1)) {
    lua_pushnil(L);
    return 1; /* no metatable */
  }
  luaL_getmetafield(L, 1, "__metatable");
  return 1; /* returns either __metatable field (if present) or metatable */
}


static int luaB_setmetatable (lua_State *L) {
  int t = lua_type(L, 2);
  luaL_checktype(L, 1, 5);
  ((void)((t == 0 || t == 5) || luaL_argerror(L, (2), ("nil or table expected"))));
  if (luaL_getmetafield(L, 1, "__metatable")) {
    luaL_error(L, "cannot change a protected metatable");
  }
  lua_settop(L, 2);
  lua_setmetatable(L, 1);
  return 1;
}


static void getfunc (lua_State *L, int opt) {
  if ((lua_type(L, (1)) == 6)) {
    lua_pushvalue(L, 1);
  }
  else {
    lua_Debug ar;
    int level = opt ? ((int)luaL_optinteger(L, (1), (1))) : ((int)luaL_checkinteger(L, (1)));
    ((void)((level >= 0) || luaL_argerror(L, (1), ("level must be non-negative"))));
    if (lua_getstack(L, level, &ar) == 0) {
      luaL_argerror(L, 1, "invalid level");
    }
    lua_getinfo(L, "f", &ar);
    if ((lua_type(L, (-1)) == 0)) {
      luaL_error(L, "no function environment for tail call at level %d",
                    level);
    }
  }
}


static int luaB_getfenv (lua_State *L) {
  getfunc(L, 1);
  if (lua_iscfunction(L, -1)) { /* is a C function? */
    lua_pushvalue(L, (-10002)); /* return the thread's global env. */
  } else {
    lua_getfenv(L, -1);
  }
  return 1;
}


static int luaB_setfenv (lua_State *L) {
  luaL_checktype(L, 2, 5);
  getfunc(L, 0);
  lua_pushvalue(L, 2);
  if (lua_isnumber(L, 1) && lua_tonumber(L, 1) == 0) {
    /* change environment of current thread */
    lua_pushthread(L);
    lua_insert(L, -2);
    lua_setfenv(L, -2);
    return 0;
  }
  else if (lua_iscfunction(L, -2) || lua_setfenv(L, -2) == 0) {
    luaL_error(L,
          "'" "setfenv" "'" " cannot change environment of given object");
  }
  return 1;
}


static int luaB_rawequal (lua_State *L) {
  luaL_checkany(L, 1);
  luaL_checkany(L, 2);
  lua_pushboolean(L, lua_rawequal(L, 1, 2));
  return 1;
}


static int luaB_rawget (lua_State *L) {
  luaL_checktype(L, 1, 5);
  luaL_checkany(L, 2);
  lua_settop(L, 2);
  lua_rawget(L, 1);
  return 1;
}


static int luaB_rawset (lua_State *L) {
  luaL_checktype(L, 1, 5);
  luaL_checkany(L, 2);
  luaL_checkany(L, 3);
  lua_settop(L, 3);
  lua_rawset(L, 1);
  return 1;
}


static int luaB_gcinfo (lua_State *L) {
  lua_pushinteger(L, lua_gc(L, 3, 0));
  return 1;
}


static int luaB_collectgarbage (lua_State *L) {
  static const char *const opts[] = {
    "stop",
    "restart",
    "collect",
    "count",
    "step",
    "setpause",
    "setstepmul",
    ((void *)0)
  };
  static const int optsnum[] = {
    0,
    1,
    2,
    3,
    5,
    6,
    7
  };
  int o = luaL_checkoption(L, 1, "collect", opts);
  int ex = ((int)luaL_optinteger(L, (2), (0)));
  int res = lua_gc(L, optsnum[o], ex);
  switch (optsnum[o]) {
    case 3: {
      int b = lua_gc(L, 4, 0);
      lua_pushnumber(L, res + ((lua_Number)b/1024));
      return 1;
    }
    case 5: {
      lua_pushboolean(L, res);
      return 1;
    }
    default: {
      lua_pushnumber(L, res);
      return 1;
    }
  }
}


static int luaB_type (lua_State *L) {
  luaL_checkany(L, 1);
  lua_pushstring(L, lua_typename(L, lua_type(L,(1))));
  return 1;
}


static int luaB_next (lua_State *L) {
  luaL_checktype(L, 1, 5);
  lua_settop(L, 2); /* create a 2nd argument if there isn't one */
  if (lua_next(L, 1)) {
    return 2;
  } else {
    lua_pushnil(L);
    return 1;
  }
}


static int luaB_pairs (lua_State *L) {
  luaL_checktype(L, 1, 5);
  lua_pushvalue(L, ((-10002)-(1))); /* return generator, */
  lua_pushvalue(L, 1); /* state, */
  lua_pushnil(L); /* and initial value */
  return 3;
}


static int ipairsaux (lua_State *L) {
  int i = ((int)luaL_checkinteger(L, (2)));
  luaL_checktype(L, 1, 5);
  i++; /* next value */
  lua_pushinteger(L, i);
  lua_rawgeti(L, 1, i);
  return ((lua_type(L, (-1)) == 0)) ? 0 : 2;
}


static int luaB_ipairs (lua_State *L) {
  luaL_checktype(L, 1, 5);
  lua_pushvalue(L, ((-10002)-(1))); /* return generator, */
  lua_pushvalue(L, 1); /* state, */
  lua_pushinteger(L, 0); /* and initial value */
  return 3;
}


static int load_aux (lua_State *L, int status) {
  if (status == 0) { /* OK? */
    return 1;
  } else {
    lua_pushnil(L);
    lua_insert(L, -2); /* put before error message */
    return 2; /* return nil plus error message */
  }
}


static int luaB_loadstring (lua_State *L) {
  size_t l;
  const char *s = luaL_checklstring(L, 1, &l);
  const char *chunkname = (luaL_optlstring(L, (2), (s), ((void *)0)));
  return load_aux(L, luaL_loadbuffer(L, s, l, chunkname));
}


static int luaB_loadfile (lua_State *L) {
  const char *fname = (luaL_optlstring(L, (1), (((void *)0)), ((void *)0)));
  return load_aux(L, luaL_loadfile(L, fname));
}


/*
** Reader for generic `load' function: `lua_load' uses the
** stack for internal stuff, so the reader cannot change the
** stack top. Instead, it keeps its resulting string in a
** reserved slot inside the stack.
*/
static const char *generic_reader (lua_State *L, void *ud, size_t *size) {
  (void)ud; /* to avoid warnings */
  luaL_checkstack(L, 2, "too many nested functions");
  lua_pushvalue(L, 1); /* get function */
  lua_call(L, 0, 1); /* call it */
  if ((lua_type(L, (-1)) == 0)) {
    *size = 0;
    return ((void *)0);
  }
  else if (lua_isstring(L, -1)) {
    lua_replace(L, 3); /* save string in a reserved stack slot */
    return lua_tolstring(L, 3, size);
  }
  else {
    luaL_error(L, "reader function must return a string");
  }
  return ((void *)0); /* to avoid warnings */
}


static int luaB_load (lua_State *L) {
  int status;
  const char *cname = (luaL_optlstring(L, (2), ("=(load)"), ((void *)0)));
  luaL_checktype(L, 1, 6);
  lua_settop(L, 3); /* function, eventual name, plus one reserved slot */
  status = lua_load(L, generic_reader, ((void *)0), cname);
  return load_aux(L, status);
}


static int luaB_dofile (lua_State *L) {
  const char *fname = (luaL_optlstring(L, (1), (((void *)0)), ((void *)0)));
  int n = lua_gettop(L);
  if (luaL_loadfile(L, fname) != 0) {
    lua_error(L);
  }
  lua_call(L, 0, (-1));
  return lua_gettop(L) - n;
}


static int luaB_assert (lua_State *L) {
  luaL_checkany(L, 1);
  if (!lua_toboolean(L, 1)) {
    return luaL_error(L, "%s", (luaL_optlstring(L, (2), ("assertion failed!"), ((void *)0))));
  }
  return lua_gettop(L);
}


static int luaB_unpack (lua_State *L) {
  int i, e, n;
  luaL_checktype(L, 1, 5);
  i = ((int)luaL_optinteger(L, (2), (1)));
  e = ((lua_type(L, ((3))) <= 0) ? (((int)lua_objlen(L, 1))) : ((int)luaL_checkinteger(L, ((3)))));
  if (i > e) {
    return 0; /* empty range */
  }
  n = e - i + 1; /* number of elements */
  if (n <= 0 || !lua_checkstack(L, n)) { /* n <= 0 means arith. overflow */
    return luaL_error(L, "too many results to unpack");
  }
  lua_rawgeti(L, 1, i); /* push arg[i] (avoiding overflow problems) */
  while (i++ < e) { /* push arg[i + 1...e] */
    lua_rawgeti(L, 1, i);
  }
  return n;
}


static int luaB_select (lua_State *L) {
  int n = lua_gettop(L);
  if (lua_type(L, 1) == 4 && *lua_tolstring(L, (1), ((void *)0)) == '#') {
    lua_pushinteger(L, n-1);
    return 1;
  }
  else {
    int i = ((int)luaL_checkinteger(L, (1)));
    if (i < 0) {
      i = n + i;
    }
    else if (i > n) {
      i = n;
    }
    ((void)((1 <= i) || luaL_argerror(L, (1), ("index out of range"))));
    return n - i;
  }
}


static int luaB_pcall (lua_State *L) {
  int status;
  luaL_checkany(L, 1);
  status = lua_pcall(L, lua_gettop(L) - 1, (-1), 0);
  lua_pushboolean(L, (status == 0));
  lua_insert(L, 1);
  return lua_gettop(L); /* return status + all results */
}


static int luaB_xpcall (lua_State *L) {
  int status;
  luaL_checkany(L, 2);
  lua_settop(L, 2);
  lua_insert(L, 1); /* put error function under function to be called */
  status = lua_pcall(L, 0, (-1), 1);
  lua_pushboolean(L, (status == 0));
  lua_replace(L, 1);
  return lua_gettop(L); /* return status + all results */
}


static int luaB_tostring (lua_State *L) {
  luaL_checkany(L, 1);
  if (luaL_callmeta(L, 1, "__tostring")) { /* is there a metafield? */
    return 1; /* use its value */
  }
  switch (lua_type(L, 1)) {
    case 3: {
      lua_pushstring(L, lua_tolstring(L, (1), ((void *)0)));
      break;
    }
    case 4: {
      lua_pushvalue(L, 1);
      break;
    }
    case 1: {
      lua_pushstring(L, (lua_toboolean(L, 1) ? "true" : "false"));
      break;
    }
    case 0: {
      lua_pushlstring(L, "" "nil", (sizeof("nil")/sizeof(char))-1);
      break;
    }
    default: {
      lua_pushfstring(L, "%s: %p", lua_typename(L, lua_type(L,(1))), lua_topointer(L, 1));
      break;
    }
  }
  return 1;
}


static int luaB_newproxy (lua_State *L) {
  lua_settop(L, 1);
  lua_newuserdata(L, 0); /* create proxy */
  if (lua_toboolean(L, 1) == 0) {
    return 1; /* no metatable */
  }
  else if ((lua_type(L, (1)) == 1)) {
    lua_createtable(L, 0, 0); /* create a new metatable `m' ... */
    lua_pushvalue(L, -1); /* ... and mark `m' as a valid metatable */
    lua_pushboolean(L, 1);
    lua_rawset(L, ((-10002)-(1))); /* weaktable[m] = true */
  }
  else {
    int validproxy = 0; /* to check if weaktable[metatable(u)] == true */
    if (lua_getmetatable(L, 1)) {
      lua_rawget(L, ((-10002)-(1)));
      validproxy = lua_toboolean(L, -1);
      lua_settop(L, -(1)-1); /* remove value */
    }
    ((void)((validproxy) || luaL_argerror(L, (1), ("boolean or proxy expected"))));
    lua_getmetatable(L, 1); /* metatable is valid; get it */
  }
  lua_setmetatable(L, 2);
  return 1;
}


static const luaL_Reg base_funcs[] = {
  {"assert", luaB_assert},
  {"collectgarbage", luaB_collectgarbage},
  {"dofile", luaB_dofile},
  {"error", luaB_error},
  {"gcinfo", luaB_gcinfo},
  {"getfenv", luaB_getfenv},
  {"getmetatable", luaB_getmetatable},
  {"loadfile", luaB_loadfile},
  {"load", luaB_load},
  {"loadstring", luaB_loadstring},
  {"next", luaB_next},
  {"pcall", luaB_pcall},
  {"print", luaB_print},
  {"rawequal", luaB_rawequal},
  {"rawget", luaB_rawget},
  {"rawset", luaB_rawset},
  {"select", luaB_select},
  {"setfenv", luaB_setfenv},
  {"setmetatable", luaB_setmetatable},
  {"tonumber", luaB_tonumber},
  {"tostring", luaB_tostring},
  {"type", luaB_type},
  {"unpack", luaB_unpack},
  {"xpcall", luaB_xpcall},
  {((void *)0), ((void *)0)}
};


/*
** {======================================================
** Coroutine library
** =======================================================
*/

#define CO_RUN	0	/* running */
#define CO_SUS	1	/* suspended */
#define CO_NOR	2	/* 'normal' (it resumed another coroutine) */
#define CO_DEAD	3

static const char *const statnames[] =
    {"running", "suspended", "normal", "dead"};

static int costatus (lua_State *L, lua_State *co) {
  if (L == co) return 0;
  switch (lua_status(co)) {
    case 1: {
      return 1;
    }
    case 0: {
      lua_Debug ar;
      if (lua_getstack(co, 0, &ar) > 0) { /* does it have frames? */
        return 2; /* it is running */
      }
      else if (lua_gettop(co) == 0) {
        return 3;
      }
      else {
        return 1; /* initial state */
      }
    }
    default: { /* some error occured */
      return 3;
    }
  }
}


static int luaB_costatus (lua_State *L) {
  lua_State *co = lua_tothread(L, 1);
  ((void)((co) || luaL_argerror(L, (1), ("coroutine expected"))));
  lua_pushstring(L, statnames[costatus(L, co)]);
  return 1;
}


static int auxresume (lua_State *L, lua_State *co, int narg) {
  int status = costatus(L, co);
  if (!lua_checkstack(co, narg)) {
    luaL_error(L, "too many arguments to resume");
  }
  if (status != 1) {
    lua_pushfstring(L, "cannot resume %s coroutine", statnames[status]);
    return -1; /* error flag */
  }
  lua_xmove(L, co, narg);
  lua_setlevel(L, co);
  status = lua_resume(co, narg);
  if (status == 0 || status == 1) {
    int nres = lua_gettop(co);
    if (!lua_checkstack(L, nres + 1)) {
      luaL_error(L, "too many results to resume");
    }
    lua_xmove(co, L, nres); /* move yielded values */
    return nres;
  }
  else {
    lua_xmove(co, L, 1); /* move error message */
    return -1; /* error flag */
  }
}


static int luaB_coresume (lua_State *L) {
  lua_State *co = lua_tothread(L, 1);
  int r;
  ((void)((co) || luaL_argerror(L, (1), ("coroutine expected"))));
  r = auxresume(L, co, lua_gettop(L) - 1);
  if (r < 0) {
    lua_pushboolean(L, 0);
    lua_insert(L, -2);
    return 2; /* return false + error message */
  }
  else {
    lua_pushboolean(L, 1);
    lua_insert(L, -(r + 1));
    return r + 1; /* return true + `resume' returns */
  }
}


static int luaB_auxwrap (lua_State *L) {
  lua_State *co = lua_tothread(L, ((-10002)-(1)));
  int r = auxresume(L, co, lua_gettop(L));
  if (r < 0) {
    if (lua_isstring(L, -1)) { /* error object is a string? */
      luaL_where(L, 1); /* add extra info */
      lua_insert(L, -2);
      lua_concat(L, 2);
    }
    lua_error(L); /* propagate error */
  }
  return r;
}


static int luaB_cocreate (lua_State *L) {
  lua_State *NL = lua_newthread(L);
  ((void)(((lua_type(L, (1)) == 6) && !lua_iscfunction(L, 1)) || luaL_argerror(L, (1), ("Lua function expected"))));
  lua_pushvalue(L, 1); /* move function to top */
  lua_xmove(L, NL, 1); /* move function from L to NL */
  return 1;
}


static int luaB_cowrap (lua_State *L) {
  luaB_cocreate(L);
  lua_pushcclosure(L, luaB_auxwrap, 1);
  return 1;
}


static int luaB_yield (lua_State *L) {
  return lua_yield(L, lua_gettop(L));
}


static int luaB_corunning (lua_State *L) {
  if (lua_pushthread(L)) {
    lua_pushnil(L); /* main thread is not a coroutine */
  }
  return 1;
}


static const luaL_Reg co_funcs[] = {
  {"create", luaB_cocreate},
  {"resume", luaB_coresume},
  {"running", luaB_corunning},
  {"status", luaB_costatus},
  {"wrap", luaB_cowrap},
  {"yield", luaB_yield},
  {((void *)0), ((void *)0)}
};


/* }====================================================== */


static void auxopen (lua_State *L, const char *name,
                     lua_CFunction f, lua_CFunction u) {
  lua_pushcclosure(L, (u), 0);
  lua_pushcclosure(L, f, 1);
  lua_setfield(L, -2, name);
}


static void base_open (lua_State *L) {
  /* set global _G */
  lua_pushvalue(L, (-10002));
  lua_setfield(L, (-10002), ("_G"));
  /* open lib into global table */
  luaL_register(L, "_G", base_funcs);
  lua_pushlstring(L, "" "Lua 5.1", (sizeof("Lua 5.1")/sizeof(char))-1);
  lua_setfield(L, (-10002), ("_VERSION")); /* set global _VERSION */
  /* `ipairs' and `pairs' need auxiliary functions as upvalues */
  auxopen(L, "ipairs", luaB_ipairs, ipairsaux);
  auxopen(L, "pairs", luaB_pairs, luaB_next);
  /* `newproxy' needs a weaktable as upvalue */
  lua_createtable(L, 0, 1); /* new table `w' */
  lua_pushvalue(L, -1); /* `w' will be its own metatable */
  lua_setmetatable(L, -2);
  lua_pushlstring(L, "" "kv", (sizeof("kv")/sizeof(char))-1);
  lua_setfield(L, -2, "__mode"); /* metatable(w).__mode = "kv" */
  lua_pushcclosure(L, luaB_newproxy, 1);
  lua_setfield(L, (-10002), ("newproxy")); /* set global `newproxy' */
}


extern int luaopen_base (lua_State *L) {
  base_open(L);
  luaL_register(L, "coroutine", co_funcs);
  return 2;
}

