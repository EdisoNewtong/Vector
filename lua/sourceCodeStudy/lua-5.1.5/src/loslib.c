/*
** $Id: loslib.c,v 1.19.1.3 2008/01/18 16:38:18 roberto Exp $
** Standard Operating System library
** See Copyright Notice in lua.h
*/


#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define loslib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


static int os_pushresult (lua_State *L, int i, const char *filename) {
  int en = (*__errno_location ()); /* calls to Lua API may change this value */
  if (i) {
    lua_pushboolean(L, 1);
    return 1;
  }
  else {
    lua_pushnil(L);
    lua_pushfstring(L, "%s: %s", filename, strerror(en));
    lua_pushinteger(L, en);
    return 3;
  }
}


static int os_execute (lua_State *L) {
  lua_pushinteger(L, system((luaL_optlstring(L, (1), (((void *)0)), ((void *)0)))));
  return 1;
}


static int os_remove (lua_State *L) {
  const char *filename = (luaL_checklstring(L, (1), ((void *)0)));
  return os_pushresult(L, remove(filename) == 0, filename);
}


static int os_rename (lua_State *L) {
  const char *fromname = (luaL_checklstring(L, (1), ((void *)0)));
  const char *toname = (luaL_checklstring(L, (2), ((void *)0)));
  return os_pushresult(L, rename(fromname, toname) == 0, fromname);
}


static int os_tmpname (lua_State *L) {
  char buff[32];
  int err;
  { strcpy(buff, "/tmp/lua_XXXXXX"); err = mkstemp(buff); if (err != -1) close(err); err = (err == -1); };
  if (err) {
    return luaL_error(L, "unable to generate a unique filename");
  }
  lua_pushstring(L, buff);
  return 1;
}


static int os_getenv (lua_State *L) {
  lua_pushstring(L, getenv((luaL_checklstring(L, (1), ((void *)0))))); /* if NULL push nil */
  return 1;
}


static int os_clock (lua_State *L) {
  lua_pushnumber(L, ((lua_Number)clock())/(lua_Number)1000000l);
  return 1;
}


/*
** {======================================================
** Time/Date operations
** { year=%Y, month=%m, day=%d, hour=%H, min=%M, sec=%S,
**   wday=%w+1, yday=%j, isdst=? }
** =======================================================
*/
static void setfield (lua_State *L, const char *key, int value) {
  lua_pushinteger(L, value);
  lua_setfield(L, -2, key);
}


static void setboolfield (lua_State *L, const char *key, int value) {
  if (value < 0) { /* undefined? */
    return; /* does not set field */
  }
  lua_pushboolean(L, value);
  lua_setfield(L, -2, key);
}


static int getboolfield (lua_State *L, const char *key) {
  int res;
  lua_getfield(L, -1, key);
  res = (lua_type(L, (-1)) == 0) ? -1 : lua_toboolean(L, -1);
  lua_settop(L, -(1)-1);
  return res;
}


static int getfield (lua_State *L, const char *key, int d) {
  int res;
  lua_getfield(L, -1, key);
  if (lua_isnumber(L, -1)) {
    res = (int)lua_tointeger(L, -1);
  } else {
    if (d < 0) {
      return luaL_error(L, "field " "'" "%s" "'" " missing in date table", key);
    }
    res = d;
  }
  lua_settop(L, -(1)-1);
  return res;
}


static int os_date (lua_State *L) {
  const char *s = (luaL_optlstring(L, (1), ("%c"), ((void *)0)));
  time_t t = ((lua_type(L, ((2))) <= 0) ? (time(((void *)0))) : (time_t)luaL_checknumber(L,(2)));
  struct tm *stm;
  if (*s == '!') { /* UTC? */
    stm = gmtime(&t);
    s++; /* skip `!' */
  }
  else {
    stm = localtime(&t);
  }
  if (stm == ((void *)0)) { /* invalid date? */
    lua_pushnil(L);
  } else if (strcmp(s, "*t") == 0) {
    lua_createtable(L, 0, 9); /* 9 = number of fields */
    setfield(L, "sec", stm->tm_sec);
    setfield(L, "min", stm->tm_min);
    setfield(L, "hour", stm->tm_hour);
    setfield(L, "day", stm->tm_mday);
    setfield(L, "month", stm->tm_mon+1);
    setfield(L, "year", stm->tm_year+1900);
    setfield(L, "wday", stm->tm_wday+1);
    setfield(L, "yday", stm->tm_yday+1);
    setboolfield(L, "isdst", stm->tm_isdst);
  }
  else {
    char cc[3];
    luaL_Buffer b;
    cc[0] = '%'; cc[2] = '\0';
    luaL_buffinit(L, &b);
    for (; *s; s++) {
      if (*s != '%' || *(s + 1) == '\0') { /* no conversion specifier? */
        ((void)((&b)->p < ((&b)->buffer+8192) || luaL_prepbuffer(&b)), (*(&b)->p++ = (char)(*s)));
      } else {
        size_t reslen;
        char buff[200]; /* should be big enough for any conversion result */
        cc[1] = *(++s);
        reslen = strftime(buff, sizeof(buff), cc, stm);
        luaL_addlstring(&b, buff, reslen);
      }
    }
    luaL_pushresult(&b);
  }
  return 1;
}


static int os_time (lua_State *L) {
  time_t t;
  if ((lua_type(L, (1)) <= 0)) { /* called without args? */
    t = time(((void *)0)); /* get current time */
  } else {
    struct tm ts;
    luaL_checktype(L, 1, 5);
    lua_settop(L, 1); /* make sure table is at the top */
    ts.tm_sec = getfield(L, "sec", 0);
    ts.tm_min = getfield(L, "min", 0);
    ts.tm_hour = getfield(L, "hour", 12);
    ts.tm_mday = getfield(L, "day", -1);
    ts.tm_mon = getfield(L, "month", -1) - 1;
    ts.tm_year = getfield(L, "year", -1) - 1900;
    ts.tm_isdst = getboolfield(L, "isdst");
    t = mktime(&ts);
  }
  if (t == (time_t)(-1)) {
    lua_pushnil(L);
  } else {
    lua_pushnumber(L, (lua_Number)t);
  }
  return 1;
}


static int os_difftime (lua_State *L) {
  lua_pushnumber(L, difftime((time_t)(luaL_checknumber(L, 1)),
                             (time_t)(luaL_optnumber(L, 2, 0))));
  return 1;
}


/* }====================================================== */
static int os_setlocale (lua_State *L) {
  static const int cat[] = {
      6,
      3,
      0,
      4,
      1,
      2};
  static const char *const catnames[] = {
      "all",
      "collate",
      "ctype",
      "monetary",
      "numeric",
      "time",
      ((void *)0)
  };
  const char *l = (luaL_optlstring(L, (1), (((void *)0)), ((void *)0)));
  int op = luaL_checkoption(L, 2, "all", catnames);
  lua_pushstring(L, setlocale(cat[op], l));
  return 1;
}


static int os_exit (lua_State *L) {
  exit(((int)luaL_optinteger(L, (1), (0))));
}


static const luaL_Reg syslib[] = {
  {"clock", os_clock},
  {"date", os_date},
  {"difftime", os_difftime},
  {"execute", os_execute},
  {"exit", os_exit},
  {"getenv", os_getenv},
  {"remove", os_remove},
  {"rename", os_rename},
  {"setlocale", os_setlocale},
  {"time", os_time},
  {"tmpname", os_tmpname},
  {((void *)0), ((void *)0)}
};

/* }====================================================== */



extern int luaopen_os (lua_State *L) {
  luaL_register(L, "os", syslib);
  return 1;
}


