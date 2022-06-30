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
    api_check(L, idx <= L->ci->top - L->base);  /* { (void)L; }; */
    if (o >= L->top) {
      /* return ((TValue *)((&luaO_nilobject_)));  */
      return cast(TValue *, luaO_nilobject);
    } else {
      return o;
    }
  }
  else if (idx > LUA_REGISTRYINDEX) {
    api_check(L, idx != 0 && -idx <= L->top - L->base); /* { (void)L; };  */
    return L->top + idx;
  }
  else switch (idx) { /* pseudo-indices */
    case LUA_REGISTRYINDEX: {
      /* return (&(L->l_G)->l_registry); */
      return registry(L);
    }
    case LUA_ENVIRONINDEX: {
      /* Closure *func = ((&(L->ci->func)->value.gc->cl));  */
      Closure *func = curr_func(L);
      /* { TValue *i_o=(&L->env); i_o->value.gc=((GCObject *)((func->c.env))); i_o->tt=LUA_TTABLE; ((void)0); };  */
      sethvalue(L, &L->env, func->c.env);
      return &L->env;
    }
    case LUA_GLOBALSINDEX: {
      /*  return (&L->l_gt);  */
      return gt(L);
    }
    default: {
      /* Closure *func = ((&(L->ci->func)->value.gc->cl));  */
      Closure *func = curr_func(L);
      idx = LUA_GLOBALSINDEX - idx;
      return (idx <= func->c.nupvalues)
                ? &func->c.upvalue[idx-1]
                : cast(TValue *, luaO_nilobject);  /* ((TValue *)((&luaO_nilobject_)));  */
    }
  }
}


static Table *getcurrenv (lua_State *L) {
  if (L->ci == L->base_ci) { /* no enclosing function? */
    /* return (&((&L->l_gt))->value.gc->h); */
    return hvalue(gt(L)); /* use global table as environment */ 
  } else {
    /* Closure *func = ((&(L->ci->func)->value.gc->cl)); */
    Closure *func = curr_func(L);
    return func->c.env;
  }
}


void luaA_pushobject (lua_State *L, const TValue *o) {
  /* { const TValue *o2=(o); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
  setobj2s(L, L->top, o);
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
}


extern int lua_checkstack (lua_State *L, int size) {
  int res = 1;
  lua_lock(L); /* ((void) 0); */
  /* LUAI_MAXCSTACK   8000 */
  if (size > LUAI_MAXCSTACK || (L->top - L->base + size) > LUAI_MAXCSTACK) {
    res = 0; /* stack overflow */
  } else if (size > 0) {
    /* luaD_checkstack(L, size); */
    if ((char *)L->stack_last - (char *)L->top <= (size)*(int)sizeof(TValue)) { 
		luaD_growstack(L, size); 
	} 
	else { 
		((void)0);;
	}


    if (L->ci->top < L->top + size) {
      L->ci->top = L->top + size;
    }
  }
  lua_unlock(L); /* ((void) 0); */
  return res;
}


extern void lua_xmove (lua_State *from, lua_State *to, int n) {
  int i;
  if (from == to) {
    return;
  }
  lua_lock(to); /* ((void) 0); */
  api_checknelems(from, n); /* { (void)from; }; */
  api_check(from, G(from) == G(to)); /* { (void)from; }; */
  api_check(from, to->ci->top - to->top >= n); /* { (void)from; }; */
  from->top -= n;
  for (i = 0; i < n; i++) {
    /* { const TValue *o2=(from->top + i); TValue *o1=(to->top++); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
    setobj2s(to, to->top++, from->top + i);
  }
  lua_unlock(to); /* ((void) 0); */
}


extern void lua_setlevel (lua_State *from, lua_State *to) {
  to->nCcalls = from->nCcalls;
}


extern lua_CFunction lua_atpanic (lua_State *L, lua_CFunction panicf) {
  lua_CFunction old;
  lua_lock(L); /* ((void) 0); */
  old = G(L)->panic; /* old = (L->l_G)->panic; */
  G(L)->panic = panicf; /* (L->l_G)->panic = panicf; */
  lua_unlock(L); /* ((void) 0); */
  return old;
}


extern lua_State *lua_newthread (lua_State *L) {
  lua_State *L1;
  lua_lock(L); /* ((void) 0); */
  /* luaC_checkGC(L); */
  {
	  ((void)0); 
	  if ( G(L)->totalbytes >= G(L)->GCthreshold ) { 
		  luaC_step(L); 
	  }
  }

  L1 = luaE_newthread(L);
  /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((L1))); i_o->tt=LUA_TTHREAD; ((void)0); }; */
  setthvalue(L, L->top, L1);
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  lua_unlock(L); /* ((void) 0); */
  luai_userstatethread(L, L1); /* ((void)L); */
  return L1;
}


/*
** basic stack manipulation
*/


extern int lua_gettop (lua_State *L) {
  /* return ((int)((L->top - L->base))); */
  return cast_int(L->top - L->base);
}


