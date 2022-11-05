/*
** $Id: ldblib.c,v 1.104.1.4 2009/08/04 18:50:18 roberto Exp $
** Interface from Lua to its debug API
** See Copyright Notice in lua.h
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ldblib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"



static int db_getregistry (lua_State *L) {
  lua_pushvalue(L, (-10000));
  return 1;
}


static int db_getmetatable (lua_State *L) {
  luaL_checkany(L, 1);
  if (!lua_getmetatable(L, 1)) {
    lua_pushnil(L); /* no metatable */
  }
  return 1;
}


static int db_setmetatable (lua_State *L) {
  int t = lua_type(L, 2);
  ((void)((t == 0 || t == 5) || luaL_argerror(L, (2), ("nil or table expected"))));
  lua_settop(L, 2);
  lua_pushboolean(L, lua_setmetatable(L, 1));
  return 1;
}


static int db_getfenv (lua_State *L) {
  luaL_checkany(L, 1);
  lua_getfenv(L, 1);
  return 1;
}


static int db_setfenv (lua_State *L) {
  luaL_checktype(L, 2, 5);
  lua_settop(L, 2);
  if (lua_setfenv(L, 1) == 0) {
    luaL_error(L, "'" "setfenv" "'"
                  " cannot change environment of given object");
  }
  return 1;
}


static void settabss (lua_State *L, const char *i, const char *v) {
  lua_pushstring(L, v);
  lua_setfield(L, -2, i);
}


static void settabsi (lua_State *L, const char *i, int v) {
  lua_pushinteger(L, v);
  lua_setfield(L, -2, i);
}


static lua_State *getthread (lua_State *L, int *arg) {
  if ((lua_type(L, (1)) == 8)) {
    *arg = 1;
    return lua_tothread(L, 1);
  }
  else {
    *arg = 0;
    return L;
  }
}


static void treatstackoption (lua_State *L, lua_State *L1, const char *fname) {
  if (L == L1) {
    lua_pushvalue(L, -2);
    lua_remove(L, -3);
  }
  else {
    lua_xmove(L1, L, 1);
  }
  lua_setfield(L, -2, fname);
}


static int db_getinfo (lua_State *L) {
  lua_Debug ar;
  int arg;
  lua_State *L1 = getthread(L, &arg);
  const char *options = (luaL_optlstring(L, (arg+2), ("flnSu"), ((void *)0)));
  if (lua_isnumber(L, arg+1)) {
    if (!lua_getstack(L1, (int)lua_tointeger(L, arg+1), &ar)) {
      lua_pushnil(L); /* level out of range */
      return 1;
    }
  }
  else if ((lua_type(L, (arg+1)) == 6)) {
    lua_pushfstring(L, ">%s", options);
    options = lua_tolstring(L, (-1), ((void *)0));
    lua_pushvalue(L, arg+1);
    lua_xmove(L, L1, 1);
  }
  else {
    return luaL_argerror(L, arg+1, "function or level expected");
  }
  if (!lua_getinfo(L1, options, &ar)) {
    return luaL_argerror(L, arg+2, "invalid option");
  }
  lua_createtable(L, 0, 2);
  if (strchr(options, 'S')) {
    settabss(L, "source", ar.source);
    settabss(L, "short_src", ar.short_src);
    settabsi(L, "linedefined", ar.linedefined);
    settabsi(L, "lastlinedefined", ar.lastlinedefined);
    settabss(L, "what", ar.what);
  }
  if (strchr(options, 'l')) {
    settabsi(L, "currentline", ar.currentline);
  }
  if (strchr(options, 'u')) {
    settabsi(L, "nups", ar.nups);
  }
  if (strchr(options, 'n')) {
    settabss(L, "name", ar.name);
    settabss(L, "namewhat", ar.namewhat);
  }
  if (strchr(options, 'L')) {
    treatstackoption(L, L1, "activelines");
  }
  if (strchr(options, 'f')) {
    treatstackoption(L, L1, "func");
  }
  return 1; /* return table */
}


static int db_getlocal (lua_State *L) {
  int arg;
  lua_State *L1 = getthread(L, &arg);
  lua_Debug ar;
  const char *name;
  if (!lua_getstack(L1, ((int)luaL_checkinteger(L, (arg+1))), &ar)) { /* out of range? */
    return luaL_argerror(L, arg+1, "level out of range");
  }
  name = lua_getlocal(L1, &ar, ((int)luaL_checkinteger(L, (arg+2))));
  if (name) {
    lua_xmove(L1, L, 1);
    lua_pushstring(L, name);
    lua_pushvalue(L, -2);
    return 2;
  }
  else {
    lua_pushnil(L);
    return 1;
  }
}


