/*
** $Id: ltm.c,v 2.8.1.1 2007/12/27 13:02:25 roberto Exp $
** Tag methods
** See Copyright Notice in lua.h
*/


#include <string.h>

#define ltm_c
#define LUA_CORE

#include "lua.h"

#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"



const char *const luaT_typenames[] = {
  "nil",
  "boolean",
  "userdata",
  "number",
  "string",
  "table",
  "function",
  "userdata",
  "thread",
  "proto",
  "upval"
};


void luaT_init (lua_State *L) {
  //                                    17 tag-method allow user to self-defined
  static const char *const luaT_eventname[] = { /* ORDER TM */
    "__index",
    "__newindex",
    "__gc",
    "__mode",
    "__eq",
    "__add",
    "__sub",
    "__mul",
    "__div",
    "__mod",
    "__pow",
    "__unm",
    "__len",
    "__lt",
    "__le",
    "__concat",
    "__call"
  };
  int i;
  for (i=0; i<TM_N; i++) {
    /*  G(L)->tmname[i] =      luaS_new(L, luaT_eventname[i]); */
    (L->l_G)->tmname[i] = (luaS_newlstr(L, luaT_eventname[i], strlen(luaT_eventname[i])));
    /*  luaS_fix(G(L)->tmname[i]); */
    (L->l_G->tmname[i]->tsv.marked) |= (1<<5); /* never collect these names */
  }
}


/*
** function to be used with macro "fasttm": optimized for absence of
** tag methods
*/
const TValue *luaT_gettm (Table *events, TMS event, TString *ename) {
  const TValue *tm = luaH_getstr(events, ename);
  /* lua_assert(event <= TM_EQ); */
  ((void)0);
  /*   ttisnil(tm) */
  if ( tm->tt == LUA_TNIL ) { /* no tag method? */
    /*                cast_byte(1u<<event); */
    events->flags |= (lu_byte)(1u<<event); /* cache this fact */
    return NULL;
  }
  else {
    return tm;
  }
}


const TValue *luaT_gettmbyobj (lua_State *L, const TValue *o, TMS event) {
  Table *mt;
  /*      ttype(o) */
  switch ( o->tt ) {
    case LUA_TTABLE: { /* 5 */
      /*     hvalue(o) */
      mt = (&(o)->value.gc->h)->metatable;
      break;
    }
    case LUA_TUSERDATA: { /* 7 */
      /*                    uvalue(o) */
      mt = (&(&(o)->value.gc->u)->uv)->metatable;
      break;
    }
    default: {
      /*    G(L)        ttype(o)*/
      mt = (L->l_G)->mt[ o->tt ];
    }
  }
  /*                            G(L)                        luaO_nilobject  */
  return (mt ? luaH_getstr(mt, (L->l_G)->tmname[event]) : (&luaO_nilobject_));
}