extern void lua_settop (lua_State *L, int idx) {
  lua_lock(L); /* ((void) 0); */
  if (idx >= 0) {
    api_check(L, idx <= L->stack_last - L->base); /* { (void)L; }; */
    while (L->top < L->base + idx) {
      /* ((L->top++)->tt=0); */
      setnilvalue(L->top++);
    }
    L->top = L->base + idx;
  }
  else {
    api_check(L, -(idx+1) <= (L->top - L->base)); /* { (void)L; }; */
    L->top += idx+1; /* `subtract' index (index is negative) */
  }
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_remove (lua_State *L, int idx) {
  StkId p;
  lua_lock(L); /* ((void) 0); */
  p = index2adr(L, idx);
  api_checkvalidindex(L, p); /* { (void)L; }; */
  while (++p < L->top) {
    /* { const TValue *o2=(p); TValue *o1=(p-1); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
    setobjs2s(L, p-1, p);
  }
  L->top--;
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_insert (lua_State *L, int idx) {
  StkId p;
  StkId q;
  lua_lock(L); /* ((void) 0); */
  p = index2adr(L, idx);
  api_checkvalidindex(L, p); /* { (void)L; }; */
  for (q = L->top; q>p; q--) {
    /* { const TValue *o2=(q-1); TValue *o1=(q); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
    setobjs2s(L, q, q-1);
  }
  /* { const TValue *o2=(L->top); TValue *o1=(p); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
  setobjs2s(L, p, L->top);
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_replace (lua_State *L, int idx) {
  StkId o;
  lua_lock(L); /* ((void) 0); */
  /* explicit test for incompatible code */
  if (idx == LUA_ENVIRONINDEX && L->ci == L->base_ci) {
    luaG_runerror(L, "no calling environment");
  }
  api_checknelems(L, 1); /* { (void)L; }; */
  o = index2adr(L, idx);
  api_checkvalidindex(L, o); /* { (void)L; }; */
  if (idx == LUA_ENVIRONINDEX) {
    /* Closure *func = ((&(L->ci->func)->value.gc->cl)); */
    Closure *func = curr_func(L); 
    api_check(L, ttistable(L->top - 1));  /* { (void)L; }; */
    /* func->c.env = (&(L->top - 1)->value.gc->h); */
    func->c.env = hvalue(L->top - 1);
    /* luaC_barrier(L, func, L->top - 1); */
    { 
		if ( ( ( (L->top - 1)->tt >= LUA_TSTRING ) && ( ((((L->top - 1)->value.gc))->gch.marked ) &  ( ( 1<< WHITE0BIT ) | (1 << WHITE1BIT) ) ) ) && ( ( ( (GCObject *)(func) )->gch.marked ) & ( ( 1 << BLACKBIT ) ) ) ) { 
			luaC_barrierf(L,( (GCObject *)(func) ),( (L->top - 1)->value.gc ) ); 
		} 
	};
  }
  else {
    /* { const TValue *o2=(L->top - 1); TValue *o1=(o); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
    setobj(L, o, L->top - 1); 
    if (idx < LUA_GLOBALSINDEX) { /* function upvalue? */
	  /* luaC_barrier(L, curr_func(L), L->top - 1); */
      {
		if ( ( ( (L->top - 1)->tt >= LUA_TSTRING ) && ( ( ( (L->top - 1)->value.gc )->gch.marked ) & ( (1<< WHITE0BIT ) | (1 << WHITE1BIT ) )) ) && ( ( ( (GCObject *)( (&(L->ci->func)->value.gc->cl) ) )->gch.marked ) & ( 1<< BLACKBIT ) ) ) {
		  luaC_barrierf(L,( (GCObject *)( (&(L->ci->func)->value.gc->cl) ) ) , ( (L->top - 1)->value.gc ) ); 
		}
	  };
    }
  }
  L->top--;
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_pushvalue (lua_State *L, int idx) {
  lua_lock(L); /* ((void) 0); */
  /* { const TValue *o2=(index2adr(L, idx)); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
  setobj2s(L, L->top, index2adr(L, idx));
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L); 
  lua_unlock(L); /* ((void) 0); */
}


/*
** access functions (stack -> C)
*/


extern int lua_type (lua_State *L, int idx) {
  StkId o = index2adr(L, idx);
  /* return (o == (&luaO_nilobject_)) ? (-1) : ((o)->tt); */
  return (o == luaO_nilobject) ? LUA_TNONE : ttype(o);
}


extern const char *lua_typename (lua_State *L, int t) {
  ((void)(L));
  /* return (t == (-1)) ? "no value" : luaT_typenames[t]; */
  return (t == LUA_TNONE) ? "no value" : luaT_typenames[t];
}


extern int lua_iscfunction (lua_State *L, int idx) {
  StkId o = index2adr(L, idx);
  /* return (((o)->tt) == LUA_TFUNCTION && (&(o)->value.gc->cl)->c.isC); */
  return iscfunction(o);
}


extern int lua_isnumber (lua_State *L, int idx) {
  TValue n;
  const TValue *o = index2adr(L, idx);
  /* return (((o)->tt) == LUA_TNUMBER || (((o) = luaV_tonumber(o,&n)) != NULL)); */
  return tonumber(o, &n);
}


extern int lua_isstring (lua_State *L, int idx) {
  int t = lua_type(L, idx);
  /* return (t == 4 || t == 3); */
  return (t == LUA_TSTRING || t == LUA_TNUMBER);
}


extern int lua_isuserdata (lua_State *L, int idx) {
  const TValue *o = index2adr(L, idx);
  /* return ((((o)->tt) == LUA_TUSERDATA) || (((o)->tt) == LUA_TLIGHTUSERDATA)); */
  return (ttisuserdata(o) || ttislightuserdata(o));
}


extern int lua_rawequal (lua_State *L, int index1, int index2) {
  StkId o1 = index2adr(L, index1);
  StkId o2 = index2adr(L, index2);
  return (o1 == luaO_nilobject || o2 == luaO_nilobject) ? 0
         : luaO_rawequalObj(o1, o2);
}


extern int lua_equal (lua_State *L, int index1, int index2) {
  StkId o1, o2;
  int i;
  lua_lock(L); /* ((void) 0); */ /* may call tag method */
  o1 = index2adr(L, index1);
  o2 = index2adr(L, index2);
  /* i = (o1 == (&luaO_nilobject_) || o2 == (&luaO_nilobject_)) ? 0 : (((o1)->tt) == ((o2)->tt) && luaV_equalval(L, o1, o2)); */
  i = (o1 == luaO_nilobject || o2 == luaO_nilobject) ? 0 : equalobj(L, o1, o2);
  lua_unlock(L); /* ((void) 0); */
  return i;
}


extern int lua_lessthan (lua_State *L, int index1, int index2) {
  StkId o1, o2;
  int i;
  lua_lock(L); /* ((void) 0); */ /* may call tag method */
  o1 = index2adr(L, index1);
  o2 = index2adr(L, index2);
  i = (o1 == luaO_nilobject || o2 == luaO_nilobject) ? 0
       : luaV_lessthan(L, o1, o2);
  lua_unlock(L); /* ((void) 0); */
  return i;
}


extern lua_Number lua_tonumber (lua_State *L, int idx) {
  TValue n;
  const TValue *o = index2adr(L, idx);
  /* if ((((o)->tt) == LUA_TNUMBER || (((o) = luaV_tonumber(o,&n)) != NULL))) { */
  if (tonumber(o, &n)) {
    /* return ((o)->value.n); */
    return nvalue(o);
  } else {
    return 0;
  }
}


extern lua_Integer lua_tointeger (lua_State *L, int idx) {
  TValue n;
  const TValue *o = index2adr(L, idx);
  /* if ((((o)->tt) == LUA_TNUMBER  || (((o) = luaV_tonumber(o,&n)) != NULL))) { */
  if (tonumber(o, &n)) {
    lua_Integer res;
    /* lua_Number num = ((o)->value.n); */
    lua_Number num = nvalue(o);
    /* ((res)=(lua_Integer)(num)); */
    lua_number2integer(res, num);
    return res;
  }
  else {
    return 0;
  }
}


extern int lua_toboolean (lua_State *L, int idx) {
  const TValue *o = index2adr(L, idx);
  /* return !((((o)->tt) == LUA_TNIL) || ((((o)->tt) == LUA_TBOOLEAN) && ((o)->value.b) == 0)); */
  return !l_isfalse(o);
}


extern const char *lua_tolstring (lua_State *L, int idx, size_t *len) {
  StkId o = index2adr(L, idx);
  /* if (!(((o)->tt) == LUA_TSTRING )) { */
  if (!ttisstring(o)) {
    lua_lock(L); /* ((void) 0); */ /* `luaV_tostring' may create a new string */
    if (!luaV_tostring(L, o)) { /* conversion failed? */
      if (len != NULL) {
        *len = 0;
      }
      lua_unlock(L); /* ((void) 0); */
      return NULL;
    }

    /* luaC_checkGC(L); */
    { 
		((void)0); 
		if ( G(L)->totalbytes >= G(L)->GCthreshold ) { 
			luaC_step(L); 
		}
	};

    o = index2adr(L, idx); /* previous call may reallocate the stack */
    lua_unlock(L); /* ((void) 0); */
  }
  if (len != NULL) {
    /* *len = (&(&(o)->value.gc->ts)->tsv)->len; */
    *len = tsvalue(o)->len;
  }
  /* return ((const char *)(((&(o)->value.gc->ts)) + 1)); */
  return svalue(o);
}


extern size_t lua_objlen (lua_State *L, int idx) {
  StkId o = index2adr(L, idx);
  switch (ttype(o)) {
    case LUA_TSTRING: { 
      /* return (&(&(o)->value.gc->ts)->tsv)->len; */
	  return tsvalue(o)->len;
    }
    case LUA_TUSERDATA: { 
      /* return (&(&(o)->value.gc->u)->uv)->len; */
      return uvalue(o)->len;
    }
    case LUA_TTABLE: { 
      /* return luaH_getn((&(o)->value.gc->h)); */
      return luaH_getn(hvalue(o));
    }
    case LUA_TNUMBER: {
      size_t l;
      lua_lock(L); /* ((void) 0); */ /* `luaV_tostring' may create a new string */
      /* l = (luaV_tostring(L, o) ? (&(&(o)->value.gc->ts)->tsv)->len : 0); */
      l = (luaV_tostring(L, o) ? tsvalue(o)->len : 0);
      lua_unlock(L); /* ((void) 0); */
      return l;
    }
    default: {
      return 0;
    }
  }
}


extern lua_CFunction lua_tocfunction (lua_State *L, int idx) {
  StkId o = index2adr(L, idx);
  /* return (!(((o)->tt) == LUA_TFUNCTION && (&(o)->value.gc->cl)->c.isC)) ? NULL : (&(o)->value.gc->cl)->c.f; */
  return (!iscfunction(o)) ? NULL : clvalue(o)->c.f;
}


extern void *lua_touserdata (lua_State *L, int idx) {
  StkId o = index2adr(L, idx);
  switch (ttype(o)) {
    case LUA_TUSERDATA: { 
      /* return ((&(o)->value.gc->u) + 1); */
      return (rawuvalue(o) + 1);
    }
    case LUA_TLIGHTUSERDATA: { 
      /* return ((o)->value.p); */
      return pvalue(o);
    }
    default: {
      return NULL;
    }
  }
}


extern lua_State *lua_tothread (lua_State *L, int idx) {
  StkId o = index2adr(L, idx);
  /* return (!(((o)->tt) == LUA_TTHREAD)) ? NULL : (&(o)->value.gc->th); */
  return (!ttisthread(o)) ? NULL : thvalue(o);
}


extern const void *lua_topointer (lua_State *L, int idx) {
  StkId o = index2adr(L, idx);
  switch (ttype(o)) {
    case LUA_TTABLE: { 
      /* return (&(o)->value.gc->h); */
      return hvalue(o);
    }
    case LUA_TFUNCTION: { 
      /* return (&(o)->value.gc->cl); */
      return clvalue(o);
    }
    case LUA_TTHREAD: { 
      /* return (&(o)->value.gc->th); */
      return thvalue(o);
    }
    case LUA_TUSERDATA:
    case LUA_TLIGHTUSERDATA: {
      return lua_touserdata(L, idx);
    }
    default: {
      return NULL;
    }
  }
}


/*
** push functions (C -> stack)
*/


extern void lua_pushnil (lua_State *L) {
  lua_lock(L); /* ((void) 0); */
  /* ((L->top)->tt=0); */
  setnilvalue(L->top);
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  lua_unlock(L); /*  ((void) 0); */
}


extern void lua_pushnumber (lua_State *L, lua_Number n) {
  lua_lock(L); /* ((void) 0); */
  /* { TValue *i_o=(L->top); i_o->value.n=(n); i_o->tt=LUA_TNUMBER; }; */
  setnvalue(L->top, n);
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_pushinteger (lua_State *L, lua_Integer n) {
  lua_lock(L); /* ((void) 0); */
  /* { TValue *i_o=(L->top); i_o->value.n=(((lua_Number)((n)))); i_o->tt=LUA_TNUMBER; }; */
  setnvalue(L->top, cast_num(n));
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_pushlstring (lua_State *L, const char *s, size_t len) {
  lua_lock(L); /* ((void) 0); */

  /* luaC_checkGC(L); */
  { 
	  ((void)0); 
	  if ( G(L)->totalbytes >= G(L)->GCthreshold ) {
		  luaC_step(L); 
	  }
  };

  /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((luaS_newlstr(L, s, len)))); i_o->tt=LUA_TSTRING; ((void)0); }; */
  setsvalue2s(L, L->top, luaS_newlstr(L, s, len));
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_pushstring (lua_State *L, const char *s) {
  if (s == NULL) {
    lua_pushnil(L);
  } else {
    lua_pushlstring(L, s, strlen(s));
  }
}


extern const char *lua_pushvfstring (lua_State *L, const char *fmt,
                                      va_list argp) {
  const char *ret;
  lua_lock(L); /* ((void) 0); */

  /* luaC_checkGC(L); */
  { 
	  ((void)0); 
	  if ( G(L)->totalbytes >= G(L)->GCthreshold ) {
		  luaC_step(L); 
	  }
  };

  ret = luaO_pushvfstring(L, fmt, argp);
  lua_unlock(L); /* ((void) 0); */
  return ret;
}


extern const char *lua_pushfstring (lua_State *L, const char *fmt, ...) {
  const char *ret;
  va_list argp;
  lua_lock(L); /* ((void) 0); */

  /* luaC_checkGC(L); */
  { 
	  ((void)0); 
	  if ( G(L)->totalbytes >= G(L)->GCthreshold ) {
		  luaC_step(L); 
	  }
  };

  va_start(argp, fmt);
  ret = luaO_pushvfstring(L, fmt, argp);
  va_end(argp);
  lua_unlock(L); /* ((void) 0); */
  return ret;
}


extern void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n) {
  Closure *cl;
  lua_lock(L); /* ((void) 0); */

  /* luaC_checkGC(L); */
  { 
	  ((void)0); 
	  if ( G(L)->totalbytes >= G(L)->GCthreshold ) {
		  luaC_step(L); 
	  }
  };

  api_checknelems(L, n); /* { (void)L; }; */
  cl = luaF_newCclosure(L, n, getcurrenv(L));
  cl->c.f = fn;
  L->top -= n;
  while (n--) {
    /* { const TValue *o2=(L->top+n); TValue *o1=(&cl->c.upvalue[n]); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
    setobj2n(L, &cl->c.upvalue[n], L->top+n);
  }
  /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((cl))); i_o->tt=LUA_TFUNCTION; ((void)0); }; */
  setclvalue(L, L->top, cl);
  lua_assert(iswhite(obj2gco(cl))); /* ((void)0); */
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_pushboolean (lua_State *L, int b) {
  lua_lock(L); /* ((void) 0); */
  /* { TValue *i_o=(L->top); i_o->value.b=((b != 0)); i_o->tt = LUA_TBOOLEAN; }; */ /* ensure that true is 1 */
  setbvalue(L->top, (b != 0));  /* ensure that true is 1 */
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_pushlightuserdata (lua_State *L, void *p) {
  lua_lock(L); /* ((void) 0); */
  /* { TValue *i_o=(L->top); i_o->value.p=(p); i_o->tt=LUA_TLIGHTUSERDATA; }; */
  setpvalue(L->top, p);
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  lua_unlock(L); /* ((void) 0); */
}


extern int lua_pushthread (lua_State *L) {
  lua_lock(L); /* ((void) 0); */
  /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((L))); i_o->tt=LUA_TTHREAD; ((void)0); }; */
  setthvalue(L, L->top, L);
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  lua_unlock(L); /* ((void) 0); */
  /* return ((L->l_G)->mainthread == L); */
  return (G(L)->mainthread == L);
}


/*
** get functions (Lua -> stack)
*/


extern void lua_gettable (lua_State *L, int idx) {
  StkId t;
  lua_lock(L); /* ((void) 0); */
  t = index2adr(L, idx);
  api_checkvalidindex(L, t); /* { (void)L; }; */
  luaV_gettable(L, t, L->top - 1, L->top - 1);
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_getfield (lua_State *L, int idx, const char *k) {
  StkId t;
  TValue key;
  lua_lock(L); /* ((void) 0); */
  t = index2adr(L, idx);
  api_checkvalidindex(L, t); /* { (void)L; }; */
  /* { TValue *i_o=(&key); i_o->value.gc=((GCObject *)(((luaS_newlstr(L, k, strlen(k)))))); i_o->tt=LUA_TSTRING; ((void)0); }; */
  setsvalue(L, &key, luaS_new(L, k));
  luaV_gettable(L, t, &key, L->top);
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_rawget (lua_State *L, int idx) {
  StkId t;
  lua_lock(L); /* ((void) 0); */
  t = index2adr(L, idx);
  api_check(L, ttistable(t)); /* { (void)L; }; */
  /* { const TValue *o2=(luaH_get((&(t)->value.gc->h), L->top - 1)); TValue *o1=(L->top - 1); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
  setobj2s(L, L->top - 1, luaH_get(hvalue(t), L->top - 1));
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_rawgeti (lua_State *L, int idx, int n) {
  StkId o;
  lua_lock(L); /* ((void) 0); */
  o = index2adr(L, idx);
  api_check(L, ttistable(o)); /* { (void)L; }; */
  /* { const TValue *o2=(luaH_getnum((&(o)->value.gc->h), n)); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
  setobj2s(L, L->top, luaH_getnum(hvalue(o), n));
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_createtable (lua_State *L, int narray, int nrec) {
  lua_lock(L); /* ((void) 0); */

  /* luaC_checkGC(L); */
  { 
	  ((void)0); 
	  if ( G(L)->totalbytes >= G(L)->GCthreshold ) {
		  luaC_step(L); 
	  }
  };

  /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((luaH_new(L, narray, nrec)))); i_o->tt=LUA_TTABLE; ((void)0); }; */
  sethvalue(L, L->top, luaH_new(L, narray, nrec));
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  lua_unlock(L); /* ((void) 0); */
}


extern int lua_getmetatable (lua_State *L, int objindex) {
  const TValue *obj;
  Table *mt = NULL;
  int res;
  lua_lock(L); /* ((void) 0); */
  obj = index2adr(L, objindex);
  switch (ttype(obj)) {
    case LUA_TTABLE: {
      /* mt = (&(obj)->value.gc->h)->metatable; */
      mt = hvalue(obj)->metatable;
      break;
    }
    case LUA_TUSERDATA: {
      /* mt = (&(&(obj)->value.gc->u)->uv)->metatable; */
      mt = uvalue(obj)->metatable;
      break;
    }
    default: {
      /* mt = (L->l_G)->mt[((obj)->tt)]; */
      mt = G(L)->mt[ttype(obj)];
      break;
    }
  }
  if (mt == NULL) {
    res = 0;
  } else {
    /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((mt))); i_o->tt=LUA_TTABLE; ((void)0); }; */
    sethvalue(L, L->top, mt);
    /* {{ (void)L; }; L->top++;}; */
    api_incr_top(L);
    res = 1;
  }
  lua_unlock(L); /* ((void) 0); */
  return res;
}


extern void lua_getfenv (lua_State *L, int idx) {
  StkId o;
  lua_lock(L); /* ((void) 0); */
  o = index2adr(L, idx);
  api_checkvalidindex(L, o); /* { (void)L; }; */
  switch (ttype(o)) {
    case LUA_TFUNCTION: {
      /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)(((&(o)->value.gc->cl)->c.env))); i_o->tt=LUA_TTABLE; ((void)0); }; */
      sethvalue(L, L->top, clvalue(o)->c.env);
      break;
    }
    case LUA_TUSERDATA: {
      /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)(((&(&(o)->value.gc->u)->uv)->env))); i_o->tt=LUA_TTABLE; ((void)0); }; */
      sethvalue(L, L->top, uvalue(o)->env);
      break;
    }
    case LUA_TTHREAD: {
      /* { const TValue *o2=((&(&(o)->value.gc->th)->l_gt)); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
      setobj2s(L, L->top,  gt(thvalue(o)));
      break;
    }
    default: {
      /* ((L->top)->tt=0); */
      setnilvalue(L->top);
      break;
    }
  }
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  lua_unlock(L); /* ((void) 0); */
}