static int db_setlocal (lua_State *L) {
  int arg;
  lua_State *L1 = getthread(L, &arg);
  lua_Debug ar;
  if (!lua_getstack(L1, ((int)luaL_checkinteger(L, (arg+1))), &ar)) { /* out of range? */
    return luaL_argerror(L, arg+1, "level out of range");
  }
  luaL_checkany(L, arg+3);
  lua_settop(L, arg+3);
  lua_xmove(L, L1, 1);
  lua_pushstring(L, lua_setlocal(L1, &ar, ((int)luaL_checkinteger(L, (arg+2)))));
  return 1;
}


static int auxupvalue (lua_State *L, int get) {
  const char *name;
  int n = ((int)luaL_checkinteger(L, (2)));
  luaL_checktype(L, 1, 6);
  if (lua_iscfunction(L, 1)) {
    return 0; /* cannot touch C upvalues from Lua */
  }
  name = get ? lua_getupvalue(L, 1, n) : lua_setupvalue(L, 1, n);
  if (name == ((void *)0)) {
    return 0;
  }
  lua_pushstring(L, name);
  lua_insert(L, -(get+1));
  return get + 1;
}


static int db_getupvalue (lua_State *L) {
  return auxupvalue(L, 1);
}


static int db_setupvalue (lua_State *L) {
  luaL_checkany(L, 3);
  return auxupvalue(L, 0);
}


static const char KEY_HOOK = 'h';


static void hookf (lua_State *L, lua_Debug *ar) {
  static const char *const hooknames[] =
    {"call", "return", "line", "count", "tail return"};
  lua_pushlightuserdata(L, (void *)&KEY_HOOK);
  lua_rawget(L, (-10000));
  lua_pushlightuserdata(L, L);
  lua_rawget(L, -2);
  if ((lua_type(L, (-1)) == 6)) {
    lua_pushstring(L, hooknames[(int)ar->event]);
    if (ar->currentline >= 0) {
      lua_pushinteger(L, ar->currentline);
    }
    else {
      lua_pushnil(L);
    }
    ((void)0);
    lua_call(L, 2, 0);
  }
}


static int makemask (const char *smask, int count) {
  int mask = 0;
  if (strchr(smask, 'c')) {
    mask |= (1 << 0);
  }
  if (strchr(smask, 'r')) {
    mask |= (1 << 1);
  }
  if (strchr(smask, 'l')) {
    mask |= (1 << 2);
  }
  if (count > 0) {
    mask |= (1 << 3);
  }
  return mask;
}


static char *unmakemask (int mask, char *smask) {
  int i = 0;
  if (mask & (1 << 0)) {
    smask[i++] = 'c';
  }
  if (mask & (1 << 1)) {
    smask[i++] = 'r';
  }
  if (mask & (1 << 2)) {
    smask[i++] = 'l';
  }
  smask[i] = '\0';
  return smask;
}


static void gethooktable (lua_State *L) {
  lua_pushlightuserdata(L, (void *)&KEY_HOOK);
  lua_rawget(L, (-10000));
  if (!(lua_type(L, (-1)) == 5)) {
    lua_settop(L, -(1)-1);
    lua_createtable(L, 0, 1);
    lua_pushlightuserdata(L, (void *)&KEY_HOOK);
    lua_pushvalue(L, -2);
    lua_rawset(L, (-10000));
  }
}


static int db_sethook (lua_State *L) {
  int arg, mask, count;
  lua_Hook func;
  lua_State *L1 = getthread(L, &arg);
  if ((lua_type(L, (arg+1)) <= 0)) {
    lua_settop(L, arg+1);
    func = ((void *)0); mask = 0; count = 0; /* turn off hooks */
  }
  else {
    const char *smask = (luaL_checklstring(L, (arg+2), ((void *)0)));
    luaL_checktype(L, arg+1, 6);
    count = ((int)luaL_optinteger(L, (arg+3), (0)));
    func = hookf; mask = makemask(smask, count);
  }
  gethooktable(L);
  lua_pushlightuserdata(L, L1);
  lua_pushvalue(L, arg+1);
  lua_rawset(L, -3); /* set new hook */
  lua_settop(L, -(1)-1); /* remove hook table */
  lua_sethook(L1, func, mask, count); /* set hooks */
  return 0;
}


