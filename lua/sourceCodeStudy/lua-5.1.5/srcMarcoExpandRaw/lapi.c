/*
** $Id: lapi.c,v 2.55.1.5 2008/07/04 18:41:18 roberto Exp $
** Lua API
** See Copyright Notice in lua.h
*/


#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

#define lapi_c
#define LUA_CORE

#include "lua.h"

#include "lapi.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lundump.h"
#include "lvm.h"



const char lua_ident[] =
  "$Lua: " "Lua 5.1.5" " " "Copyright (C) 1994-2012 Lua.org, PUC-Rio" " $\n"
  "$Authors: " "R. Ierusalimschy, L. H. de Figueiredo & W. Celes" " $\n"
  "$URL: www.lua.org $\n";




#define api_checknelems(L, n)	api_check(L, (n) <= (L->top - L->base))

#define api_checkvalidindex(L, i)	api_check(L, (i) != luaO_nilobject)

#define api_incr_top(L)   {api_check(L, L->top < L->ci->top); L->top++;}


static TValue *index2adr (lua_State *L, int idx) {
  if (idx > 0) {
    TValue *o = L->base + (idx - 1);
    { (void)L; };
    if (o >= L->top) {
      return ((TValue *)((&luaO_nilobject_)));
    } else {
      return o;
    }
  }
  else if (idx > (-10000)) {
    { (void)L; };
    return L->top + idx;
  }
  else switch (idx) { /* pseudo-indices */
    case (-10000): {
      return (&(L->l_G)->l_registry);
    }
    case (-10001): {
      Closure *func = ((&(L->ci->func)->value.gc->cl));
      { TValue *i_o=(&L->env); i_o->value.gc=((GCObject *)((func->c.env))); i_o->tt=5; ((void)0); };
      return &L->env;
    }
    case (-10002): {
      return (&L->l_gt);
    }
    default: {
      Closure *func = ((&(L->ci->func)->value.gc->cl));
      idx = (-10002) - idx;
      return (idx <= func->c.nupvalues)
                ? &func->c.upvalue[idx-1]
                : ((TValue *)((&luaO_nilobject_)));
    }
  }
}


static Table *getcurrenv (lua_State *L) {
  if (L->ci == L->base_ci) { /* no enclosing function? */
    return (&((&L->l_gt))->value.gc->h); /* use global table as environment */
  } else {
    Closure *func = ((&(L->ci->func)->value.gc->cl));
    return func->c.env;
  }
}