/*
** set functions (stack -> Lua)
*/


extern void lua_settable (lua_State *L, int idx) {
  StkId t;
  lua_lock(L); /* ((void) 0); */
  api_checknelems(L, 2); /* { (void)L; }; */
  t = index2adr(L, idx);
  api_checkvalidindex(L, t); /* { (void)L; }; */
  luaV_settable(L, t, L->top - 2, L->top - 1);
  L->top -= 2; /* pop index and value */
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_setfield (lua_State *L, int idx, const char *k) {
  StkId t;
  TValue key;
  lua_lock(L); /* ((void) 0); */
  api_checknelems(L, 1); /* { (void)L; }; */
  t = index2adr(L, idx);
  api_checkvalidindex(L, t); /* { (void)L; }; */
  /* { TValue *i_o=(&key); i_o->value.gc=((GCObject *)(((luaS_newlstr(L, k, strlen(k)))))); i_o->tt=LUA_TSTRING; ((void)0); }; */
  setsvalue(L, &key, luaS_new(L, k));
  luaV_settable(L, t, &key, L->top - 1);
  L->top--; /* pop value */
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_rawset (lua_State *L, int idx) {
  StkId t;
  lua_lock(L); /* ((void) 0); */
  api_checknelems(L, 2); /* { (void)L; }; */
  t = index2adr(L, idx);
  api_check(L, ttistable(t)); /* { (void)L; }; */
  /* { const TValue *o2=(L->top-1); TValue *o1=(luaH_set(L, (&(t)->value.gc->h), L->top-2)); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
  setobj2t(L, luaH_set(L, hvalue(t), L->top-2), L->top-1);


  /* luaC_barriert(L, hvalue(t), L->top-1); */
  { 
	  if ( ( ( (L->top-1)->tt >= LUA_TSTRING ) && ( ( ((L->top-1)->value.gc)->gch.marked ) & ( (1<<WHITE0BIT) | (1<<WHITE1BIT) ) ) ) && ( ( ( (GCObject *)( &(t)->value.gc->h ) )->gch.marked) & (1<<BLACKBIT) )  ) {
		  luaC_barrierback(L, (&(t)->value.gc->h) ); 
	  }
  };

  L->top -= 2;
  lua_unlock(L); /* ((void) 0); */
}


extern void lua_rawseti (lua_State *L, int idx, int n) {
  StkId o;
  lua_lock(L); /* ((void) 0); */
  api_checknelems(L, 1); /* { (void)L; }; */
  o = index2adr(L, idx);
  api_check(L, ttistable(o)); /* { (void)L; }; */
  /* { const TValue *o2=(L->top-1); TValue *o1=(luaH_setnum(L, (&(o)->value.gc->h), n)); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
  setobj2t(L, luaH_setnum(L, hvalue(o), n), L->top-1);


  /* luaC_barriert(L, hvalue(o), L->top-1); */
  { 
	  if (  ( ((L->top-1)->tt) >= LUA_TSTRING && ( ( ( (L->top-1)->value.gc )->gch.marked ) & ( (1<<WHITE0BIT) | (1<<WHITE1BIT) ) ) ) && ( ( ( (GCObject *)( &(o)->value.gc->h  ) )->gch.marked ) & ( 1<< BLACKBIT ) )  ) {
		  luaC_barrierback(L, (&(o)->value.gc->h) ); 
	  }
  };

  L->top--;
  lua_unlock(L); /* ((void) 0); */
}


extern int lua_setmetatable (lua_State *L, int objindex) {
  TValue *obj;
  Table *mt;
  lua_lock(L); /* ((void) 0); */
  api_checknelems(L, 1); /* { (void)L; }; */
  obj = index2adr(L, objindex);
  api_checkvalidindex(L, obj); /* { (void)L; }; */
  if (ttisnil(L->top - 1)) {
    mt = NULL;
  } else {
    api_check(L, ttistable(L->top - 1)); /* { (void)L; }; */
    /* mt = (&(L->top - 1)->value.gc->h); */
    mt = hvalue(L->top - 1);
  }
  switch (ttype(obj)) {
    case LUA_TTABLE: {
      /* (&(obj)->value.gc->h)->metatable = mt; */
      hvalue(obj)->metatable = mt;
      if (mt) {

        /* luaC_objbarriert(L, hvalue(obj), mt); */
        { 
			if ( ( ( ( (GCObject *)(mt) )->gch.marked) &  ( (1<< WHITE0BIT ) | (1<<WHITE1BIT ) ) ) && (  ( ( (GCObject *)(&(obj)->value.gc->h) )->gch.marked) & (1<<BLACKBIT) )  ) { 
				luaC_barrierback(L,(&(obj)->value.gc->h)); 
			}
		};

      }
      break;
    }
    case LUA_TUSERDATA: {
      /* (&(&(obj)->value.gc->u)->uv)->metatable = mt; */
      uvalue(obj)->metatable = mt;
      if (mt) {

        /* luaC_objbarrier(L, rawuvalue(obj), mt); */
        { 
			if ( ( ( ( (GCObject *)(mt) )->gch.marked ) & ( ( (1<<WHITE0BIT) | (1<<WHITE1BIT) )) ) && ( ( ( (GCObject *)( (&(obj)->value.gc->u) ) )->gch.marked) & (1<<BLACKBIT) ) ) { 
				luaC_barrierf(L,( (GCObject *)( &(obj)->value.gc->u) ),( (GCObject *)(mt) ) ); 
			}
		};

      }
      break;
    }
    default: {
      /* (L->l_G)->mt[((obj)->tt)] = mt; */
      G(L)->mt[ttype(obj)] = mt;
      break;
    }
  }
  L->top--;
  lua_unlock(L); /* ((void) 0); */
  return 1;
}


extern int lua_setfenv (lua_State *L, int idx) {
  StkId o;
  int res = 1;
  lua_lock(L); /* ((void) 0); */
  api_checknelems(L, 1); /* { (void)L; }; */
  o = index2adr(L, idx);
  api_checkvalidindex(L, o); /* { (void)L; }; */
  api_check(L, ttistable(L->top - 1)); /* { (void)L; }; */
  switch (ttype(o)) {
    case LUA_TFUNCTION: {
      /* (&(o)->value.gc->cl)->c.env = (&(L->top - 1)->value.gc->h); */
      clvalue(o)->c.env = hvalue(L->top - 1);
      break;
    }
    case LUA_TUSERDATA: {
      /* (&(&(o)->value.gc->u)->uv)->env = (&(L->top - 1)->value.gc->h); */
      uvalue(o)->env = hvalue(L->top - 1);
      break;
    }
    case LUA_TTHREAD: {
      /* { TValue *i_o=((&(&(o)->value.gc->th)->l_gt)); i_o->value.gc=((GCObject *)(((&(L->top - 1)->value.gc->h)))); i_o->tt=LUA_TTABLE; ((void)0); }; */
      sethvalue(L, gt(thvalue(o)), hvalue(L->top - 1));
      break;
    }
    default: {
      res = 0;
      break;
    }
  }
  if (res) {
    /* luaC_objbarrier(L, gcvalue(o), hvalue(L->top - 1)); */
    { 
		if (((((((GCObject *)(((&(L->top - 1)->value.gc->h))))))->gch.marked) & ((((1<<(0)) | (1<<(1)))))) && ((((((GCObject *)((((o)->value.gc))))))->gch.marked) & ((1<<(2))))) {
			luaC_barrierf(L, ( (GCObject *)( (o)->value.gc ) ), ( (GCObject *)((&(L->top - 1)->value.gc->h))) ); 
		}
	};

  }
  L->top--;
  lua_unlock(L); /* ((void) 0); */
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
  lua_lock(L); /* ((void) 0); */
  api_checknelems(L, nargs+1); /* { (void)L; }; */
  checkresults(L, nargs, nresults); /* { (void)L; }; */
  func = L->top - (nargs+1);
  luaD_call(L, func, nresults);

  /* adjustresults(L, nresults); */
  { 
	  if (nresults == LUA_MULTRET && L->top >= L->ci->top) {
		  L->ci->top = L->top; 
	  }
  };

  lua_unlock(L); /* ((void) 0); */
}


/*
** Execute a protected call.
*/
struct CallS { /* data to `f_call' */
  StkId func;
  int nresults;
};


static void f_call (lua_State *L, void *ud) {
  /* struct CallS *c = ((struct CallS *)(ud)); */
  struct CallS *c = cast(struct CallS *, ud);
  luaD_call(L, c->func, c->nresults);
}


extern int lua_pcall (lua_State *L, int nargs, int nresults, int errfunc) {
  struct CallS c;
  int status;
  ptrdiff_t func;
  lua_lock(L); /* ((void) 0); */
  api_checknelems(L, nargs+1); /* { (void)L; }; */
  checkresults(L, nargs, nresults); /* { (void)L; }; */
  if (errfunc == 0) {
    func = 0;
  } else {
    StkId o = index2adr(L, errfunc);
    api_checkvalidindex(L, o); /* { (void)L; }; */
    /* func = ((char *)(o) - (char *)L->stack); */
    func = savestack(L, o);
  }
  c.func = L->top - (nargs+1); /* function to be called */
  c.nresults = nresults;
  /* status = luaD_pcall(L, f_call, &c, ((char *)(c.func) - (char *)L->stack), func); */
  status = luaD_pcall(L, f_call, &c, savestack(L, c.func), func);

  /* adjustresults(L, nresults); */
  { 
	  if (nresults == (-1) && L->top >= L->ci->top) {
		  L->ci->top = L->top; 
	  }
  };

  lua_unlock(L); /* ((void) 0); */
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
  /* struct CCallS *c = ((struct CCallS *)(ud)); */
  struct CCallS *c = cast(struct CCallS *, ud);
  Closure *cl;
  cl = luaF_newCclosure(L, 0, getcurrenv(L));
  cl->c.f = c->func;
  /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((cl))); i_o->tt=LUA_TFUNCTION; ((void)0); }; */ /* push function */
  setclvalue(L, L->top, cl);  /* push function */
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  /* { TValue *i_o=(L->top); i_o->value.p=(c->ud); i_o->tt=LUA_TLIGHTUSERDATA; }; */ /* push only argument */
  setpvalue(L->top, c->ud);  /* push only argument */
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  luaD_call(L, L->top - 2, 0);
}


