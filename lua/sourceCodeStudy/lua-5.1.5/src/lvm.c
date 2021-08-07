/*
** $Id: lvm.c,v 2.63.1.5 2011/08/17 20:43:11 roberto Exp $
** Lua virtual machine
** See Copyright Notice in lua.h
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define lvm_c
#define LUA_CORE

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"



/* limit for table tag-method chains (to avoid loops) */
#define MAXTAGLOOP	100


const TValue *luaV_tonumber (const TValue *obj, TValue *n) {
  lua_Number num;
  if ((((obj)->tt) == 3)) {
    return obj;
  }
  if ((((obj)->tt) == 4) && luaO_str2d(((const char *)(((&(obj)->value.gc->ts)) + 1)), &num)) {
    { TValue *i_o=(n); i_o->value.n=(num); i_o->tt=3; };
    return n;
  }
  else {
    return ((void *)0);
  }
}


int luaV_tostring (lua_State *L, StkId obj) {
  if (!(((obj)->tt) == 3)) {
    return 0;
  } else {
    char s[32];
    lua_Number n = ((obj)->value.n);
    sprintf((s), "%.14g", (n));
    { TValue *i_o=(obj); i_o->value.gc=((GCObject *)(((luaS_newlstr(L, s, strlen(s)))))); i_o->tt=4; ((void)0); };
    return 1;
  }
}


static void traceexec (lua_State *L, const Instruction *pc) {
  lu_byte mask = L->hookmask;
  const Instruction *oldpc = L->savedpc;
  L->savedpc = pc;
  if ((mask & (1 << 3)) && L->hookcount == 0) {
    (L->hookcount = L->basehookcount);
    luaD_callhook(L, 3, -1);
  }
  if (mask & (1 << 2)) {
    Proto *p = ((&((L->ci)->func)->value.gc->cl))->l.p;
    int npc = (((int)((pc) - (p)->code)) - 1);
    int newline = (((p)->lineinfo) ? (p)->lineinfo[npc] : 0);
    /* call linehook when enter a new function, when jump back (loop),
       or when enter a new line */
    if (npc == 0 || pc <= oldpc || newline != (((p)->lineinfo) ? (p)->lineinfo[(((int)((oldpc) - (p)->code)) - 1)] : 0)) {
      luaD_callhook(L, 2, newline);
    }
  }
}