void luaA_pushobject (lua_State *L, const TValue *o) {
  { const TValue *o2=(o); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
  {{ (void)L; }; L->top++;};
}


extern int lua_checkstack (lua_State *L, int size) {
  int res = 1;
  ((void) 0);
  if (size > 8000 || (L->top - L->base + size) > 8000) {
    res = 0; /* stack overflow */
  } else if (size > 0) {
    if ((char *)L->stack_last - (char *)L->top <= (size)*(int)sizeof(TValue)) luaD_growstack(L, size); else ((void)0);;
    if (L->ci->top < L->top + size) {
      L->ci->top = L->top + size;
    }
  }
  ((void) 0);
  return res;
}


extern void lua_xmove (lua_State *from, lua_State *to, int n) {
  int i;
  if (from == to) {
    return;
  }
  ((void) 0);
  { (void)from; };
  { (void)from; };
  { (void)from; };
  from->top -= n;
  for (i = 0; i < n; i++) {
    { const TValue *o2=(from->top + i); TValue *o1=(to->top++); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
  }
  ((void) 0);
}


extern void lua_setlevel (lua_State *from, lua_State *to) {
  to->nCcalls = from->nCcalls;
}


extern lua_CFunction lua_atpanic (lua_State *L, lua_CFunction panicf) {
  lua_CFunction old;
  ((void) 0);
  old = (L->l_G)->panic;
  (L->l_G)->panic = panicf;
  ((void) 0);
  return old;
}


extern lua_State *lua_newthread (lua_State *L) {
  lua_State *L1;
  ((void) 0);
  { ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };
  L1 = luaE_newthread(L);
  { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((L1))); i_o->tt=8; ((void)0); };
  {{ (void)L; }; L->top++;};
  ((void) 0);
  ((void)L);
  return L1;
}


/*
** basic stack manipulation
*/


extern int lua_gettop (lua_State *L) {
  return ((int)((L->top - L->base)));
}


extern void lua_settop (lua_State *L, int idx) {
  ((void) 0);
  if (idx >= 0) {
    { (void)L; };
    while (L->top < L->base + idx) {
      ((L->top++)->tt=0);
    }
    L->top = L->base + idx;
  }
  else {
    { (void)L; };
    L->top += idx+1; /* `subtract' index (index is negative) */
  }
  ((void) 0);
}


extern void lua_remove (lua_State *L, int idx) {
  StkId p;
  ((void) 0);
  p = index2adr(L, idx);
  { (void)L; };
  while (++p < L->top) {
    { const TValue *o2=(p); TValue *o1=(p-1); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
  }
  L->top--;
  ((void) 0);
}


extern void lua_insert (lua_State *L, int idx) {
  StkId p;
  StkId q;
  ((void) 0);
  p = index2adr(L, idx);
  { (void)L; };
  for (q = L->top; q>p; q--) {
    { const TValue *o2=(q-1); TValue *o1=(q); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
  }
  { const TValue *o2=(L->top); TValue *o1=(p); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
  ((void) 0);
}


extern void lua_replace (lua_State *L, int idx) {
  StkId o;
  ((void) 0);
  /* explicit test for incompatible code */
  if (idx == (-10001) && L->ci == L->base_ci) {
    luaG_runerror(L, "no calling environment");
  }
  { (void)L; };
  o = index2adr(L, idx);
  { (void)L; };
  if (idx == (-10001)) {
    Closure *func = ((&(L->ci->func)->value.gc->cl));
    { (void)L; };
    func->c.env = (&(L->top - 1)->value.gc->h);
    { if (((((L->top - 1)->tt) >= 4) && (((((L->top - 1)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) && ((((((GCObject *)((func)))))->gch.marked) & ((1<<(2))))) luaC_barrierf(L,(((GCObject *)((func)))),((L->top - 1)->value.gc)); };
  }
  else {
    { const TValue *o2=(L->top - 1); TValue *o1=(o); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
    if (idx < (-10002)) { /* function upvalue? */
      { if (((((L->top - 1)->tt) >= 4) && (((((L->top - 1)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) && ((((((GCObject *)((((&(L->ci->func)->value.gc->cl)))))))->gch.marked) & ((1<<(2))))) luaC_barrierf(L,(((GCObject *)((((&(L->ci->func)->value.gc->cl)))))),((L->top - 1)->value.gc)); };
    }
  }
  L->top--;
  ((void) 0);
}


extern void lua_pushvalue (lua_State *L, int idx) {
  ((void) 0);
  { const TValue *o2=(index2adr(L, idx)); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
  {{ (void)L; }; L->top++;};
  ((void) 0);
}


/*
** access functions (stack -> C)
*/


extern int lua_type (lua_State *L, int idx) {
  StkId o = index2adr(L, idx);
  return (o == (&luaO_nilobject_)) ? (-1) : ((o)->tt);
}


extern const char *lua_typename (lua_State *L, int t) {
  ((void)(L));
  return (t == (-1)) ? "no value" : luaT_typenames[t];
}


extern int lua_iscfunction (lua_State *L, int idx) {
  StkId o = index2adr(L, idx);
  return (((o)->tt) == 6 && (&(o)->value.gc->cl)->c.isC);
}


extern int lua_isnumber (lua_State *L, int idx) {
  TValue n;
  const TValue *o = index2adr(L, idx);
  return (((o)->tt) == 3 || (((o) = luaV_tonumber(o,&n)) != ((void *)0)));
}


extern int lua_isstring (lua_State *L, int idx) {
  int t = lua_type(L, idx);
  return (t == 4 || t == 3);
}


extern int lua_isuserdata (lua_State *L, int idx) {
  const TValue *o = index2adr(L, idx);
  return ((((o)->tt) == 7) || (((o)->tt) == 2));
}


extern int lua_rawequal (lua_State *L, int index1, int index2) {
  StkId o1 = index2adr(L, index1);
  StkId o2 = index2adr(L, index2);
  return (o1 == (&luaO_nilobject_) || o2 == (&luaO_nilobject_)) ? 0
         : luaO_rawequalObj(o1, o2);
}


extern int lua_equal (lua_State *L, int index1, int index2) {
  StkId o1, o2;
  int i;
  ((void) 0); /* may call tag method */
  o1 = index2adr(L, index1);
  o2 = index2adr(L, index2);
  i = (o1 == (&luaO_nilobject_) || o2 == (&luaO_nilobject_)) ? 0 : (((o1)->tt) == ((o2)->tt) && luaV_equalval(L, o1, o2));
  ((void) 0);
  return i;
}


extern int lua_lessthan (lua_State *L, int index1, int index2) {
  StkId o1, o2;
  int i;
  ((void) 0); /* may call tag method */
  o1 = index2adr(L, index1);
  o2 = index2adr(L, index2);
  i = (o1 == (&luaO_nilobject_) || o2 == (&luaO_nilobject_)) ? 0
       : luaV_lessthan(L, o1, o2);
  ((void) 0);
  return i;
}


extern lua_Number lua_tonumber (lua_State *L, int idx) {
  TValue n;
  const TValue *o = index2adr(L, idx);
  if ((((o)->tt) == 3 || (((o) = luaV_tonumber(o,&n)) != ((void *)0)))) {
    return ((o)->value.n);
  } else {
    return 0;
  }
}


extern lua_Integer lua_tointeger (lua_State *L, int idx) {
  TValue n;
  const TValue *o = index2adr(L, idx);
  if ((((o)->tt) == 3 || (((o) = luaV_tonumber(o,&n)) != ((void *)0)))) {
    lua_Integer res;
    lua_Number num = ((o)->value.n);
    ((res)=(lua_Integer)(num));
    return res;
  }
  else {
    return 0;
  }
}


extern int lua_toboolean (lua_State *L, int idx) {
  const TValue *o = index2adr(L, idx);
  return !((((o)->tt) == 0) || ((((o)->tt) == 1) && ((o)->value.b) == 0));
}


extern const char *lua_tolstring (lua_State *L, int idx, size_t *len) {
  StkId o = index2adr(L, idx);
  if (!(((o)->tt) == 4)) {
    ((void) 0); /* `luaV_tostring' may create a new string */
    if (!luaV_tostring(L, o)) { /* conversion failed? */
      if (len != ((void *)0)) {
        *len = 0;
      }
      ((void) 0);
      return ((void *)0);
    }
    { ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };
    o = index2adr(L, idx); /* previous call may reallocate the stack */
    ((void) 0);
  }
  if (len != ((void *)0)) {
    *len = (&(&(o)->value.gc->ts)->tsv)->len;
  }
  return ((const char *)(((&(o)->value.gc->ts)) + 1));
}


extern size_t lua_objlen (lua_State *L, int idx) {
  StkId o = index2adr(L, idx);
  switch (((o)->tt)) {
    case 4: {
      return (&(&(o)->value.gc->ts)->tsv)->len;
    }
    case 7: {
      return (&(&(o)->value.gc->u)->uv)->len;
    }
    case 5: {
      return luaH_getn((&(o)->value.gc->h));
    }
    case 3: {
      size_t l;
      ((void) 0); /* `luaV_tostring' may create a new string */
      l = (luaV_tostring(L, o) ? (&(&(o)->value.gc->ts)->tsv)->len : 0);
      ((void) 0);
      return l;
    }
    default: {
      return 0;
    }
  }
}


extern lua_CFunction lua_tocfunction (lua_State *L, int idx) {
  StkId o = index2adr(L, idx);
  return (!(((o)->tt) == 6 && (&(o)->value.gc->cl)->c.isC)) ? ((void *)0) : (&(o)->value.gc->cl)->c.f;
}


extern void *lua_touserdata (lua_State *L, int idx) {
  StkId o = index2adr(L, idx);
  switch (((o)->tt)) {
    case 7: {
      return ((&(o)->value.gc->u) + 1);
    }
    case 2: {
      return ((o)->value.p);
    }
    default: {
      return ((void *)0);
    }
  }
}


extern lua_State *lua_tothread (lua_State *L, int idx) {
  StkId o = index2adr(L, idx);
  return (!(((o)->tt) == 8)) ? ((void *)0) : (&(o)->value.gc->th);
}


extern const void *lua_topointer (lua_State *L, int idx) {
  StkId o = index2adr(L, idx);
  switch (((o)->tt)) {
    case 5: {
      return (&(o)->value.gc->h);
    }
    case 6: {
      return (&(o)->value.gc->cl);
    }
    case 8: {
      return (&(o)->value.gc->th);
    }
    case 7:
    case 2: {
      return lua_touserdata(L, idx);
    }
    default: {
      return ((void *)0);
    }
  }
}


/*
** push functions (C -> stack)
*/


extern void lua_pushnil (lua_State *L) {
  ((void) 0);
  ((L->top)->tt=0);
  {{ (void)L; }; L->top++;};
  ((void) 0);
}


extern void lua_pushnumber (lua_State *L, lua_Number n) {
  ((void) 0);
  { TValue *i_o=(L->top); i_o->value.n=(n); i_o->tt=3; };
  {{ (void)L; }; L->top++;};
  ((void) 0);
}


extern void lua_pushinteger (lua_State *L, lua_Integer n) {
  ((void) 0);
  { TValue *i_o=(L->top); i_o->value.n=(((lua_Number)((n)))); i_o->tt=3; };
  {{ (void)L; }; L->top++;};
  ((void) 0);
}


extern void lua_pushlstring (lua_State *L, const char *s, size_t len) {
  ((void) 0);
  { ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };
  { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((luaS_newlstr(L, s, len)))); i_o->tt=4; ((void)0); };
  {{ (void)L; }; L->top++;};
  ((void) 0);
}


extern void lua_pushstring (lua_State *L, const char *s) {
  if (s == ((void *)0)) {
    lua_pushnil(L);
  } else {
    lua_pushlstring(L, s, strlen(s));
  }
}


extern const char *lua_pushvfstring (lua_State *L, const char *fmt,
                                      va_list argp) {
  const char *ret;
  ((void) 0);
  { ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };
  ret = luaO_pushvfstring(L, fmt, argp);
  ((void) 0);
  return ret;
}


extern const char *lua_pushfstring (lua_State *L, const char *fmt, ...) {
  const char *ret;
  va_list argp;
  ((void) 0);
  { ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };
  __builtin_va_start(argp,fmt);
  ret = luaO_pushvfstring(L, fmt, argp);
  __builtin_va_end(argp);
  ((void) 0);
  return ret;
}


extern void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n) {
  Closure *cl;
  ((void) 0);
  { ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };
  { (void)L; };
  cl = luaF_newCclosure(L, n, getcurrenv(L));
  cl->c.f = fn;
  L->top -= n;
  while (n--) {
    { const TValue *o2=(L->top+n); TValue *o1=(&cl->c.upvalue[n]); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
  }
  { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((cl))); i_o->tt=6; ((void)0); };
  ((void)0);
  {{ (void)L; }; L->top++;};
  ((void) 0);
}


extern void lua_pushboolean (lua_State *L, int b) {
  ((void) 0);
  { TValue *i_o=(L->top); i_o->value.b=((b != 0)); i_o->tt=1; }; /* ensure that true is 1 */
  {{ (void)L; }; L->top++;};
  ((void) 0);
}


extern void lua_pushlightuserdata (lua_State *L, void *p) {
  ((void) 0);
  { TValue *i_o=(L->top); i_o->value.p=(p); i_o->tt=2; };
  {{ (void)L; }; L->top++;};
  ((void) 0);
}


extern int lua_pushthread (lua_State *L) {
  ((void) 0);
  { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((L))); i_o->tt=8; ((void)0); };
  {{ (void)L; }; L->top++;};
  ((void) 0);
  return ((L->l_G)->mainthread == L);
}


/*
** get functions (Lua -> stack)
*/


extern void lua_gettable (lua_State *L, int idx) {
  StkId t;
  ((void) 0);
  t = index2adr(L, idx);
  { (void)L; };
  luaV_gettable(L, t, L->top - 1, L->top - 1);
  ((void) 0);
}


extern void lua_getfield (lua_State *L, int idx, const char *k) {
  StkId t;
  TValue key;
  ((void) 0);
  t = index2adr(L, idx);
  { (void)L; };
  { TValue *i_o=(&key); i_o->value.gc=((GCObject *)(((luaS_newlstr(L, k, strlen(k)))))); i_o->tt=4; ((void)0); };
  luaV_gettable(L, t, &key, L->top);
  {{ (void)L; }; L->top++;};
  ((void) 0);
}


extern void lua_rawget (lua_State *L, int idx) {
  StkId t;
  ((void) 0);
  t = index2adr(L, idx);
  { (void)L; };
  { const TValue *o2=(luaH_get((&(t)->value.gc->h), L->top - 1)); TValue *o1=(L->top - 1); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
  ((void) 0);
}


extern void lua_rawgeti (lua_State *L, int idx, int n) {
  StkId o;
  ((void) 0);
  o = index2adr(L, idx);
  { (void)L; };
  { const TValue *o2=(luaH_getnum((&(o)->value.gc->h), n)); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
  {{ (void)L; }; L->top++;};
  ((void) 0);
}


extern void lua_createtable (lua_State *L, int narray, int nrec) {
  ((void) 0);
  { ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };
  { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((luaH_new(L, narray, nrec)))); i_o->tt=5; ((void)0); };
  {{ (void)L; }; L->top++;};
  ((void) 0);
}


extern int lua_getmetatable (lua_State *L, int objindex) {
  const TValue *obj;
  Table *mt = ((void *)0);
  int res;
  ((void) 0);
  obj = index2adr(L, objindex);
  switch (((obj)->tt)) {
    case 5: {
      mt = (&(obj)->value.gc->h)->metatable;
      break;
    }
    case 7: {
      mt = (&(&(obj)->value.gc->u)->uv)->metatable;
      break;
    }
    default: {
      mt = (L->l_G)->mt[((obj)->tt)];
      break;
    }
  }
  if (mt == ((void *)0)) {
    res = 0;
  } else {
    { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((mt))); i_o->tt=5; ((void)0); };
    {{ (void)L; }; L->top++;};
    res = 1;
  }
  ((void) 0);
  return res;
}


extern void lua_getfenv (lua_State *L, int idx) {
  StkId o;
  ((void) 0);
  o = index2adr(L, idx);
  { (void)L; };
  switch (((o)->tt)) {
    case 6: {
      { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)(((&(o)->value.gc->cl)->c.env))); i_o->tt=5; ((void)0); };
      break;
    }
    case 7: {
      { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)(((&(&(o)->value.gc->u)->uv)->env))); i_o->tt=5; ((void)0); };
      break;
    }
    case 8: {
      { const TValue *o2=((&(&(o)->value.gc->th)->l_gt)); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
      break;
    }
    default: {
      ((L->top)->tt=0);
      break;
    }
  }
  {{ (void)L; }; L->top++;};
  ((void) 0);
}


/*
** set functions (stack -> Lua)
*/


extern void lua_settable (lua_State *L, int idx) {
  StkId t;
  ((void) 0);
  { (void)L; };
  t = index2adr(L, idx);
  { (void)L; };
  luaV_settable(L, t, L->top - 2, L->top - 1);
  L->top -= 2; /* pop index and value */
  ((void) 0);
}


extern void lua_setfield (lua_State *L, int idx, const char *k) {
  StkId t;
  TValue key;
  ((void) 0);
  { (void)L; };
  t = index2adr(L, idx);
  { (void)L; };
  { TValue *i_o=(&key); i_o->value.gc=((GCObject *)(((luaS_newlstr(L, k, strlen(k)))))); i_o->tt=4; ((void)0); };
  luaV_settable(L, t, &key, L->top - 1);
  L->top--; /* pop value */
  ((void) 0);
}


extern void lua_rawset (lua_State *L, int idx) {
  StkId t;
  ((void) 0);
  { (void)L; };
  t = index2adr(L, idx);
  { (void)L; };
  { const TValue *o2=(L->top-1); TValue *o1=(luaH_set(L, (&(t)->value.gc->h), L->top-2)); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
  { if (((((L->top-1)->tt) >= 4) && (((((L->top-1)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) && ((((((GCObject *)(((&(t)->value.gc->h))))))->gch.marked) & ((1<<(2))))) luaC_barrierback(L,(&(t)->value.gc->h)); };
  L->top -= 2;
  ((void) 0);
}


extern void lua_rawseti (lua_State *L, int idx, int n) {
  StkId o;
  ((void) 0);
  { (void)L; };
  o = index2adr(L, idx);
  { (void)L; };
  { const TValue *o2=(L->top-1); TValue *o1=(luaH_setnum(L, (&(o)->value.gc->h), n)); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
  { if (((((L->top-1)->tt) >= 4) && (((((L->top-1)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) && ((((((GCObject *)(((&(o)->value.gc->h))))))->gch.marked) & ((1<<(2))))) luaC_barrierback(L,(&(o)->value.gc->h)); };
  L->top--;
  ((void) 0);
}


extern int lua_setmetatable (lua_State *L, int objindex) {
  TValue *obj;
  Table *mt;
  ((void) 0);
  { (void)L; };
  obj = index2adr(L, objindex);
  { (void)L; };
  if ((((L->top - 1)->tt) == 0)) {
    mt = ((void *)0);
  } else {
    { (void)L; };
    mt = (&(L->top - 1)->value.gc->h);
  }
  switch (((obj)->tt)) {
    case 5: {
      (&(obj)->value.gc->h)->metatable = mt;
      if (mt) {
        { if (((((((GCObject *)((mt)))))->gch.marked) & ((((1<<(0)) | (1<<(1)))))) && ((((((GCObject *)(((&(obj)->value.gc->h))))))->gch.marked) & ((1<<(2))))) luaC_barrierback(L,(&(obj)->value.gc->h)); };
      }
      break;
    }
    case 7: {
      (&(&(obj)->value.gc->u)->uv)->metatable = mt;
      if (mt) {
        { if (((((((GCObject *)((mt)))))->gch.marked) & ((((1<<(0)) | (1<<(1)))))) && ((((((GCObject *)(((&(obj)->value.gc->u))))))->gch.marked) & ((1<<(2))))) luaC_barrierf(L,(((GCObject *)(((&(obj)->value.gc->u))))),(((GCObject *)((mt))))); };
      }
      break;
    }
    default: {
      (L->l_G)->mt[((obj)->tt)] = mt;
      break;
    }
  }
  L->top--;
  ((void) 0);
  return 1;
}


extern int lua_setfenv (lua_State *L, int idx) {
  StkId o;
  int res = 1;
  ((void) 0);
  { (void)L; };
  o = index2adr(L, idx);
  { (void)L; };
  { (void)L; };
  switch (((o)->tt)) {
    case 6: {
      (&(o)->value.gc->cl)->c.env = (&(L->top - 1)->value.gc->h);
      break;
    }
    case 7: {
      (&(&(o)->value.gc->u)->uv)->env = (&(L->top - 1)->value.gc->h);
      break;
    }
    case 8: {
      { TValue *i_o=((&(&(o)->value.gc->th)->l_gt)); i_o->value.gc=((GCObject *)(((&(L->top - 1)->value.gc->h)))); i_o->tt=5; ((void)0); };
      break;
    }
    default: {
      res = 0;
      break;
    }
  }
  if (res) {
    { if (((((((GCObject *)(((&(L->top - 1)->value.gc->h))))))->gch.marked) & ((((1<<(0)) | (1<<(1)))))) && ((((((GCObject *)((((o)->value.gc))))))->gch.marked) & ((1<<(2))))) luaC_barrierf(L,(((GCObject *)((((o)->value.gc))))),(((GCObject *)(((&(L->top - 1)->value.gc->h)))))); };
  }
  L->top--;
  ((void) 0);
  return res;
}


/*
** `load' and `call' functions (run Lua code)
*/


#define adjustresults(L,nres) \
    { if (nres == LUA_MULTRET && L->top >= L->ci->top) L->ci->top = L->top; }


#define checkresults(L,na,nr) \
     api_check(L, (nr) == LUA_MULTRET || (L->ci->top - L->top >= (nr) - (na)))
	


extern void lua_call (lua_State *L, int nargs, int nresults) {
  StkId func;
  ((void) 0);
  { (void)L; };
  { (void)L; };
  func = L->top - (nargs+1);
  luaD_call(L, func, nresults);
  { if (nresults == (-1) && L->top >= L->ci->top) L->ci->top = L->top; };
  ((void) 0);
}


/*
** Execute a protected call.
*/
struct CallS { /* data to `f_call' */
  StkId func;
  int nresults;
};


static void f_call (lua_State *L, void *ud) {
  struct CallS *c = ((struct CallS *)(ud));
  luaD_call(L, c->func, c->nresults);
}


extern int lua_pcall (lua_State *L, int nargs, int nresults, int errfunc) {
  struct CallS c;
  int status;
  ptrdiff_t func;
  ((void) 0);
  { (void)L; };
  { (void)L; };
  if (errfunc == 0) {
    func = 0;
  } else {
    StkId o = index2adr(L, errfunc);
    { (void)L; };
    func = ((char *)(o) - (char *)L->stack);
  }
  c.func = L->top - (nargs+1); /* function to be called */
  c.nresults = nresults;
  status = luaD_pcall(L, f_call, &c, ((char *)(c.func) - (char *)L->stack), func);
  { if (nresults == (-1) && L->top >= L->ci->top) L->ci->top = L->top; };
  ((void) 0);
  return status;
}


/*
** Execute a protected C call.
*/
struct CCallS { /* data to `f_Ccall' */
  lua_CFunction func;
  void *ud;
};


static void f_Ccall (lua_State *L, void *ud) {
  struct CCallS *c = ((struct CCallS *)(ud));
  Closure *cl;
  cl = luaF_newCclosure(L, 0, getcurrenv(L));
  cl->c.f = c->func;
  { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((cl))); i_o->tt=6; ((void)0); }; /* push function */
  {{ (void)L; }; L->top++;};
  { TValue *i_o=(L->top); i_o->value.p=(c->ud); i_o->tt=2; }; /* push only argument */
  {{ (void)L; }; L->top++;};
  luaD_call(L, L->top - 2, 0);
}


extern int lua_cpcall (lua_State *L, lua_CFunction func, void *ud) {
  struct CCallS c;
  int status;
  ((void) 0);
  c.func = func;
  c.ud = ud;
  status = luaD_pcall(L, f_Ccall, &c, ((char *)(L->top) - (char *)L->stack), 0);
  ((void) 0);
  return status;
}


extern int lua_load (lua_State *L, lua_Reader reader, void *data,
                      const char *chunkname) {
  ZIO z;
  int status;
  ((void) 0);
  if (!chunkname) {
    chunkname = "?";
  }
  luaZ_init(L, &z, reader, data);
  status = luaD_protectedparser(L, &z, chunkname);
  ((void) 0);
  return status;
}


extern int lua_dump (lua_State *L, lua_Writer writer, void *data) {
  int status;
  TValue *o;
  ((void) 0);
  { (void)L; };
  o = L->top - 1;
  if ((((o)->tt) == 6 && !(&(o)->value.gc->cl)->c.isC)) {
    status = luaU_dump(L, (&(o)->value.gc->cl)->l.p, writer, data, 0);
  }
  else {
    status = 1;
  }
  ((void) 0);
  return status;
}


extern int lua_status (lua_State *L) {
  return L->status;
}


/*
** Garbage-collection function
*/

extern int lua_gc (lua_State *L, int what, int data) {
  int res = 0;
  global_State *g;
  ((void) 0);
  g = (L->l_G);
  switch (what) {
    case 0: {
      g->GCthreshold = ((lu_mem)(~(lu_mem)0)-2);
      break;
    }
    case 1: {
      g->GCthreshold = g->totalbytes;
      break;
    }
    case 2: {
      luaC_fullgc(L);
      break;
    }
    case 3: {
      /* GC values are expressed in Kbytes: #bytes/2^10 */
      res = ((int)((g->totalbytes >> 10)));
      break;
    }
    case 4: {
      res = ((int)((g->totalbytes & 0x3ff)));
      break;
    }
    case 5: {
      lu_mem a = (((lu_mem)(data)) << 10);
      if (a <= g->totalbytes) {
        g->GCthreshold = g->totalbytes - a;
      } else {
        g->GCthreshold = 0;
      }
      while (g->GCthreshold <= g->totalbytes) {
        luaC_step(L);
        if (g->gcstate == 0) { /* end of cycle? */
          res = 1; /* signal it */
          break;
        }
      }
      break;
    }
    case 6: {
      res = g->gcpause;
      g->gcpause = data;
      break;
    }
    case 7: {
      res = g->gcstepmul;
      g->gcstepmul = data;
      break;
    }
    default: {
      res = -1; /* invalid option */
    }
  }
  ((void) 0);
  return res;
}



/*
** miscellaneous functions
*/


extern int lua_error (lua_State *L) {
  ((void) 0);
  { (void)L; };
  luaG_errormsg(L);
  ((void) 0);
  return 0; /* to avoid warnings */
}


extern int lua_next (lua_State *L, int idx) {
  StkId t;
  int more;
  ((void) 0);
  t = index2adr(L, idx);
  { (void)L; };
  more = luaH_next(L, (&(t)->value.gc->h), L->top - 1);
  if (more) {
    {{ (void)L; }; L->top++;};
  }
  else { /* no more elements */
    L->top -= 1; /* remove key */
  }
  ((void) 0);
  return more;
}


extern void lua_concat (lua_State *L, int n) {
  ((void) 0);
  { (void)L; };
  if (n >= 2) {
    { ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };
    luaV_concat(L, n, ((int)((L->top - L->base))) - 1);
    L->top -= (n-1);
  }
  else if (n == 0) { /* push empty string */
    { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((luaS_newlstr(L, "", 0)))); i_o->tt=4; ((void)0); };
    {{ (void)L; }; L->top++;};
  }
  /* else n == 1; nothing to do */
  ((void) 0);
}


extern lua_Alloc lua_getallocf (lua_State *L, void **ud) {
  lua_Alloc f;
  ((void) 0);
  if (ud) {
    *ud = (L->l_G)->ud;
  }
  f = (L->l_G)->frealloc;
  ((void) 0);
  return f;
}


extern void lua_setallocf (lua_State *L, lua_Alloc f, void *ud) {
  ((void) 0);
  (L->l_G)->ud = ud;
  (L->l_G)->frealloc = f;
  ((void) 0);
}


extern void *lua_newuserdata (lua_State *L, size_t size) {
  Udata *u;
  ((void) 0);
  { ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };
  u = luaS_newudata(L, size, getcurrenv(L));
  { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((u))); i_o->tt=7; ((void)0); };
  {{ (void)L; }; L->top++;};
  ((void) 0);
  return u + 1;
}


static const char *aux_upvalue (StkId fi, int n, TValue **val) {
  Closure *f;
  if (!(((fi)->tt) == 6)) {
    return ((void *)0);
  }
  f = (&(fi)->value.gc->cl);
  if (f->c.isC) {
    if (!(1 <= n && n <= f->c.nupvalues)) {
      return ((void *)0);
    }
    *val = &f->c.upvalue[n-1];
    return "";
  }
  else {
    Proto *p = f->l.p;
    if (!(1 <= n && n <= p->sizeupvalues)) {
      return ((void *)0);
    }
    *val = f->l.upvals[n-1]->v;
    return ((const char *)((p->upvalues[n-1]) + 1));
  }
}


extern const char *lua_getupvalue (lua_State *L, int funcindex, int n) {
  const char *name;
  TValue *val;
  ((void) 0);
  name = aux_upvalue(index2adr(L, funcindex), n, &val);
  if (name) {
    { const TValue *o2=(val); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
    {{ (void)L; }; L->top++;};
  }
  ((void) 0);
  return name;
}


extern const char *lua_setupvalue (lua_State *L, int funcindex, int n) {
  const char *name;
  TValue *val;
  StkId fi;
  ((void) 0);
  fi = index2adr(L, funcindex);
  { (void)L; };
  name = aux_upvalue(fi, n, &val);
  if (name) {
    L->top--;
    { const TValue *o2=(L->top); TValue *o1=(val); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
    { if (((((L->top)->tt) >= 4) && (((((L->top)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) && ((((((GCObject *)(((&(fi)->value.gc->cl))))))->gch.marked) & ((1<<(2))))) luaC_barrierf(L,(((GCObject *)(((&(fi)->value.gc->cl))))),((L->top)->value.gc)); };
  }
  ((void) 0);
  return name;
}