extern int lua_cpcall (lua_State *L, lua_CFunction func, void *ud) {
  struct CCallS c;
  int status;
  lua_lock(L); /* ((void) 0); */
  c.func = func;
  c.ud = ud;
  // luaD_pcall(...)
  /* status = luaD_pcall(L, f_Ccall, &c, ((char *)(L->top) - (char *)L->stack), 0); */
  status = luaD_pcall(L, f_Ccall, &c, savestack(L, L->top), 0);
  lua_unlock(L); /* ((void) 0); */
  return status;
}


extern int lua_load (lua_State *L, lua_Reader reader, void *data,
                      const char *chunkname) {
  ZIO z;
  int status;
  lua_lock(L); /* ((void) 0); */
  if (!chunkname) {
    chunkname = "?";
  }
  luaZ_init(L, &z, reader, data);
  status = luaD_protectedparser(L, &z, chunkname);
  lua_unlock(L); /* ((void) 0); */
  return status;
}


extern int lua_dump (lua_State *L, lua_Writer writer, void *data) {
  int status;
  TValue *o;
  lua_lock(L); /* ((void) 0); */
  api_checknelems(L, 1); /* { (void)L; }; */
  o = L->top - 1;
  /* if ((((o)->tt) == LUA_TFUNCTION && !(&(o)->value.gc->cl)->c.isC)) { */
  if (isLfunction(o)) {
    /* status = luaU_dump(L, (&(o)->value.gc->cl)->l.p, writer, data, 0); */
    status = luaU_dump(L, clvalue(o)->l.p, writer, data, 0);
  }
  else {
    status = 1;
  }
  lua_unlock(L); /* ((void) 0); */
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
  lua_lock(L); /* ((void) 0); */
  /* g = (L->l_G); */
  g = G(L);
  switch (what) {
    case LUA_GCSTOP: {
      /* g->GCthreshold = ((lu_mem)(~(lu_mem)0)-2); */
      g->GCthreshold = MAX_LUMEM;
      break;
    }
    case LUA_GCRESTART: {
      g->GCthreshold = g->totalbytes;
      break;
    }
    case LUA_GCCOLLECT: {
      luaC_fullgc(L);
      break;
    }
    case LUA_GCCOUNT: {
      /* GC values are expressed in Kbytes: #bytes/2^10 */
      /* res = ((int)((g->totalbytes >> 10))); */
      res = cast_int(g->totalbytes >> 10);
      break;
    }
    case LUA_GCCOUNTB: {
      /* res = ((int)((g->totalbytes & 0x3ff))); */
      res = cast_int(g->totalbytes & 0x3ff);
      break;
    }
    case LUA_GCSTEP: {
      /* lu_mem a = (((lu_mem)(data)) << 10); */
      lu_mem a = (cast(lu_mem, data) << 10);
      if (a <= g->totalbytes) {
        g->GCthreshold = g->totalbytes - a;
      } else {
        g->GCthreshold = 0;
      }
      while (g->GCthreshold <= g->totalbytes) {
        luaC_step(L);
        if (g->gcstate == GCSpause) { /* end of cycle? */
          res = 1; /* signal it */
          break;
        }
      }
      break;
    }
    case LUA_GCSETPAUSE: {
      res = g->gcpause;
      g->gcpause = data;
      break;
    }
    case LUA_GCSETSTEPMUL: {
      res = g->gcstepmul;
      g->gcstepmul = data;
      break;
    }
    default: {
      res = -1; /* invalid option */
    }
  }
  lua_unlock(L); /* ((void) 0); */
  return res;
}