static void callTMres (lua_State *L, StkId res, const TValue *f,
                        const TValue *p1, const TValue *p2) {
  ptrdiff_t result = ((char *)(res) - (char *)L->stack);
  { const TValue *o2=(f); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; /* push function */
  { const TValue *o2=(p1); TValue *o1=(L->top+1); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; /* 1st argument */
  { const TValue *o2=(p2); TValue *o1=(L->top+2); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; /* 2nd argument */
  if ((char *)L->stack_last - (char *)L->top <= (3)*(int)sizeof(TValue)) luaD_growstack(L, 3); else ((void)0);;
  L->top += 3;
  luaD_call(L, L->top - 3, 1);
  res = ((TValue *)((char *)L->stack + (result)));
  L->top--;
  { const TValue *o2=(L->top); TValue *o1=(res); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
}


static void callTM (lua_State *L, const TValue *f, const TValue *p1,
                    const TValue *p2, const TValue *p3) {
  { const TValue *o2=(f); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; /* push function */
  { const TValue *o2=(p1); TValue *o1=(L->top+1); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; /* 1st argument */
  { const TValue *o2=(p2); TValue *o1=(L->top+2); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; /* 2nd argument */
  { const TValue *o2=(p3); TValue *o1=(L->top+3); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; /* 3th argument */
  if ((char *)L->stack_last - (char *)L->top <= (4)*(int)sizeof(TValue)) luaD_growstack(L, 4); else ((void)0);;
  L->top += 4;
  luaD_call(L, L->top - 4, 0);
}


void luaV_gettable (lua_State *L, const TValue *t, TValue *key, StkId val) {
  int loop;
  for (loop = 0; loop < 100; loop++) {
    const TValue *tm;
    if ((((t)->tt) == 5)) { /* `t' is a table? */
      Table *h = (&(t)->value.gc->h);
      const TValue *res = luaH_get(h, key); /* do a primitive get */
      if (!(((res)->tt) == 0) || /* result is no nil? */
          (tm = ((h->metatable) == ((void *)0) ? ((void *)0) : ((h->metatable)->flags & (1u<<(TM_INDEX))) ? ((void *)0) : luaT_gettm(h->metatable, TM_INDEX, ((L->l_G))->tmname[TM_INDEX]))) == ((void *)0))
      { /* or no TM? */
        { const TValue *o2=(res); TValue *o1=(val); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
        return;
      }
      /* else will try the tag method */
    }
    else if ((((tm = luaT_gettmbyobj(L, t, TM_INDEX))->tt) == 0)) {
      luaG_typeerror(L, t, "index");
    }
    if ((((tm)->tt) == 6)) {
      callTMres(L, val, tm, t, key);
      return;
    }
    t = tm; /* else repeat with `tm' */
  }
  luaG_runerror(L, "loop in gettable");
}


void luaV_settable (lua_State *L, const TValue *t, TValue *key, StkId val) {
  int loop;
  TValue temp;
  for (loop = 0; loop < 100; loop++) {
    const TValue *tm;
    if ((((t)->tt) == 5)) { /* `t' is a table? */
      Table *h = (&(t)->value.gc->h);
      TValue *oldval = luaH_set(L, h, key); /* do a primitive set */
      if (!(((oldval)->tt) == 0) || /* result is no nil? */
          (tm = ((h->metatable) == ((void *)0) ? ((void *)0) : ((h->metatable)->flags & (1u<<(TM_NEWINDEX))) ? ((void *)0) : luaT_gettm(h->metatable, TM_NEWINDEX, ((L->l_G))->tmname[TM_NEWINDEX]))) == ((void *)0))
      { /* or no TM? */
        { const TValue *o2=(val); TValue *o1=(oldval); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
        h->flags = 0;
        { if (((((val)->tt) >= 4) && (((((val)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) && ((((((GCObject *)((h)))))->gch.marked) & ((1<<(2))))) luaC_barrierback(L,h); };
        return;
      }
      /* else will try the tag method */
    }
    else if ((((tm = luaT_gettmbyobj(L, t, TM_NEWINDEX))->tt) == 0)) {
      luaG_typeerror(L, t, "index");
    }
    if ((((tm)->tt) == 6)) {
      callTM(L, tm, t, key, val);
      return;
    }
    /* else repeat with `tm' */
    { const TValue *o2=(tm); TValue *o1=(&temp); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; /* avoid pointing inside table (may rehash) */
    t = &temp;
  }
  luaG_runerror(L, "loop in settable");
}


static int call_binTM (lua_State *L, const TValue *p1, const TValue *p2,
                       StkId res, TMS event) {
  const TValue *tm = luaT_gettmbyobj(L, p1, event); /* try first operand */
  if ((((tm)->tt) == 0)) {
    tm = luaT_gettmbyobj(L, p2, event); /* try second operand */
  }
  if ((((tm)->tt) == 0)) {
    return 0;
  }
  callTMres(L, res, tm, p1, p2);
  return 1;
}


static const TValue *get_compTM (lua_State *L, Table *mt1, Table *mt2,
                                  TMS event) {
  const TValue *tm1 = ((mt1) == ((void *)0) ? ((void *)0) : ((mt1)->flags & (1u<<(event))) ? ((void *)0) : luaT_gettm(mt1, event, ((L->l_G))->tmname[event]));
  const TValue *tm2;
  if (tm1 == ((void *)0)) {
    return ((void *)0); /* no metamethod */
  }
  if (mt1 == mt2) {
    return tm1; /* same metatables => same metamethods */
  }
  tm2 = ((mt2) == ((void *)0) ? ((void *)0) : ((mt2)->flags & (1u<<(event))) ? ((void *)0) : luaT_gettm(mt2, event, ((L->l_G))->tmname[event]));
  if (tm2 == ((void *)0)) {
    return ((void *)0); /* no metamethod */
  }
  if (luaO_rawequalObj(tm1, tm2)) { /* same metamethods? */
    return tm1;
  }
  return ((void *)0);
}


static int call_orderTM (lua_State *L, const TValue *p1, const TValue *p2,
                         TMS event) {
  const TValue *tm1 = luaT_gettmbyobj(L, p1, event);
  const TValue *tm2;
  if ((((tm1)->tt) == 0)) {
    return -1; /* no metamethod? */
  }
  tm2 = luaT_gettmbyobj(L, p2, event);
  if (!luaO_rawequalObj(tm1, tm2)) { /* different metamethods? */
    return -1;
  }
  callTMres(L, L->top, tm1, p1, p2);
  return !((((L->top)->tt) == 0) || ((((L->top)->tt) == 1) && ((L->top)->value.b) == 0));
}


static int l_strcmp (const TString *ls, const TString *rs) {
  const char *l = ((const char *)((ls) + 1));
  size_t ll = ls->tsv.len;
  const char *r = ((const char *)((rs) + 1));
  size_t lr = rs->tsv.len;
  for (;;) {
    int temp = strcoll(l, r);
    if (temp != 0) {
      return temp;
    }
    else { /* strings are equal up to a `\0' */
      size_t len = strlen(l); /* index of first `\0' in both strings */
      if (len == lr) { /* r is finished? */
        return (len == ll) ? 0 : 1;
      } else if (len == ll) { /* l is finished? */
        return -1; /* l is smaller than r (because r is not finished) */
      }
      /* both strings longer than `len'; go on comparing (after the `\0') */
      len++;
      l += len; ll -= len; r += len; lr -= len;
    }
  }
}


int luaV_lessthan (lua_State *L, const TValue *l, const TValue *r) {
  int res;
  if (((l)->tt) != ((r)->tt)) {
    return luaG_ordererror(L, l, r);
  } else if ((((l)->tt) == 3)) {
    return ((((l)->value.n))<(((r)->value.n)));
  } else if ((((l)->tt) == 4)) {
    return l_strcmp((&(l)->value.gc->ts), (&(r)->value.gc->ts)) < 0;
  } else if ((res = call_orderTM(L, l, r, TM_LT)) != -1) {
    return res;
  }
  return luaG_ordererror(L, l, r);
}


static int lessequal (lua_State *L, const TValue *l, const TValue *r) {
  int res;
  if (((l)->tt) != ((r)->tt)) {
    return luaG_ordererror(L, l, r);
  } else if ((((l)->tt) == 3)) {
    return ((((l)->value.n))<=(((r)->value.n)));
  } else if ((((l)->tt) == 4)) {
    return l_strcmp((&(l)->value.gc->ts), (&(r)->value.gc->ts)) <= 0;
  } else if ((res = call_orderTM(L, l, r, TM_LE)) != -1) { /* first try `le' */
    return res;
  } else if ((res = call_orderTM(L, r, l, TM_LT)) != -1) { /* else try `lt' */
    return !res;
  }
  return luaG_ordererror(L, l, r);
}


int luaV_equalval (lua_State *L, const TValue *t1, const TValue *t2) {
  const TValue *tm;
  ((void)0);
  switch (((t1)->tt)) {
    case 0: {
      return 1;
    }
    case 3: {
      return ((((t1)->value.n))==(((t2)->value.n)));
    }
    case 1: {
      return ((t1)->value.b) == ((t2)->value.b); /* true must be 1 !! */
    }
    case 2: {
      return ((t1)->value.p) == ((t2)->value.p);
    }
    case 7: {
      if ((&(&(t1)->value.gc->u)->uv) == (&(&(t2)->value.gc->u)->uv)) {
        return 1;
      }
      tm = get_compTM(L, (&(&(t1)->value.gc->u)->uv)->metatable, (&(&(t2)->value.gc->u)->uv)->metatable,
                         TM_EQ);
      break; /* will try TM */
    }
    case 5: {
      if ((&(t1)->value.gc->h) == (&(t2)->value.gc->h)) {
        return 1;
      }
      tm = get_compTM(L, (&(t1)->value.gc->h)->metatable, (&(t2)->value.gc->h)->metatable, TM_EQ);
      break; /* will try TM */
    }
    default: {
      return ((t1)->value.gc) == ((t2)->value.gc);
    }
  }
  if (tm == ((void *)0)) {
    return 0; /* no TM? */
  }
  callTMres(L, L->top, tm, t1, t2); /* call TM */
  return !((((L->top)->tt) == 0) || ((((L->top)->tt) == 1) && ((L->top)->value.b) == 0));
}


void luaV_concat (lua_State *L, int total, int last) {
  do {
    StkId top = L->base + last + 1;
    int n = 2; /* number of elements handled in this pass (at least 2) */
    if (!((((top-2)->tt) == 4) || (((top-2)->tt) == 3)) || !((((top-1)->tt) == 4) || (luaV_tostring(L, top-1)))) {
      if (!call_binTM(L, top-2, top-1, top-2, TM_CONCAT)) {
        luaG_concaterror(L, top-2, top-1);
      }
    } else if ((&(&(top-1)->value.gc->ts)->tsv)->len == 0) { /* second op is empty? */
      (void)((((top - 2)->tt) == 4) || (luaV_tostring(L, top - 2))); /* result is first op (as string) */
    }
    else {
      /* at least two string values; get as many as possible */
      size_t tl = (&(&(top-1)->value.gc->ts)->tsv)->len;
      char *buffer;
      int i;
      /* collect total length */
      for (n = 1; n < total && ((((top-n-1)->tt) == 4) || (luaV_tostring(L, top-n-1))); n++) {
        size_t l = (&(&(top-n-1)->value.gc->ts)->tsv)->len;
        if (l >= ((size_t)(~(size_t)0)-2) - tl) {
          luaG_runerror(L, "string length overflow");
        }
        tl += l;
      }
      buffer = luaZ_openspace(L, &(L->l_G)->buff, tl);
      tl = 0;
      for (i=n; i>0; i--) { /* concat all strings */
        size_t l = (&(&(top-i)->value.gc->ts)->tsv)->len;
        memcpy(buffer+tl, ((const char *)(((&(top-i)->value.gc->ts)) + 1)), l);
        tl += l;
      }
      { TValue *i_o=(top-n); i_o->value.gc=((GCObject *)((luaS_newlstr(L, buffer, tl)))); i_o->tt=4; ((void)0); };
    }
    total -= n-1; /* got `n' strings to create 1 new */
    last -= n-1;
  } while (total > 1); /* repeat until only 1 result left */
}


static void Arith (lua_State *L, StkId ra, const TValue *rb,
                   const TValue *rc, TMS op) {
  TValue tempb, tempc;
  const TValue *b, *c;
  if ((b = luaV_tonumber(rb, &tempb)) != ((void *)0) &&
      (c = luaV_tonumber(rc, &tempc)) != ((void *)0)) {
    lua_Number nb = ((b)->value.n), nc = ((c)->value.n);
    switch (op) {
      case TM_ADD: {
        { TValue *i_o=(ra); i_o->value.n=(((nb)+(nc))); i_o->tt=3; };
        break;
      }
      case TM_SUB: {
        { TValue *i_o=(ra); i_o->value.n=(((nb)-(nc))); i_o->tt=3; };
        break;
      }
      case TM_MUL: {
        { TValue *i_o=(ra); i_o->value.n=(((nb)*(nc))); i_o->tt=3; };
        break;
      }
      case TM_DIV: {
        { TValue *i_o=(ra); i_o->value.n=(((nb)/(nc))); i_o->tt=3; };
        break;
      }
      case TM_MOD: {
        { TValue *i_o=(ra); i_o->value.n=(((nb) - floor((nb)/(nc))*(nc))); i_o->tt=3; };
        break;
      }
      case TM_POW: {
        { TValue *i_o=(ra); i_o->value.n=((pow(nb,nc))); i_o->tt=3; };
        break;
      }
      case TM_UNM: {
        { TValue *i_o=(ra); i_o->value.n=((-(nb))); i_o->tt=3; };
        break;
      }
      default: {
        ((void)0);
        break;
      }
    }
  }
  else if (!call_binTM(L, rb, rc, ra, op)) {
    luaG_aritherror(L, rb, rc);
  }
}


/*
** some macros for common tasks in `luaV_execute'
*/
/* to be used after possible stack reallocation */
void luaV_execute (lua_State *L, int nexeccalls) {
  LClosure *cl;
  StkId base;
  TValue *k;
  const Instruction *pc;
 reentry: /* entry point */
  ((void)0);
  pc = L->savedpc;
  cl = &(&(L->ci->func)->value.gc->cl)->l;
  base = L->base;
  k = cl->p->k;
  /* main loop of interpreter */
  for (;;) {
    const Instruction i = *pc++;
    StkId ra;
    if ((L->hookmask & ((1 << 2) | (1 << 3))) &&
        (--L->hookcount == 0 || L->hookmask & (1 << 2))) {
      traceexec(L, pc);
      if (L->status == 1) { /* did hook yield? */
        L->savedpc = pc - 1;
        return;
      }
      base = L->base;
    }
    /* warning!! several calls may realloc the stack and invalidate `ra' */
    ra = (base+(((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0)))));
    ((void)0);
    ((void)0);
    ((void)0);
    switch ((((OpCode)(((i)>>0) & ((~((~(Instruction)0)<<6))<<0))))) {
      case OP_MOVE: {
        { const TValue *o2=((base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)))))); TValue *o1=(ra); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
        continue;
      }
      case OP_LOADK: {
        { const TValue *o2=((k+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0)))))); TValue *o1=(ra); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
        continue;
      }
      case OP_LOADBOOL: {
        { TValue *i_o=(ra); i_o->value.b=((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))); i_o->tt=1; };
        if ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) pc++; /* skip next instruction (if C) */
        continue;
      }
      case OP_LOADNIL: {
        TValue *rb = (base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)))));
        do {
          ((rb--)->tt=0);
        } while (rb >= ra);
        continue;
      }
      case OP_GETUPVAL: {
        int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))));
        { const TValue *o2=(cl->upvals[b]->v); TValue *o1=(ra); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
        continue;
      }
      case OP_GETGLOBAL: {
        TValue g;
        TValue *rb = (k+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0)))));
        { TValue *i_o=(&g); i_o->value.gc=((GCObject *)((cl->env))); i_o->tt=5; ((void)0); };
        ((void)0);
        { L->savedpc = pc; {luaV_gettable(L, &g, rb, ra);}; base = L->base; };
        continue;
      }
      case OP_GETTABLE: {
        { L->savedpc = pc; {luaV_gettable(L, (base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))), ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))), ra);}; base = L->base; };
        continue;
      }
      case OP_SETGLOBAL: {
        TValue g;
        { TValue *i_o=(&g); i_o->value.gc=((GCObject *)((cl->env))); i_o->tt=5; ((void)0); };
        ((void)0);
        { L->savedpc = pc; {luaV_settable(L, &g, (k+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))), ra);}; base = L->base; };
        continue;
      }
      case OP_SETUPVAL: {
        UpVal *uv = cl->upvals[(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))];
        { const TValue *o2=(ra); TValue *o1=(uv->v); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
        { if (((((ra)->tt) >= 4) && (((((ra)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) && ((((((GCObject *)((uv)))))->gch.marked) & ((1<<(2))))) luaC_barrierf(L,(((GCObject *)((uv)))),((ra)->value.gc)); };
        continue;
      }
      case OP_SETTABLE: {
        { L->savedpc = pc; {luaV_settable(L, ra, ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))), ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))));}; base = L->base; };
        continue;
      }
      case OP_NEWTABLE: {
        int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))));
        int c = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))));
        { TValue *i_o=(ra); i_o->value.gc=((GCObject *)((luaH_new(L, luaO_fb2int(b), luaO_fb2int(c))))); i_o->tt=5; ((void)0); };
        { L->savedpc = pc; {{ ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };}; base = L->base; };
        continue;
      }
      case OP_SELF: {
        StkId rb = (base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)))));
        { const TValue *o2=(rb); TValue *o1=(ra+1); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
        { L->savedpc = pc; {luaV_gettable(L, rb, ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))), ra);}; base = L->base; };
        continue;
      }
      case OP_ADD: {
        { TValue *rb = ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))); TValue *rc = ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))); if ((((rb)->tt) == 3) && (((rc)->tt) == 3)) { lua_Number nb = ((rb)->value.n), nc = ((rc)->value.n); { TValue *i_o=(ra); i_o->value.n=(((nb)+(nc))); i_o->tt=3; }; } else { L->savedpc = pc; {Arith(L, ra, rb, rc, TM_ADD);}; base = L->base; }; };
        continue;
      }
      case OP_SUB: {
        { TValue *rb = ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))); TValue *rc = ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))); if ((((rb)->tt) == 3) && (((rc)->tt) == 3)) { lua_Number nb = ((rb)->value.n), nc = ((rc)->value.n); { TValue *i_o=(ra); i_o->value.n=(((nb)-(nc))); i_o->tt=3; }; } else { L->savedpc = pc; {Arith(L, ra, rb, rc, TM_SUB);}; base = L->base; }; };
        continue;
      }
      case OP_MUL: {
        { TValue *rb = ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))); TValue *rc = ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))); if ((((rb)->tt) == 3) && (((rc)->tt) == 3)) { lua_Number nb = ((rb)->value.n), nc = ((rc)->value.n); { TValue *i_o=(ra); i_o->value.n=(((nb)*(nc))); i_o->tt=3; }; } else { L->savedpc = pc; {Arith(L, ra, rb, rc, TM_MUL);}; base = L->base; }; };
        continue;
      }
      case OP_DIV: {
        { TValue *rb = ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))); TValue *rc = ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))); if ((((rb)->tt) == 3) && (((rc)->tt) == 3)) { lua_Number nb = ((rb)->value.n), nc = ((rc)->value.n); { TValue *i_o=(ra); i_o->value.n=(((nb)/(nc))); i_o->tt=3; }; } else { L->savedpc = pc; {Arith(L, ra, rb, rc, TM_DIV);}; base = L->base; }; };
        continue;
      }
      case OP_MOD: {
        { TValue *rb = ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))); TValue *rc = ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))); if ((((rb)->tt) == 3) && (((rc)->tt) == 3)) { lua_Number nb = ((rb)->value.n), nc = ((rc)->value.n); { TValue *i_o=(ra); i_o->value.n=(((nb) - floor((nb)/(nc))*(nc))); i_o->tt=3; }; } else { L->savedpc = pc; {Arith(L, ra, rb, rc, TM_MOD);}; base = L->base; }; };
        continue;
      }
      case OP_POW: {
        { TValue *rb = ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))); TValue *rc = ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))); if ((((rb)->tt) == 3) && (((rc)->tt) == 3)) { lua_Number nb = ((rb)->value.n), nc = ((rc)->value.n); { TValue *i_o=(ra); i_o->value.n=((pow(nb,nc))); i_o->tt=3; }; } else { L->savedpc = pc; {Arith(L, ra, rb, rc, TM_POW);}; base = L->base; }; };
        continue;
      }
      case OP_UNM: {
        TValue *rb = (base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)))));
        if ((((rb)->tt) == 3)) {
          lua_Number nb = ((rb)->value.n);
          { TValue *i_o=(ra); i_o->value.n=((-(nb))); i_o->tt=3; };
        }
        else {
          { L->savedpc = pc; {Arith(L, ra, rb, rb, TM_UNM);}; base = L->base; };
        }
        continue;
      }
      case OP_NOT: {
        int res = (((((base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))))->tt) == 0) || (((((base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))))->tt) == 1) && (((base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))))->value.b) == 0)); /* next assignment may change this value */
        { TValue *i_o=(ra); i_o->value.b=(res); i_o->tt=1; };
        continue;
      }
      case OP_LEN: {
        const TValue *rb = (base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)))));
        switch (((rb)->tt)) {
          case 5: {
            { TValue *i_o=(ra); i_o->value.n=(((lua_Number)((luaH_getn((&(rb)->value.gc->h)))))); i_o->tt=3; };
            break;
          }
          case 4: {
            { TValue *i_o=(ra); i_o->value.n=(((lua_Number)(((&(&(rb)->value.gc->ts)->tsv)->len)))); i_o->tt=3; };
            break;
          }
          default: { /* try metamethod */
            { L->savedpc = pc; {if (!call_binTM(L, rb, (&luaO_nilobject_), ra, TM_LEN)) { luaG_typeerror(L, rb, "get length of"); };}; base = L->base; }
          }
        }
        continue;
      }
      case OP_CONCAT: {
        int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))));
        int c = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))));
        { L->savedpc = pc; {luaV_concat(L, c-b+1, c); { ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };}; base = L->base; };
        { const TValue *o2=(base+b); TValue *o1=((base+(((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0)))))); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
        continue;
      }
      case OP_JMP: {
        {(pc) += (((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))-(((1<<(9 + 9))-1)>>1))); {((void) 0); ((void) 0);};};
        continue;
      }
      case OP_EQ: {
        TValue *rb = ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)))));
        TValue *rc = ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)))));
        { L->savedpc = pc; {if ((((rb)->tt) == ((rc)->tt) && luaV_equalval(L, rb, rc)) == (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0))))) { {(pc) += (((((int)(((*pc)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))-(((1<<(9 + 9))-1)>>1))); {((void) 0); ((void) 0);};}; };}; base = L->base; }
        pc++;
        continue;
      }
      case OP_LT: {
        { L->savedpc = pc; {if (luaV_lessthan(L, ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))), ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)))))) == (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0))))) { {(pc) += (((((int)(((*pc)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))-(((1<<(9 + 9))-1)>>1))); {((void) 0); ((void) 0);};}; };}; base = L->base; }
        pc++;
        continue;
      }
      case OP_LE: {
        { L->savedpc = pc; {if (lessequal(L, ((((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))), ((((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & (1 << (9 - 1))) ? k+((int)((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) & ~(1 << (9 - 1))) : base+(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)))))) == (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0))))) { {(pc) += (((((int)(((*pc)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))-(((1<<(9 + 9))-1)>>1))); {((void) 0); ((void) 0);};}; };}; base = L->base; }
        pc++;
        continue;
      }
      case OP_TEST: {
        if (((((ra)->tt) == 0) || ((((ra)->tt) == 1) && ((ra)->value.b) == 0)) != (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) {
          {(pc) += (((((int)(((*pc)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))-(((1<<(9 + 9))-1)>>1))); {((void) 0); ((void) 0);};};
        }
        pc++;
        continue;
      }
      case OP_TESTSET: {
        TValue *rb = (base+(((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)))));
        if (((((rb)->tt) == 0) || ((((rb)->tt) == 1) && ((rb)->value.b) == 0)) != (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))) {
          { const TValue *o2=(rb); TValue *o1=(ra); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
          {(pc) += (((((int)(((*pc)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))-(((1<<(9 + 9))-1)>>1))); {((void) 0); ((void) 0);};};
        }
        pc++;
        continue;
      }
      case OP_CALL: {
        int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))));
        int nresults = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)))) - 1;
        if (b != 0) {
          L->top = ra+b; /* else previous instruction set top */
        }
        L->savedpc = pc;
        switch (luaD_precall(L, ra, nresults)) {
          case 0: {
            nexeccalls++;
            goto reentry; /* restart luaV_execute over new Lua function */
          }
          case 1: {
            /* it was a C function (`precall' called it); adjust results */
            if (nresults >= 0) {
              L->top = L->ci->top;
            }
            base = L->base;
            continue;
          }
          default: {
            return; /* yield */
          }
        }
      }
      case OP_TAILCALL: {
        int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))));
        if (b != 0) {
          L->top = ra+b; /* else previous instruction set top */
        }
        L->savedpc = pc;
        ((void)0);
        switch (luaD_precall(L, ra, (-1))) {
          case 0: {
            /* tail call: put new frame in place of previous one */
            CallInfo *ci = L->ci - 1; /* previous frame */
            int aux;
            StkId func = ci->func;
            StkId pfunc = (ci+1)->func; /* previous function index */
            if (L->openupval) {
              luaF_close(L, ci->base);
            }
            L->base = ci->base = ci->func + ((ci+1)->base - pfunc);
            for (aux = 0; pfunc+aux < L->top; aux++) { /* move frame down */
              { const TValue *o2=(pfunc+aux); TValue *o1=(func+aux); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
            }
            ci->top = L->top = func+aux; /* correct top */
            ((void)0);
            ci->savedpc = L->savedpc;
            ci->tailcalls++; /* one more call lost */
            L->ci--; /* remove new frame */
            goto reentry;
          }
          case 1: { /* it was a C function (`precall' called it) */
            base = L->base;
            continue;
          }
          default: {
            return; /* yield */
          }
        }
      }
      case OP_RETURN: {
        int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))));
        if (b != 0) {
          L->top = ra+b-1;
        }
        if (L->openupval) {
          luaF_close(L, base);
        }
        L->savedpc = pc;
        b = luaD_poscall(L, ra);
        if (--nexeccalls == 0) { /* was previous function running `here'? */
          return; /* no: return */
        }
        else { /* yes: continue its execution */
          if (b) {
            L->top = L->ci->top;
          }
          ((void)0);
          ((void)0);
          goto reentry;
        }
      }
      case OP_FORLOOP: {
        lua_Number step = ((ra+2)->value.n);
        lua_Number idx = ((((ra)->value.n))+(step)); /* increment index */
        lua_Number limit = ((ra+1)->value.n);
        if (((0)<(step)) ? ((idx)<=(limit))
                                : ((limit)<=(idx))) {
          {(pc) += (((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))-(((1<<(9 + 9))-1)>>1))); {((void) 0); ((void) 0);};}; /* jump back */
          { TValue *i_o=(ra); i_o->value.n=(idx); i_o->tt=3; }; /* update internal index... */
          { TValue *i_o=(ra+3); i_o->value.n=(idx); i_o->tt=3; }; /* ...and external index */
        }
        continue;
      }
      case OP_FORPREP: {
        const TValue *init = ra;
        const TValue *plimit = ra+1;
        const TValue *pstep = ra+2;
        L->savedpc = pc; /* next steps may throw errors */
        if (!(((init)->tt) == 3 || (((init) = luaV_tonumber(init,ra)) != ((void *)0)))) {
          luaG_runerror(L, "'" "for" "'" " initial value must be a number");
        } else if (!(((plimit)->tt) == 3 || (((plimit) = luaV_tonumber(plimit,ra+1)) != ((void *)0)))) {
          luaG_runerror(L, "'" "for" "'" " limit must be a number");
        } else if (!(((pstep)->tt) == 3 || (((pstep) = luaV_tonumber(pstep,ra+2)) != ((void *)0)))) {
          luaG_runerror(L, "'" "for" "'" " step must be a number");
        }
        { TValue *i_o=(ra); i_o->value.n=(((((ra)->value.n))-(((pstep)->value.n)))); i_o->tt=3; };
        {(pc) += (((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))-(((1<<(9 + 9))-1)>>1))); {((void) 0); ((void) 0);};};
        continue;
      }
      case OP_TFORLOOP: {
        StkId cb = ra + 3; /* call base */
        { const TValue *o2=(ra+2); TValue *o1=(cb+2); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
        { const TValue *o2=(ra+1); TValue *o1=(cb+1); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
        { const TValue *o2=(ra); TValue *o1=(cb); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
        L->top = cb+3; /* func. + 2 args (state and index) */
        { L->savedpc = pc; {luaD_call(L, cb, (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)))));}; base = L->base; };
        L->top = L->ci->top;
        cb = (base+(((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0))))) + 3; /* previous call may change the stack */
        if (!(((cb)->tt) == 0)) { /* continue loop? */
          { const TValue *o2=(cb); TValue *o1=(cb-1); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; /* save control variable */
          {(pc) += (((((int)(((*pc)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))-(((1<<(9 + 9))-1)>>1))); {((void) 0); ((void) 0);};}; /* jump back */
        }
        pc++;
        continue;
      }
      case OP_SETLIST: {
        int n = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))));
        int c = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))));
        int last;
        Table *h;
        if (n == 0) {
          n = ((int)((L->top - ra))) - 1;
          L->top = L->ci->top;
        }
        if (c == 0) {
          c = ((int)((*pc++)));
        }
        { if (!((((ra)->tt) == 5))) break; };
        h = (&(ra)->value.gc->h);
        last = ((c-1)*50) + n;
        if (last > h->sizearray) { /* needs more space? */
          luaH_resizearray(L, h, last); /* pre-alloc it at once */
        }
        for (; n > 0; n--) {
          TValue *val = ra+n;
          { const TValue *o2=(val); TValue *o1=(luaH_setnum(L, h, last--)); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
          { if (((((val)->tt) >= 4) && (((((val)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) && ((((((GCObject *)((h)))))->gch.marked) & ((1<<(2))))) luaC_barrierback(L,h); };
        }
        continue;
      }
      case OP_CLOSE: {
        luaF_close(L, ra);
        continue;
      }
      case OP_CLOSURE: {
        Proto *p;
        Closure *ncl;
        int nup, j;
        p = cl->p->p[(((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))];
        nup = p->nups;
        ncl = luaF_newLclosure(L, nup, cl->env);
        ncl->l.p = p;
        for (j=0; j<nup; j++, pc++) {
          if ((((OpCode)(((*pc)>>0) & ((~((~(Instruction)0)<<6))<<0)))) == OP_GETUPVAL) {
            ncl->l.upvals[j] = cl->upvals[(((int)(((*pc)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))];
          } else {
            ((void)0);
            ncl->l.upvals[j] = luaF_findupval(L, base + (((int)(((*pc)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)))));
          }
        }
        { TValue *i_o=(ra); i_o->value.gc=((GCObject *)((ncl))); i_o->tt=6; ((void)0); };
        { L->savedpc = pc; {{ ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };}; base = L->base; };
        continue;
      }
      case OP_VARARG: {
        int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)))) - 1;
        int j;
        CallInfo *ci = L->ci;
        int n = ((int)((ci->base - ci->func))) - cl->p->numparams - 1;
        if (b == (-1)) {
          { L->savedpc = pc; {if ((char *)L->stack_last - (char *)L->top <= (n)*(int)sizeof(TValue)) luaD_growstack(L, n); else ((void)0);;}; base = L->base; };
          ra = (base+(((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0))))); /* previous call may change the stack */
          b = n;
          L->top = ra + n;
        }
        for (j = 0; j < b; j++) {
          if (j < n) {
            { const TValue *o2=(ci->base - n + j); TValue *o1=(ra + j); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
          }
          else {
            ((ra + j)->tt=0);
          }
        }
        continue;
      }
    }
  }
}