static int db_gethook (lua_State *L) {
  int arg;
  lua_State *L1 = getthread(L, &arg);
  char buff[5];
  int mask = lua_gethookmask(L1);
  lua_Hook hook = lua_gethook(L1);
  if (hook != ((void *)0) && hook != hookf) { /* external hook? */
    lua_pushlstring(L, "" "external hook", (sizeof("external hook")/sizeof(char))-1);
  } else {
    gethooktable(L);
    lua_pushlightuserdata(L, L1);
    lua_rawget(L, -2); /* get hook */
    lua_remove(L, -2); /* remove hook table */
  }
  lua_pushstring(L, unmakemask(mask, buff));
  lua_pushinteger(L, lua_gethookcount(L1));
  return 3;
}


static int db_debug (lua_State *L) {
  for (;;) {
    char buffer[250];
    fputs("lua_debug> ", stderr);
    if (fgets(buffer, sizeof(buffer), stdin) == 0 ||
        strcmp(buffer, "cont\n") == 0)
    {
      return 0;
    }
    if (luaL_loadbuffer(L, buffer, strlen(buffer), "=(debug command)") ||
        lua_pcall(L, 0, 0, 0)) {
      fputs(lua_tolstring(L, (-1), ((void *)0)), stderr);
      fputs("\n", stderr);
    }
    lua_settop(L, 0); /* remove eventual returns */
  }
}


static int db_errorfb (lua_State *L) {
  int level;
  int firstpart = 1; /* still before eventual `...' */
  int arg;
  lua_State *L1 = getthread(L, &arg);
  lua_Debug ar;
  if (lua_isnumber(L, arg+2)) {
    level = (int)lua_tointeger(L, arg+2);
    lua_settop(L, -(1)-1);
  }
  else {
    level = (L == L1) ? 1 : 0; /* level 0 may be this own function */
  }
  if (lua_gettop(L) == arg) {
    lua_pushlstring(L, "" "", (sizeof("")/sizeof(char))-1);
  }
  else if (!lua_isstring(L, arg+1)) {
    return 1; /* message is not a string */
  }
  else {
    lua_pushlstring(L, "" "\n", (sizeof("\n")/sizeof(char))-1);
  }
  lua_pushlstring(L, "" "stack traceback:", (sizeof("stack traceback:")/sizeof(char))-1);
  while (lua_getstack(L1, level++, &ar)) {
    if (level > 12 && firstpart) {
      /* no more than `LEVELS2' more levels? */
      if (!lua_getstack(L1, level+10, &ar)) {
        level--; /* keep going */
      } else {
        lua_pushlstring(L, "" "\n\t...", (sizeof("\n\t...")/sizeof(char))-1); /* too many levels */
        while (lua_getstack(L1, level+10, &ar)) { /* find last levels */
          level++;
        }
      }
      firstpart = 0;
      continue;
    }
    lua_pushlstring(L, "" "\n\t", (sizeof("\n\t")/sizeof(char))-1);
    lua_getinfo(L1, "Snl", &ar);
    lua_pushfstring(L, "%s:", ar.short_src);
    if (ar.currentline > 0) {
      lua_pushfstring(L, "%d:", ar.currentline);
    }
    if (*ar.namewhat != '\0') { /* is there a name? */
        lua_pushfstring(L, " in function " "'" "%s" "'", ar.name);
    } else {
      if (*ar.what == 'm') { /* main? */
        lua_pushfstring(L, " in main chunk");
      } else if (*ar.what == 'C' || *ar.what == 't') {
        lua_pushlstring(L, "" " ?", (sizeof(" ?")/sizeof(char))-1); /* C function or tail call */
      } else {
        lua_pushfstring(L, " in function <%s:%d>",
                           ar.short_src, ar.linedefined);
      }
    }
    lua_concat(L, lua_gettop(L) - arg);
  }
  lua_concat(L, lua_gettop(L) - arg);
  return 1;
}


static const luaL_Reg dblib[] = {
  {"debug", db_debug},
  {"getfenv", db_getfenv},
  {"gethook", db_gethook},
  {"getinfo", db_getinfo},
  {"getlocal", db_getlocal},
  {"getregistry", db_getregistry},
  {"getmetatable", db_getmetatable},
  {"getupvalue", db_getupvalue},
  {"setfenv", db_setfenv},
  {"sethook", db_sethook},
  {"setlocal", db_setlocal},
  {"setmetatable", db_setmetatable},
  {"setupvalue", db_setupvalue},
  {"traceback", db_errorfb},
  {((void *)0), ((void *)0)}
};


extern int luaopen_debug (lua_State *L) {
  luaL_register(L, "debug", dblib);
  return 1;
}