/*
** miscellaneous functions
*/


extern int lua_error (lua_State *L) {
  lua_lock(L); /* ((void) 0); */
  api_checknelems(L, 1); /* { (void)L; }; */
  luaG_errormsg(L);
  lua_unlock(L); /* ((void) 0); */
  return 0; /* to avoid warnings */
}


extern int lua_next (lua_State *L, int idx) {
  StkId t;
  int more;
  lua_lock(L); /* ((void) 0); */
  t = index2adr(L, idx);
  api_check(L, ttistable(t)); /* { (void)L; }; */
  /* more = luaH_next(L, (&(t)->value.gc->h), L->top - 1); */
  more = luaH_next(L, hvalue(t), L->top - 1);
  if (more) {
    /* {{ (void)L; }; L->top++;}; */
	api_incr_top(L);
  }
  else { /* no more elements */
    L->top -= 1; /* remove key */
  }
  lua_unlock(L); /* ((void) 0); */
  return more;
}


extern void lua_concat (lua_State *L, int n) {
  lua_lock(L); /* ((void) 0); */
  api_checknelems(L, n); /* { (void)L; }; */
  if (n >= 2) {
    /* luaC_checkGC(L); */
    { 
		((void)0); 
		if ( G(L)->totalbytes >= G(L)->GCthreshold) {
			luaC_step(L); 
		}
	};

    /* luaV_concat(L, n, ((int)((L->top - L->base))) - 1); */
    luaV_concat(L, n, cast_int(L->top - L->base) - 1);
    L->top -= (n-1);
  }
  else if (n == 0) { /* push empty string */
    /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((luaS_newlstr(L, "", 0)))); i_o->tt=LUA_TSTRING; ((void)0); }; */
    setsvalue2s(L, L->top, luaS_newlstr(L, "", 0));
    /* {{ (void)L; }; L->top++;}; */
	api_incr_top(L);
  }
  /* else n == 1; nothing to do */
  lua_unlock(L); /* ((void) 0); */
}


extern lua_Alloc lua_getallocf (lua_State *L, void **ud) {
  lua_Alloc f;
  lua_lock(L); /* ((void) 0); */
  if (ud) {
    /* *ud = (L->l_G)->ud; */
    *ud = G(L)->ud;
  }
  /* f = (L->l_G)->frealloc; */
  f = G(L)->frealloc;
  lua_unlock(L); /* ((void) 0); */
  return f;
}


extern void lua_setallocf (lua_State *L, lua_Alloc f, void *ud) {
  lua_lock(L); /* ((void) 0); */
  /* (L->l_G)->ud = ud; */
  G(L)->ud = ud;
  /* (L->l_G)->frealloc = f; */
  G(L)->frealloc = f;
  ((void) 0);
}


extern void *lua_newuserdata (lua_State *L, size_t size) {
  Udata *u;
  lua_lock(L); /* ((void) 0); */

  /* luaC_checkGC(L); */
  { 
	  ((void)0); 
	  if ( G(L)->totalbytes >= G(L)->GCthreshold) {
		  luaC_step(L); 
	  }
  };

  u = luaS_newudata(L, size, getcurrenv(L));
  /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((u))); i_o->tt=LUA_TUSERDATA; ((void)0); }; */
  setuvalue(L, L->top, u);
  /* {{ (void)L; }; L->top++;}; */
  api_incr_top(L);
  lua_unlock(L); /* ((void) 0); */
  return u + 1;
}


static const char *aux_upvalue (StkId fi, int n, TValue **val) {
  Closure *f;
  /* if (!(((fi)->tt) == LUA_TFUNCTION)) { */
  if (!ttisfunction(fi)) { 
    return NULL;
  }
  /* f = (&(fi)->value.gc->cl); */
  f = clvalue(fi);
  if (f->c.isC) {
    if (!(1 <= n && n <= f->c.nupvalues)) {
      return NULL;
    }
    *val = &f->c.upvalue[n-1];
    return "";
  }
  else {
    Proto *p = f->l.p;
    if (!(1 <= n && n <= p->sizeupvalues)) {
      return NULL;
    }
    *val = f->l.upvals[n-1]->v;
    /* return ((const char *)((p->upvalues[n-1]) + 1)); */
    return getstr(p->upvalues[n-1]);
  }
}


extern const char *lua_getupvalue (lua_State *L, int funcindex, int n) {
  const char *name;
  TValue *val;
  lua_lock(L); /* ((void) 0); */
  name = aux_upvalue(index2adr(L, funcindex), n, &val);
  if (name) {
    /* { const TValue *o2=(val); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
    setobj2s(L, L->top, val);
    /* {{ (void)L; }; L->top++;}; */
	api_incr_top(L);
  }
  lua_unlock(L); /* ((void) 0); */
  return name;
}


extern const char *lua_setupvalue (lua_State *L, int funcindex, int n) {
  const char *name;
  TValue *val;
  StkId fi;
  lua_lock(L); /* ((void) 0); */
  fi = index2adr(L, funcindex);
  api_checknelems(L, 1); /* { (void)L; }; */
  name = aux_upvalue(fi, n, &val);
  if (name) {
    L->top--;
    /* { const TValue *o2=(L->top); TValue *o1=(val); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
    setobj(L, val, L->top);

    /* luaC_barrier(L, clvalue(fi), L->top); */
    { 
		if ( ( (((L->top)->tt) >= LUA_TSTRING) && ( ( ( (L->top)->value.gc )->gch.marked ) & ( (1<<WHITE0BIT) | (1<<WHITE1BIT) ) ) ) && ( (  ( (GCObject *)( (&(fi)->value.gc->cl) ) )->gch.marked ) & (1<<BLACKBIT) ) ) {
			luaC_barrierf(L, ( (GCObject *)(&(fi)->value.gc->cl) ),( (L->top)->value.gc ) ); 
		}
	};

  }
  lua_unlock(L); /* ((void) 0); */
  return name;
}


