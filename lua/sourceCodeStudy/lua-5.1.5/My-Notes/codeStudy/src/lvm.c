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
  /*   ttisnumber(obj) */
  if ( obj->tt == LUA_TNUMBER ) {
    return obj;
  }
  /*    ttisstring(obj)                         svalue(obj)*/
  if (  obj->tt == LUA_TSTRING  && luaO_str2d( (const char *)((&(obj)->value.gc->ts) + 1), &num) ) {
    /* setnvalue(n, num); */
    { 
        TValue *i_o = n; 
        i_o->value.n = num; 
        i_o->tt = LUA_TNUMBER; 
    };
    return n;
  }
  else {
    return NULL;
  }
}


int luaV_tostring (lua_State *L, StkId obj) {
  /*    ttisnumber(obj) */
  if (!(obj->tt == LUA_TNUMBER) ) {
    return 0;
  } else {
    /*                32      LUAI_MAXNUMBER2STR */
    char s[LUAI_MAXNUMBER2STR];
    /*             nvalue(obj) */
    lua_Number n = obj->value.n;
    /* lua_number2str(s, n); */
    sprintf( s, "%.14g", n );
    /* setsvalue2s(L, obj, luaS_new(L, s)); */
    { 
        TValue *i_o = obj; 
        i_o->value.gc = (GCObject *)( luaS_newlstr(L, s, strlen(s)) ); 
        i_o->tt = LUA_TSTRING; 
        ((void)0); 
    };
    return 1;
  }
}


static void traceexec (lua_State *L, const Instruction *pc) {
  lu_byte mask = L->hookmask;
  const Instruction *oldpc = L->savedpc;
  L->savedpc = pc;
  /*           LUA_MASKCOUNT */
  if ( (mask & (1 << 3)) && L->hookcount == 0) {
    /* resethookcount(L); */
    L->hookcount = L->basehookcount;
    /*                     3      LUA_HOOKCOUNT */
    luaD_callhook(L, LUA_HOOKCOUNT, -1);
  }
  /*         LUA_MASKLINE */
  if (mask & (1 << 2)) {
    /*                         ci_func(L->ci)->l.p */
    Proto *p = (&(L->ci->func)->value.gc->cl)->l.p;
    /*         pcRel(pc, p); */
    int npc = (int)(pc - p->code) - 1;
    /*            getline(p, npc); */
    int newline = (p->lineinfo) ? p->lineinfo[npc] : 0;
    /* call linehook when enter a new function, when jump back (loop),
       or when enter a new line */
    /*                                          getline(p, pcRel(oldpc, p)) */
    if (npc == 0 || pc <= oldpc || newline != ( (p->lineinfo) ? p->lineinfo[((int)(oldpc - p->code)) - 1] : 0) ) {
      /*                        2    LUA_HOOKLINE */
      luaD_callhook(L, LUA_HOOKLINE, newline);
    }
  }
}


static void callTMres (lua_State *L, StkId res, const TValue *f,
                        const TValue *p1, const TValue *p2) {
  /*                  savestack(L, res); */
  ptrdiff_t result = ((char *)(res) - (char *)L->stack);
  /* setobj2s(L, L->top, f); */
  { 
      const TValue *o2 = f; 
      TValue *o1 = L->top; 
      o1->value = o2->value; 
      o1->tt = o2->tt; 
      ((void)0); 
  }; /* push function */
  /* setobj2s(L, L->top+1, p1); */
  { 
      const TValue *o2 = p1; 
      TValue *o1 = L->top+1; 
      o1->value = o2->value; 
      o1->tt = o2->tt; 
      ((void)0); 
  }; /* 1st argument */
  /* setobj2s(L, L->top+2, p2); */
  { 
      const TValue *o2 = p2; 
      TValue *o1= L->top+2; 
      o1->value = o2->value; 
      o1->tt = o2->tt; 
      ((void)0); 
  }; /* 2nd argument */

  /* luaD_checkstack(L, 3); */
  if ( (char *)L->stack_last - (char *)L->top <=  3* (int)sizeof(TValue) ) { 
      luaD_growstack(L, 3); 
  } else { 
      ((void)0);;
  }
  L->top += 3;
  luaD_call(L, L->top - 3, 1);
  /*     restorestack(L, result); */
  res = (TValue *)((char *)L->stack + result);
  L->top--;

  /* setobjs2s(L, res, L->top); */
  { 
      const TValue *o2 = L->top; 
      TValue *o1 = res; 
      o1->value = o2->value; 
      o1->tt = o2->tt; 
      ((void)0); 
  };
}


static void callTM (lua_State *L, const TValue *f, const TValue *p1,
                    const TValue *p2, const TValue *p3) {
  /* setobj2s(L, L->top, f); */
  { 
      const TValue *o2=(f); 
      TValue *o1=(L->top); 
      o1->value = o2->value; 
      o1->tt=o2->tt; 
      ((void)0); 
  }; /* push function */

  /* setobj2s(L, L->top+1, p1); */
  { 
      const TValue *o2 = p1;
      TValue *o1 = L->top + 1; 
      o1->value = o2->value; 
      o1->tt = o2->tt; 
      ((void)0); 
  }; /* 1st argument */

  /* setobj2s(L, L->top+2, p2); */
  { 
      const TValue *o2 = p2;
      TValue *o1 = L->top + 2; 
      o1->value = o2->value; 
      o1->tt = o2->tt; 
      ((void)0); 
  }; /* 2nd argument */

  /* setobj2s(L, L->top+3, p3); */
  { 
      const TValue *o2 = p3;
      TValue *o1 = L->top + 3; 
      o1->value = o2->value; 
      o1->tt = o2->tt; 
      ((void)0); 
  }; /* 3th argument */

  /* luaD_checkstack(L, 4); */
  if ( (char *)L->stack_last - (char *)L->top <= 4 * (int)sizeof(TValue) ) { 
      luaD_growstack(L, 4);
  } else { 
      ((void)0);;
  }

  L->top += 4;
  luaD_call(L, L->top - 4, 0);
}


void luaV_gettable (lua_State *L, const TValue *t, TValue *key, StkId val) {
  int loop;
  /*                          100   MAXTAGLOOP */
  for (loop = 0; loop < MAXTAGLOOP; loop++) {
    const TValue *tm;
    /*   ttistable(t) */
    if ( t->tt == LUA_TTABLE ) { /* `t' is a table? */
      /*         hvalue(t); */
      Table *h = &(t)->value.gc->h;
      const TValue *res = luaH_get(h, key); /* do a primitive get */
      /*    ttisnil(res) */
      if (!(res->tt == LUA_TNIL)  || /* result is no nil? */
          /*     fasttm(L, h->metatable, TM_INDEX) */
          (tm = (h->metatable == NULL 
                 ? NULL 
                 : (h->metatable->flags & (1u<<(TM_INDEX))) 
                   ? NULL 
                   : luaT_gettm(h->metatable, TM_INDEX, ((L->l_G))->tmname[TM_INDEX]))
          ) == NULL 
      )
      { /* or no TM? */
        /* setobj2s(L, val, res); */
        { 
            const TValue *o2 = (res); 
            TValue *o1 = (val); 
            o1->value = o2->value; 
            o1->tt = o2->tt; 
            ((void)0); 
        };
        return;
      }
      /* else will try the tag method */
    }
    /*          ttisnil(tm = luaT_gettmbyobj(L, t, TM_INDEX) */
    else if ( ((tm = luaT_gettmbyobj(L, t, TM_INDEX))->tt) == LUA_TNIL ) {
      luaG_typeerror(L, t, "index");
    }
    /*   ttisfunction(tm) */
    if ( tm->tt == LUA_TFUNCTION ) {
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
  /*                         100    MAXTAGLOOP */
  for (loop = 0; loop < MAXTAGLOOP; loop++) {
    const TValue *tm;
    /*    ttistable(t) */
    if ( t->tt == LUA_TTABLE ) { /* `t' is a table? */
      /*         hvalue(t) */
      Table *h = &(t)->value.gc->h;
      TValue *oldval = luaH_set(L, h, key); /* do a primitive set */
      /*   ttisnil(oldval) */
      if (!(oldval->tt == LUA_TNIL) || /* result is no nil? */
          /*     fasttm(L, h->metatable, TM_NEWINDEX) */
          (tm = (h->metatable == NULL 
                  ? NULL
                  : (h->metatable->flags & (1u<<(TM_NEWINDEX))) 
                    ? NULL 
                    : luaT_gettm(h->metatable, TM_NEWINDEX, ((L->l_G))->tmname[TM_NEWINDEX])
                )) == NULL
      )
      { /* or no TM? */
        /* setobj2t(L, oldval, val); */
        { 
            const TValue *o2 = val; 
            TValue *o1 = oldval; 
            o1->value = o2->value; 
            o1->tt = o2->tt; 
            ((void)0); 
        };
        h->flags = 0;

        /* luaC_barriert(L, h, val); */
        { 
          if ( ( (val->tt >= LUA_TSTRING) && ((val->value.gc->gch.marked) & ((1<<0) | (1<<1)) ) ) 
               && ( ((GCObject *)h)->gch.marked & (1<<2) ) ) 
          {
              luaC_barrierback(L,h); 
          } 
        };
        return;
      }
      /* else will try the tag method */
    }
    /*          ttisnil(tm = luaT_gettmbyobj(L, t, TM_NEWINDEX) */
    else if ( (tm = luaT_gettmbyobj(L, t, TM_NEWINDEX))->tt == LUA_TNIL ) {
      luaG_typeerror(L, t, "index");
    }
    /*   ttisfunction(tm) */
    if ( tm->tt == LUA_TFUNCTION ) {
      callTM(L, tm, t, key, val);
      return;
    }
    /* else repeat with `tm' */
    /* setobj(L, &temp, tm); */
    { 
        const TValue *o2 = tm; 
        TValue *o1 = &temp; 
        o1->value = o2->value; 
        o1->tt = o2->tt; 
        ((void)0); 
    }; /* avoid pointing inside table (may rehash) */
    t = &temp;
  }
  luaG_runerror(L, "loop in settable");
}


static int call_binTM (lua_State *L, const TValue *p1, const TValue *p2,
                       StkId res, TMS event) {
  const TValue *tm = luaT_gettmbyobj(L, p1, event); /* try first operand */
  /*   ttisnil(tm) */
  if ( tm->tt == LUA_TNIL ) {
    tm = luaT_gettmbyobj(L, p2, event); /* try second operand */
  }
  /*   ttisnil(tm) */
  if ( tm->tt == LUA_TNIL ) {
    return 0;
  }
  callTMres(L, res, tm, p1, p2);
  return 1;
}


static const TValue *get_compTM (lua_State *L, Table *mt1, Table *mt2,
                                  TMS event) {
  /*                  fasttm(L, mt1, event) */
  const TValue *tm1 = (mt1 == NULL 
                       ? NULL 
                       : (mt1->flags & (1u<<(event))) 
                          ? NULL 
                          : luaT_gettm(mt1, event, ((L->l_G))->tmname[event])
  );
  const TValue *tm2;
  if (tm1 == NULL) {
    return NULL; /* no metamethod */
  }
  if (mt1 == mt2) {
    return tm1; /* same metatables => same metamethods */
  }
  /*    fasttm(L, mt2, event) */
  tm2 = (mt2 == NULL 
                       ? NULL 
                       : (mt2->flags & (1u<<(event))) 
                          ? NULL 
                          : luaT_gettm(mt2, event, ((L->l_G))->tmname[event])
  );
  if (tm2 == NULL) {
    return NULL; /* no metamethod */
  }
  if (luaO_rawequalObj(tm1, tm2)) { /* same metamethods? */
    return tm1;
  }
  return NULL;
}


static int call_orderTM (lua_State *L, const TValue *p1, const TValue *p2,
                         TMS event) {
  const TValue *tm1 = luaT_gettmbyobj(L, p1, event);
  const TValue *tm2;
  /*   ttisnil(tm1) */
  if ( tm1->tt == LUA_TNIL ) {
    return -1; /* no metamethod? */
  }
  tm2 = luaT_gettmbyobj(L, p2, event);
  if (!luaO_rawequalObj(tm1, tm2)) { /* different metamethods? */
    return -1;
  }
  callTMres(L, L->top, tm1, p1, p2);
  /*      l_isfalse(L->top) */
  return !( L->top->tt == LUA_TNIL || ( L->top->tt == LUA_TBOOLEAN && L->top->value.b == 0 ) );
}


static int l_strcmp (const TString *ls, const TString *rs) {
  /*               getstr(ls); */
  const char *l = (const char *)(ls + 1);
  size_t ll = ls->tsv.len;
  /*               getstr(rs); */
  const char *r = (const char *)(rs + 1);
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
  /*   ttype(l) != ttype(r) */
  if (  l->tt != r->tt ) {
    return luaG_ordererror(L, l, r);
    /*        ttisnumber(l) */
  } else if ( l->tt == LUA_TNUMBER ) {
    /*       luai_numlt(nvalue(l), nvalue(r)); */
    return (l->value.n) < (r->value.n);
    /*        ttisstring(l) */
  } else if ( l->tt == LUA_TSTRING ) {
    /*               rawtsvalue(l),        rawtsvalue(r))  */
    return l_strcmp( (&(l)->value.gc->ts), (&(r)->value.gc->ts) ) < 0;
  } else if ((res = call_orderTM(L, l, r, TM_LT)) != -1) {
    return res;
  }
  return luaG_ordererror(L, l, r);
}


static int lessequal (lua_State *L, const TValue *l, const TValue *r) {
  int res;
  /*  ttype(l) != ttype(r) */
  if ( l->tt != r->tt ) {
    return luaG_ordererror(L, l, r);
  /*          ttisnumber(l) */
  } else if ( l->tt == LUA_TNUMBER ) {
    /*      luai_numle(nvalue(l), nvalue(r)); */
    return  l->value.n <= r->value.n;
  /*         ttisstring(l) */
  } else if ( l->tt == LUA_TSTRING ) {
    /*                   rawtsvalue(l), rawtsvalue(r) */
    return l_strcmp(&(l)->value.gc->ts, &(r)->value.gc->ts) <= 0;
  } else if ((res = call_orderTM(L, l, r, TM_LE)) != -1) { /* first try `le' */
    return res;
  } else if ((res = call_orderTM(L, r, l, TM_LT)) != -1) { /* else try `lt' */
    return !res;
  }
  return luaG_ordererror(L, l, r);
}


int luaV_equalval (lua_State *L, const TValue *t1, const TValue *t2) {
  const TValue *tm;
  /* lua_assert(ttype(t1) == ttype(t2)); */
  ((void)0);
  /*      ttype(t1) */
  switch ( t1->tt ) {
    case LUA_TNIL: { /* 0 */
      return 1;
    }
    case LUA_TNUMBER: { /* 3 */
      /*     luai_numeq(nvalue(t1), nvalue(t2)); */
      return t1->value.n== t2->value.n;
    }
    case LUA_TBOOLEAN: {  /* 1 */
      /*     bvalue(t1) == bvalue(t2) */
      return t1->value.b == t2->value.b; /* true must be 1 !! */
    }
    case LUA_TLIGHTUSERDATA: { /* 2 */
      /*     pvalue(t1) == pvalue(t2) */
      return t1->value.p == t2->value.p;
    }
    case LUA_TUSERDATA: { /* 7 */
      /*   uvalue(t1)                == uvalue(t2) */
      if ( &(&(t1)->value.gc->u)->uv == &(&(t2)->value.gc->u)->uv ) {
        return 1;
      }
      /*                                  uvalue(t1)->metatable,                  uvalue(t2)->metatable */
      tm = get_compTM(L, (&(&(t1)->value.gc->u)->uv)->metatable, (&(&(t2)->value.gc->u)->uv)->metatable,
                         TM_EQ);
      break; /* will try TM */
    }
    case LUA_TTABLE: { /* 5 */
      /*   hvalue(t1) == hvalue(t2) */
      if (&(t1)->value.gc->h == &(t2)->value.gc->h ) {
        return 1;
      }
      /*                           hvalue(t1)->metatable,           hvalue(t2)->metatable */
      tm = get_compTM(L, (&(t1)->value.gc->h)->metatable, (&(t2)->value.gc->h)->metatable, TM_EQ);
      break; /* will try TM */
    }
    default: {
      /*        gcvalue(t1) == gcvalue(t2) */
      return (t1->value.gc) == (t2->value.gc);
    }
  }
  if (tm == NULL) {
    return 0; /* no TM? */
  }
  callTMres(L, L->top, tm, t1, t2); /* call TM */
  /*       l_isfalse(L->top); */
  return !( L->top->tt == LUA_TNIL || ( L->top->tt == LUA_TBOOLEAN && L->top->value.b == 0) );
}


void luaV_concat (lua_State *L, int total, int last) {
  do {
    StkId top = L->base + last + 1;
    int n = 2; /* number of elements handled in this pass (at least 2) */
    /*     ttisstring(top-2)                  ttisnumber(top-2)                   tostring(L, top-1)*/
    if ( !( ((top-2)->tt == LUA_TSTRING) || ( (top-2)->tt == LUA_TNUMBER)) || !( ((top-1)->tt == LUA_TSTRING) || (luaV_tostring(L, top-1)) ) ) {
      if (!call_binTM(L, top-2, top-1, top-2, TM_CONCAT)) {
        luaG_concaterror(L, top-2, top-1);
      }
    /*                            tsvalue(top-1)->len */
    } else if ( (&(&(top-1)->value.gc->ts)->tsv)->len == 0) { /* second op is empty? */
      /*      tostring(L, top - 2) */
      (void)( ((top - 2)->tt == LUA_TSTRING) || luaV_tostring(L, top - 2) ); /* result is first op (as string) */
    }
    else {
      /* at least two string values; get as many as possible */
      /*                            tsvalue(top-1)->len */
      size_t tl = (&(&(top-1)->value.gc->ts)->tsv)->len;
      char *buffer;
      int i;
      /* collect total length */
      /*                         tostring(L, top-n-1) */
      for (n = 1; n < total && ( ((top-n-1)->tt == LUA_TSTRING) || luaV_tostring(L, top-n-1) ); n++) {
        /*                           tsvalue(top-n-1)->len */
        size_t l = (&(&(top-n-1)->value.gc->ts)->tsv)->len;
        if (l >= MAX_SIZET - tl) {
          luaG_runerror(L, "string length overflow");
        }
        tl += l;
      }
      /*                           G(L) */
      buffer = luaZ_openspace(L, &(L->l_G)->buff, tl);
      tl = 0;
      for (i=n; i>0; i--) { /* concat all strings */
        /*                           tsvalue(top-i)->len */
        size_t l = (&(&(top-i)->value.gc->ts)->tsv)->len;
        /*                 svalue(top-i) */
        memcpy(buffer+tl, ((const char *)((&(top-i)->value.gc->ts) + 1)), l);
        tl += l;
      }
      /* setsvalue2s(L, top-n, luaS_newlstr(L, buffer, tl)); */
      { 
          TValue *i_o = top-n; 
          i_o->value.gc = (GCObject *)(luaS_newlstr(L, buffer, tl)); 
          i_o->tt = LUA_TSTRING; 
          ((void)0); 
      };
    }
    total -= n-1; /* got `n' strings to create 1 new */
    last -= n-1;
  } while (total > 1); /* repeat until only 1 result left */
}


static void Arith (lua_State *L, StkId ra, const TValue *rb,
                   const TValue *rc, TMS op) {
  TValue tempb, tempc;
  const TValue *b, *c;
  if ((b = luaV_tonumber(rb, &tempb)) != NULL &&
      (c = luaV_tonumber(rc, &tempc)) != NULL) {
    /*         nb = nvalue(b), nc = nvalue(c); */
    lua_Number nb = b->value.n, nc = c->value.n;
    switch (op) {
      case TM_ADD: {
        /* setnvalue(ra, luai_numadd(nb, nc)); */
        { 
            TValue *i_o =ra; 
            i_o->value.n = nb + nc; 
            i_o->tt = LUA_TNUMBER; 
        };
        break;
      }
      case TM_SUB: {
        /* setnvalue(ra, luai_numsub(nb, nc));  */
        { 
            TValue *i_o =ra; 
            i_o->value.n = nb - nc; 
            i_o->tt = LUA_TNUMBER; 
        };
        break;
      }
      case TM_MUL: {
        /* setnvalue(ra, luai_nummul(nb, nc));  */
        { 
            TValue *i_o = ra; 
            i_o->value.n = nb * nc; 
            i_o->tt = LUA_TNUMBER; 
        };
        break;
      }
      case TM_DIV: {
        /* setnvalue(ra, luai_numdiv(nb, nc));  */
        { 
            TValue *i_o = ra; 
            i_o->value.n = nb / nc; 
            i_o->tt = LUA_TNUMBER; 
        };
        break;
      }
      case TM_MOD: {
        /* setnvalue(ra, luai_nummod(nb, nc));  */
        { 
            TValue *i_o = (ra);
            i_o->value.n = nb - floor(nb/nc) * nc; 
            i_o->tt = LUA_TNUMBER; 
        };
        break;
      }
      case TM_POW: {
        /* setnvalue(ra, luai_numpow(nb, nc));  */
        { 
            TValue *i_o = ra; 
            i_o->value.n = pow(nb,nc); 
            i_o->tt = LUA_TNUMBER; 
        };
        break;
      }
      case TM_UNM: {
        /* setnvalue(ra, luai_numunm(nb));  */
        { 
            TValue *i_o = ra; 
            i_o->value.n =  -(nb); 
            i_o->tt = LUA_TNUMBER; 
        };
        break;
      }
      default: {
        /* lua_assert(0);  */
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

#define runtime_check(L, c)	{ if (!(c)) break; }

#define RA(i)	(base+GETARG_A(i))
/* to be used after possible stack reallocation */
#define RB(i)	check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base+GETARG_B(i))
#define RC(i)	check_exp(getCMode(GET_OPCODE(i)) == OpArgR, base+GETARG_C(i))
#define RKB(i)	check_exp(getBMode(GET_OPCODE(i)) == OpArgK, \
	ISK(GETARG_B(i)) ? k+INDEXK(GETARG_B(i)) : base+GETARG_B(i))
#define RKC(i)	check_exp(getCMode(GET_OPCODE(i)) == OpArgK, \
	ISK(GETARG_C(i)) ? k+INDEXK(GETARG_C(i)) : base+GETARG_C(i))
#define KBx(i)	check_exp(getBMode(GET_OPCODE(i)) == OpArgK, k+GETARG_Bx(i))


#define dojump(L,pc,i)	{(pc) += (i); luai_threadyield(L);}


#define Protect(x)	{ L->savedpc = pc; {x;}; base = L->base; }


#define arith_op(op,tm) { \
        TValue *rb = RKB(i); \
        TValue *rc = RKC(i); \
        if (ttisnumber(rb) && ttisnumber(rc)) { \
          lua_Number nb = nvalue(rb), nc = nvalue(rc); \
          setnvalue(ra, op(nb, nc)); \
        } \
        else \
          Protect(Arith(L, ra, rb, rc, tm)); \
      }



void luaV_execute (lua_State *L, int nexeccalls) {
  LClosure *cl;
  StkId base;
  TValue *k;
  const Instruction *pc;
 reentry: /* entry point */
  /* lua_assert(isLua(L->ci)); */
  ((void)0);
  pc = L->savedpc;
  /*              clvalue(L->ci->func)->l */
  cl = &(&(L->ci->func)->value.gc->cl)->l;
  base = L->base;
  k = cl->p->k;
  /* main loop of interpreter */
  for (;;) {
    const Instruction i = *pc++;
    StkId ra;
    /*              LUA_MASKLINE | LUA_MASKCOUNT */
    if ((L->hookmask & ((1 << 2) | (1 << 3))) &&
        /*                                    LUA_MASKLINE */
        (--L->hookcount == 0 || L->hookmask & (1 << 2))) {
      traceexec(L, pc);
      /*                     1   LUA_YIELD */
      if (L->status == LUA_YIELD) { /* did hook yield? */
        L->savedpc = pc - 1;
        return;
      }
      base = L->base;
    }
    /* warning!! several calls may realloc the stack and invalidate `ra' */
    /*    RA(i) */
    ra = base + ( (int)( (i>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0) ) );
    /* lua_assert(base == L->base && L->base == L->ci->base); */
    ((void)0);
    /* lua_assert(base <= L->top && L->top <= L->stack + L->stacksize); */
    ((void)0);
    /* lua_assert(L->top == L->ci->top || luaG_checkopenop(i)); */
    ((void)0);

    /*         GET_OPCODE(i) */
    switch ( (OpCode)((i>>0) & ((~((~(Instruction)0)<<6))<<0)) ) {
      case OP_MOVE: {
        /* setobjs2s(L, ra, RB(i)); */
        { 
            /*                 RB(i) */
            const TValue *o2 = (base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)) ); 
            TValue *o1 = ra; 
            o1->value = o2->value; 
            o1->tt = o2->tt; 
            ((void)0); 
        };
        continue;
      }
      case OP_LOADK: {
        /* setobj2s(L, ra, KBx(i)); */
        { 
            /*               KBx(i) */
            const TValue *o2 = (k + (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0) ) ); 
            TValue *o1 = ra; 
            o1->value = o2->value; 
            o1->tt = o2->tt; 
            ((void)0); 
        };
        continue;
      }
      case OP_LOADBOOL: {
        /* setbvalue(ra, GETARG_B(i)); */
        { 
            TValue *i_o = ra; 
            /*             GETARG_B(i) */
            i_o->value.b = (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ); 
            i_o->tt = LUA_TBOOLEAN; 
        };
        /*   GETARG_C(i) */
        if ( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) ) ) { 
            pc++; 
        } /* skip next instruction (if C) */
        continue;
      }
      case OP_LOADNIL: {
        /*           RB(i) */
        TValue *rb = base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) );
        do {
          /* setnilvalue(rb--); */
          (rb--)->tt = LUA_TNIL;
        } while (rb >= ra);
        continue;
      }
      case OP_GETUPVAL: {
        /*      GETARG_B(i); */
        int b = (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) );
        /* setobj2s(L, ra, cl->upvals[b]->v); */
        { 
            const TValue *o2 = cl->upvals[b]->v; 
            TValue *o1 = ra; 
            o1->value = o2->value; 
            o1->tt = o2->tt; 
            ((void)0); 
        };
        continue;
      }
      case OP_GETGLOBAL: {
        TValue g;
        /*           KBx(i) */
        TValue *rb = k + (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0) );
        /* sethvalue(L, &g, cl->env); */
        { 
            TValue *i_o = &g; 
            i_o->value.gc = (GCObject *)(cl->env); 
            i_o->tt = LUA_TTABLE; 
            ((void)0); 
        };
        /* lua_assert(ttisstring(rb)); */
        ((void)0);
        /* Protect(luaV_gettable(L, &g, rb, ra)); */
        { 
            L->savedpc = pc; 
            { luaV_gettable(L, &g, rb, ra); }; 
            base = L->base; 
        };
        continue;
      }
      case OP_GETTABLE: {
        /* Protect(luaV_gettable(L, RB(i), RKC(i), ra)); */
        { 
            L->savedpc = pc; 
            {
                /*                RB(i) */
                luaV_gettable(L, ( base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)) ), 
                    /* RKC(i) */
                    ( (  ((int) (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) ) & (1 << (9 - 1)) ) 
                       ? k + ( (int)( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) ) & ~(1 << (9 - 1)) ) 
                       : base + (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) )
                    ), 
                    ra);
            }; 
            base = L->base; 
        };
        continue;
      }
      case OP_SETGLOBAL: {
        TValue g;
        /* sethvalue(L, &g, cl->env); */
        { 
            TValue *i_o = &g; 
            i_o->value.gc = (GCObject *)(cl->env); 
            i_o->tt = LUA_TTABLE; 
            ((void)0); 
        };
        /* lua_assert(ttisstring(KBx(i))); */
        ((void)0);
        /* Protect(luaV_settable(L, &g, KBx(i), ra)); */
        { 
            L->savedpc = pc; 
            {
                /*                   KBx(i) */
                luaV_settable(L, &g, k + (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0) ), ra);
            }; 
            base = L->base; 
        };
        continue;
      }
      case OP_SETUPVAL: {
        /*                      GETARG_B(i) */
        UpVal *uv = cl->upvals[ (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ) ];
        /* setobj(L, uv->v, ra); */
        { 
            const TValue *o2 = ra; 
            TValue *o1 = uv->v; 
            o1->value = o2->value; 
            o1->tt = o2->tt; 
            ((void)0); 
        };
        /* luaC_barrier(L, uv, ra); */
        { 
            if (  ra->tt >= LUA_TSTRING && ( (ra->value.gc)->gch.marked & ( (1<<0) | (1<<1) ) ) && ( ( ((GCObject *)(uv))->gch.marked) & (1<<2) ) ) { 
                luaC_barrierf(L,(GCObject *)(uv), ra->value.gc); 
            } 
        };
        continue;
      }
      case OP_SETTABLE: {
        /* Protect(luaV_settable(L, ra, RKB(i), RKC(i))); */
        { 
            L->savedpc = pc; 
            {
                luaV_settable(L, ra, 
                     /*   RKB(i) */
                     (      ( (int)( (i >> (((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))) & (1 << (9 - 1)) 
                          ? k + ( (int)( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ) ) & ~(1 << (9 - 1)) ) 
                          : base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ) ), 
                     /* RKC(i) */
                     ( (  ((int) (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) ) & (1 << (9 - 1)) ) 
                        ? k + ( (int)( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) ) & ~(1 << (9 - 1)) ) 
                        : base + (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) )
                     )
                );
            }; 
            base = L->base; 
        };
        continue;
      }
      case OP_NEWTABLE: {
        /*       GETARG_B(i); */
        int b =  (int)( (i>> (((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ); 
        /*       GETARG_C(i); */
        int c = (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) );
        /* sethvalue(L, ra, luaH_new(L, luaO_fb2int(b), luaO_fb2int(c))); */
        { 
            TValue *i_o = ra; 
            i_o->value.gc = (GCObject *)( luaH_new(L, luaO_fb2int(b), luaO_fb2int(c) ) ); 
            i_o->tt = LUA_TTABLE; 
            ((void)0); 
        };
        /* Protect(luaC_checkGC(L)); */
        { 
            L->savedpc = pc; 
            {
                { 
                    ((void)0); 
                    if ( (L->l_G)->totalbytes >= (L->l_G)->GCthreshold ) { 
                        luaC_step(L); 
                    }
                };
            }; 
            base = L->base; 
        };
        continue;
      }
      case OP_SELF: {
        /*         RB(i) */
        StkId rb = base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0));
        /* setobjs2s(L, ra+1, rb); */
        { 
            const TValue *o2 = rb; 
            TValue *o1 = ra+1; 
            o1->value = o2->value; 
            o1->tt = o2->tt; 
            ((void)0); 
        };
        /* Protect(luaV_gettable(L, rb, RKC(i), ra)); */
        { 
            L->savedpc = pc; 
            {
                /*                    RKC(i) */
                luaV_gettable(L, rb, (  (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) & (1 << (9 - 1)) ) 
                                        ? k + ( (int)(  (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) ) & ~(1 << (9 - 1)) ) 
                                        : base + (int)((i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) ), 
                        ra);
            }; 
            base = L->base; 
        };
        continue;
      }
      case OP_ADD: {
        /* arith_op(luai_numadd, TM_ADD); */
        { 
            TValue *rb = ( ( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)) ) & (1 << (9 - 1)) )
                              ? k + ((int)( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) )) & ~(1 << (9 - 1))) 
                           : base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ); 
            TValue *rc = ( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))) & (1 << (9 - 1) ) 
                           ? k + ((int)( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) ) & ~(1 << (9 - 1) )) 
                           : base + (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) ); 
            if ( rb->tt == LUA_TNUMBER && rc->tt == LUA_TNUMBER ) { 
                lua_Number nb = rb->value.n, nc = rc->value.n; 
                { 
                    TValue *i_o = ra; 
                    i_o->value.n = nb + nc; 
                    i_o->tt = LUA_TNUMBER; 
                }; 
            } else { 
                L->savedpc = pc; 
                { Arith(L, ra, rb, rc, TM_ADD); }; 
                base = L->base; 
            }; 
        };
        continue;
      }
      case OP_SUB: {
        /* arith_op(luai_numsub, TM_SUB); */
        { 

            TValue *rb = ( ( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)) ) & (1 << (9 - 1)) )
                              ? k + ((int)( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) )) & ~(1 << (9 - 1))) 
                           : base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ); 
            TValue *rc = ( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))) & (1 << (9 - 1) ) 
                           ? k + ((int)( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) ) & ~(1 << (9 - 1) )) 
                           : base + (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) ); 
            if ( rb->tt == LUA_TNUMBER && rc->tt == LUA_TNUMBER ) { 
                lua_Number nb = rb->value.n, nc = rc->value.n; 
                { 

                    TValue *i_o = ra; 
                    i_o->value.n = nb - nc; 
                    i_o->tt = LUA_TNUMBER; 
                }; 
            } else { 
                L->savedpc = pc; 
                { Arith(L, ra, rb, rc, TM_SUB); }; 
                base = L->base; 
            }; 
        };
        continue;
      }
      case OP_MUL: {
        /* arith_op(luai_nummul, TM_MUL); */
        { 
            TValue *rb = ( ( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)) ) & (1 << (9 - 1)) )
                              ? k + ((int)( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) )) & ~(1 << (9 - 1))) 
                           : base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ); 
            TValue *rc = ( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))) & (1 << (9 - 1) ) 
                           ? k + ((int)( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) ) & ~(1 << (9 - 1) )) 
                           : base + (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) ); 
            if ( rb->tt == LUA_TNUMBER && rc->tt == LUA_TNUMBER ) { 
                lua_Number nb = rb->value.n, nc = rc->value.n; 
                { 

                    TValue *i_o = ra; 
                    i_o->value.n = nb * nc; 
                    i_o->tt = LUA_TNUMBER; 
                }; 
            } else { 
                L->savedpc = pc; 
                { Arith(L, ra, rb, rc, TM_MUL); }; 
                base = L->base; 
            }; 
        };
        continue;
      }
      case OP_DIV: {
        /* arith_op(luai_numdiv, TM_DIV); */
        { 
            TValue *rb = ( ( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)) ) & (1 << (9 - 1)) )
                              ? k + ((int)( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) )) & ~(1 << (9 - 1))) 
                           : base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ); 
            TValue *rc = ( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))) & (1 << (9 - 1) ) 
                           ? k + ((int)( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) ) & ~(1 << (9 - 1) )) 
                           : base + (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) ); 
            if ( rb->tt == LUA_TNUMBER && rc->tt == LUA_TNUMBER ) { 
                lua_Number nb = rb->value.n, nc = rc->value.n; 
                { 

                    TValue *i_o = ra; 
                    i_o->value.n = nb / nc; 
                    i_o->tt = LUA_TNUMBER; 
                }; 
            } else { 
                L->savedpc = pc; 
                { Arith(L, ra, rb, rc, TM_DIV); }; 
                base = L->base; 
            }; 
        };
        continue;
      }
      case OP_MOD: {
        /* arith_op(luai_nummod, TM_MOD); */
        { 
            TValue *rb = ( ( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)) ) & (1 << (9 - 1)) )
                              ? k + ((int)( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) )) & ~(1 << (9 - 1))) 
                           : base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ); 
            TValue *rc = ( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))) & (1 << (9 - 1) ) 
                           ? k + ((int)( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) ) & ~(1 << (9 - 1) )) 
                           : base + (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) ); 
            if ( rb->tt == LUA_TNUMBER && rc->tt == LUA_TNUMBER ) { 
                lua_Number nb = rb->value.n, nc = rc->value.n; 
                { 

                    TValue *i_o = ra; 
                    i_o->value.n = nb - floor(nb/nc) *nc;
                    i_o->tt = LUA_TNUMBER; 
                }; 
            } else { 
                L->savedpc = pc; 
                { Arith(L, ra, rb, rc, TM_MOD); }; 
                base = L->base; 
            }; 
        };       
        continue;
      }
      case OP_POW: {
        /* arith_op(luai_numpow, TM_POW); */
        { 
            TValue *rb = ( ( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)) ) & (1 << (9 - 1)) )
                              ? k + ((int)( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) )) & ~(1 << (9 - 1))) 
                           : base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ); 
            TValue *rc = ( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))) & (1 << (9 - 1) ) 
                           ? k + ((int)( (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) ) & ~(1 << (9 - 1) )) 
                           : base + (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) ); 
            if ( rb->tt == LUA_TNUMBER && rc->tt == LUA_TNUMBER ) { 
                lua_Number nb = rb->value.n, nc = rc->value.n; 
                { 

                    TValue *i_o = ra; 
                    i_o->value.n = pow(nb,nc);
                    i_o->tt = LUA_TNUMBER; 
                }; 
            } else { 
                L->savedpc = pc; 
                { Arith(L, ra, rb, rc, TM_POW); }; 
                base = L->base; 
            }; 
        };
        continue;
      }
      case OP_UNM: {
        /*            RB(i) */
        TValue *rb = base + (int)( (i>> (((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) );
        /*    ttisnumber(rb) */
        if ( rb->tt == LUA_TNUMBER ) {
          /*              nvalue(rb) */
          lua_Number nb = rb->value.n;
          { 
              TValue *i_o = ra; 
              /*              luai_numunm(nb) */
              i_o->value.n = -(nb); 
              i_o->tt = LUA_TNUMBER; 
          };
        }
        else {
          /* Protect(Arith(L, ra, rb, rb, TM_UNM)); */
          { 
              L->savedpc = pc; 
              { Arith(L, ra, rb, rb, TM_UNM); }; 
              base = L->base; 
          };
        }
        continue;
      }
      case OP_NOT: {
        /*         l_isfalse(RB(i)) */
        int res = ( ( (base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ))->tt ) == LUA_TNIL ) || 
                    ( ( ( (base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)))->tt) == LUA_TBOOLEAN) 
                      && (((base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))->value.b) == 0); /* next assignment may change this value */
        /* setbvalue(ra, res); */
        { 
            TValue *i_o = ra; 
            i_o->value.b = res; 
            i_o->tt = LUA_TBOOLEAN; 
        };
        continue;
      }
      case OP_LEN: {
        /*                 RB(i) */
        const TValue *rb = base + (int)( (i>> (((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) );
        /*      ttype(rb) */
        switch ( rb->tt ) {
          case LUA_TTABLE: { /* 5 */
            /* setnvalue(ra, cast_num(luaH_getn(hvalue(rb)))); */
            { 
                TValue *i_o = ra; 
                i_o->value.n = (lua_Number)( luaH_getn( &(rb)->value.gc->h ) ); 
                i_o->tt = LUA_TNUMBER; 
            };
            break;
          }
          case LUA_TSTRING: { /* 4 */
            /* setnvalue(ra, cast_num(tsvalue(rb)->len)); */
            { 
                TValue *i_o = ra; 
                i_o->value.n = (lua_Number)( (&(&(rb)->value.gc->ts)->tsv)->len ); 
                i_o->tt = LUA_TNUMBER; 
            };
            break;
          }
          default: { /* try metamethod */
            /*
            Protect(
              if (!call_binTM(L, rb, luaO_nilobject, ra, TM_LEN)) {
                luaG_typeerror(L, rb, "get length of");
              }
            )
            */
            { 
                L->savedpc = pc; 
                {
                    if ( !call_binTM(L, rb, (&luaO_nilobject_), ra, TM_LEN) ) { 
                        luaG_typeerror(L, rb, "get length of"); 
                    };
                }; 
                base = L->base; 
            }
          }
        }
        continue;
      }
      case OP_CONCAT: {
        /*      GETARG_B(i); */
        int b = (int)( (i>> (((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) );
        /*      GETARG_C(i) */
        int c = (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) );
        /* Protect(luaV_concat(L, c-b+1, c); luaC_checkGC(L)); */
        { 
            L->savedpc = pc; 
            {
                luaV_concat(L, c-b+1, c); 
                {
                    ((void)0); 
                    if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) { 
                        luaC_step(L); 
                    }
                };
            }; 
            base = L->base; 
        };
        /* setobjs2s(L, RA(i), base+b); */
        { 
            const TValue *o2 = base+b; 
            /*           RA(i) */
            TValue *o1 = base + ( (int)( (i>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0) ) );
            o1->value = o2->value; 
            o1->tt = o2->tt; 
            ((void)0); 
        };
        continue;
      }
      case OP_JMP: {
        /* dojump(L, pc, GETARG_sBx(i)); */
        {
            /*     GETARG_sBx(i) */
            pc += ((int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0) ) - (((1<<(9 + 9))-1)>>1) ); 
            { ((void) 0); ((void) 0); };
        };
        continue;
      }



      case OP_EQ: {
        /*           RKB(i) */
        TValue *rb = (      ( (int)( (i >> (((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))) & (1 << (9 - 1)) 
                          ? k + ( (int)( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ) ) & ~(1 << (9 - 1)) ) 
                          : base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ) ); 
        /*           RKC(i) */
        TValue *rc = (  (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) & (1 << (9 - 1)) ) 
                                        ? k + ( (int)(  (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) ) & ~(1 << (9 - 1)) ) 
                                        : base + (int)((i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) );
        /* 
        Protect(
          if (equalobj(L, rb, rc) == GETARG_A(i)) {
            dojump(L, pc, GETARG_sBx(*pc));
          }
        )
        */
        { 
            L->savedpc = pc; 
            {
                /*    equalobj(L, rb, rc)                              == GETARG_A(i)  */
                if ( ( rb->tt == rc->tt && luaV_equalval(L, rb, rc) )  == ((int)( (i>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0)) ) ) { 
                    {
                        /*    GETARG_sBx(*pc) */
                        pc += (int)( ( *pc >>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0) ) - (((1<<(9 + 9))-1)>>1); 
                        {((void) 0); ((void) 0);};
                    }; 
                };
            }; 
            base = L->base; 
        }
        pc++;
        continue;
      }
      case OP_LT: {
        /*
        Protect(
          if (luaV_lessthan(L, RKB(i), RKC(i)) == GETARG_A(i)) {
            dojump(L, pc, GETARG_sBx(*pc));
          }
        )
        */
        { 
            L->savedpc = pc; 
            {
                /*                     RKB(i)      */
                if ( luaV_lessthan(L, (      ( (int)( (i >> (((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))) & (1 << (9 - 1)) 
                          ? k + ( (int)( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ) ) & ~(1 << (9 - 1)) ) 
                          : base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ) ), 
                            /* RKC(i) */
                            (    (  (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) & (1 << (9 - 1)) ) 
                               ? k + ( (int)(  (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) ) & ~(1 << (9 - 1)) ) 
                               : base + (int)((i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) ) )
                           /* GETARG_A(i) */
                     ) == ( (int)( (i>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0) ) ) ) { 
                {
                    /*     GETARG_sBx(*pc) */
                    pc += ( (int)( (*pc>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0)) - (((1<<(9 + 9))-1)>>1) ); 
                    {((void) 0); ((void) 0);};
                }; 
                };
            }; 
            base = L->base; 
}
        pc++;
        continue;
      }
      case OP_LE: {
        /*
        Protect(
          if (lessequal(L, RKB(i), RKC(i)) == GETARG_A(i)) {
            dojump(L, pc, GETARG_sBx(*pc));
          }
        )
        */
        { 
            L->savedpc = pc; 
            {
                /*               RKB(i) */
                if (lessequal(L, (      ( (int)( (i >> (((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))) & (1 << (9 - 1)) 
                          ? k + ( (int)( (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ) ) & ~(1 << (9 - 1)) ) 
                          : base + (int)( (i>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ) ), 
                            /* RKC(i) */
                            (    (  (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) & (1 << (9 - 1)) ) 
                               ? k + ( (int)(  (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)) ) & ~(1 << (9 - 1)) ) 
                               : base + (int)((i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) ) )
                          /* GETARG_A(i) */
                     ) == ( (int)( (i>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0) ) ) ) 
                { 
                    {
                        /*      GETARG_sBx(*pc) */
                        (pc) += ((int)( (*pc>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0)) - (((1<<(9 + 9))-1)>>1) ); 
                        {((void) 0); ((void) 0);};
                    }; 
                };
            }; 
            base = L->base; 
        }
        pc++;
        continue;
      }


      case OP_TEST: {
        /*   l_isfalse(ra)                                                          != GETARG_C(i)*/
        if ( ( ra->tt == LUA_TNIL || (ra->tt == LUA_TBOOLEAN && ra->value.b == 0) ) != ((int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) )) ) {
          /* dojump(L, pc, GETARG_sBx(*pc)); */
          {
              /*     GETARG_sBx(*pc) */
              pc += ((int)( (*pc>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))) - (((1<<(9 + 9))-1)>>1); 
              {((void) 0); ((void) 0);};
          };
        }
        pc++;
        continue;
      }
      case OP_TESTSET: {
        /*           RB(i) */
        TValue *rb = base + (int)( (i>> (((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) );
        /*   l_isfalse(rb)                                                          != GETARG_C(i)*/
        if ( ( rb->tt == LUA_TNIL || (rb->tt == LUA_TBOOLEAN && rb->value.b == 0) ) != ((int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) )) ) {
          /* setobjs2s(L, ra, rb); */
          { 
              const TValue *o2 = rb; 
              TValue *o1 = ra; 
              o1->value = o2->value; 
              o1->tt = o2->tt; 
              ((void)0); 
          };
          /* dojump(L, pc, GETARG_sBx(*pc)); */
          {
              /*     GETARG_sBx(*pc) */
              pc += ((int)( (*pc>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))) - (((1<<(9 + 9))-1)>>1); 
              {((void) 0); ((void) 0);};
          }
        }
        pc++;
        continue;
      }
      case OP_CALL: {
        /*      GETARG_B(i) */
        int b = (int)( (i>> (((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) );
        /*             GETARG_C(i) */
        int nresults = ((int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) )) - 1;
        if (b != 0) {
          L->top = ra+b; /* else previous instruction set top */
        }
        L->savedpc = pc;
        switch (luaD_precall(L, ra, nresults)) {
          case PCRLUA: { /* 0 */
            nexeccalls++;
            goto reentry; /* restart luaV_execute over new Lua function */
          }
          case PCRC: { /* 1 */
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
        /*       GETARG_B(i) */
        int b = (int)( (i>> (((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) );
        if (b != 0) {
          L->top = ra+b; /* else previous instruction set top */
        }
        L->savedpc = pc;
        /* lua_assert(GETARG_C(i) - 1 == LUA_MULTRET); */
        ((void)0);
        /*                                (-1)   LUA_MULTRET */
        switch (luaD_precall(L, ra, LUA_MULTRET )) {
          case PCRLUA: { /* 0 */
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
              /* setobjs2s(L, func+aux, pfunc+aux); */
              { 
                  const TValue *o2 = pfunc+aux; 
                  TValue *o1 = func+aux; 
                  o1->value = o2->value; 
                  o1->tt = o2->tt; 
                  ((void)0); 
              };
            }
            ci->top = L->top = func+aux; /* correct top */
            /* lua_assert(L->top == L->base + clvalue(func)->l.p->maxstacksize); */
            ((void)0);
            ci->savedpc = L->savedpc;
            ci->tailcalls++; /* one more call lost */
            L->ci--; /* remove new frame */
            goto reentry;
          }
          case PCRC: { /* 1 */  /* it was a C function (`precall' called it) */
            base = L->base;
            continue;
          }
          default: {
            return; /* yield */
          }
        }
      }
      case OP_RETURN: {
        /*      GETARG_B(i) */
        int b = (int)( (i>> (((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) );
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
          /* lua_assert(isLua(L->ci)); */
          ((void)0);
          /* lua_assert(GET_OPCODE(*((L->ci)->savedpc - 1)) == OP_CALL); */
          ((void)0);
          goto reentry;
        }
      }

      case OP_FORLOOP: {
        /*                nvalue(ra+2) */
        lua_Number step = (ra+2)->value.n;
        /*                luai_numadd(nvalue(ra), step) */
        lua_Number idx = (ra->value.n) + step; /* increment index */
        /*                  nvalue(ra+1) */
        lua_Number limit = (ra+1)->value.n;
        /*   luai_numlt(0, step) ? luai_numle(idx, limit)*/
        if ( (0<step)            ? (idx<=limit)
        /*                         luai_numle(limit, idx) */
                                 : limit<=idx ) {
          /* dojump(L, pc, GETARG_sBx(i)); */
          {
              /*     GETARG_sBx(*pc) */
              pc += ((int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0) ) - (((1<<(9 + 9))-1)>>1) );
              {((void) 0); ((void) 0);};
          }; /* jump back */
          /* setnvalue(ra, idx) */
          { 
              TValue *i_o = ra; 
              i_o->value.n = idx; 
              i_o->tt = LUA_TNUMBER; 
          }; /* update internal index... */
          /* setnvalue(ra+3, idx); */
          { 
              TValue *i_o = ra+3; 
              i_o->value.n = idx; 
              i_o->tt = LUA_TNUMBER; 
          }; /* ...and external index */
        }
        continue;
      }
      case OP_FORPREP: {
        const TValue *init = ra;
        const TValue *plimit = ra+1;
        const TValue *pstep = ra+2;
        L->savedpc = pc; /* next steps may throw errors */
        /*   tonumber(init, ra) */
        if (!( init->tt == LUA_TNUMBER || (init = luaV_tonumber(init,ra)) != NULL ) ) {
          /*            LUA_QL("for") */
          luaG_runerror(L, "'" "for" "'" " initial value must be a number");
        /*          tonumber(plimit, ra+1) */
        } else if (!( plimit->tt == LUA_TNUMBER || ( (plimit = luaV_tonumber(plimit,ra+1) ) != NULL) ) ) {
          /*            LUA_QL("for") */
          luaG_runerror(L, "'" "for" "'" " limit must be a number");
          /*         tonumber(pstep, ra+2) */
        } else if (!( pstep->tt == LUA_TNUMBER || (pstep = luaV_tonumber(pstep,ra+2)) != NULL) ) {
          /*            LUA_QL("for") */
          luaG_runerror(L, "'" "for" "'" " step must be a number");
        }
        /* setnvalue(ra, luai_numsub(nvalue(ra), nvalue(pstep))); */
        { 
            TValue *i_o = ra;
            i_o->value.n = ra->value.n - pstep->value.n; 
            i_o->tt = LUA_TNUMBER; 
        };
        /* dojump(L, pc, GETARG_sBx(i)); */
        {
            /*     GETARG_sBx(*pc) */
            pc += ((int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0) ) - (((1<<(9 + 9))-1)>>1) );
            {((void) 0); ((void) 0);};
        };
        continue;
      }
      case OP_TFORLOOP: {
        StkId cb = ra + 3; /* call base */
        /* setobjs2s(L, cb+2, ra+2); */
        {
            const TValue *o2 = ra+2; 
            TValue *o1 = cb+2; 
            o1->value = o2->value; 
            o1->tt = o2->tt; 
            ((void)0); 
        };
        /* setobjs2s(L, cb+1, ra+1); */
        { 
            const TValue *o2 = ra+1; 
            TValue *o1 = cb+1; 
            o1->value = o2->value; 
            o1->tt = o2->tt; 
            ((void)0); 
        };
        /* setobjs2s(L, cb, ra); */
        { 
            const TValue *o2 = ra; 
            TValue *o1 = cb; 
            o1->value = o2->value; 
            o1->tt = o2->tt; 
            ((void)0); 
        };
        L->top = cb+3; /* func. + 2 args (state and index) */
        /* Protect(luaD_call(L, cb, GETARG_C(i))); */
        { 
            L->savedpc = pc; 
            /*                 GETARG_C(i) */
            {luaD_call(L, cb, ((int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) )) ); }; 
            base = L->base; 
        };
        L->top = L->ci->top;
        /*    RA(i) */
        cb = (base + ( (int)( (i>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0) ) )) + 3; /* previous call may change the stack */
        /*   ttisnil(cb) */
        if (!(cb->tt == LUA_TNIL) ) { /* continue loop? */
          /* setobjs2s(L, cb-1, cb); */
          { 
              const TValue *o2 = cb; 
              TValue *o1 = cb-1; 
              o1->value = o2->value; 
              o1->tt = o2->tt; 
              ((void)0); 
          }; /* save control variable */
          /* dojump(L, pc, GETARG_sBx(*pc)) */
          {
              /*     GETARG_sBx(*pc) */
              pc += ((int)( (*pc>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))) - (((1<<(9 + 9))-1)>>1); 
              {((void) 0); ((void) 0);};
          }
        }
        pc++;
        continue;
      }
      case OP_SETLIST: {
        /*      GETARG_B(i); */
        int n = (int)( (i>> (((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) );
        /*      GETARG_C(i) */
        int c = (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0) );
        int last;
        Table *h;
        if (n == 0) {
          /*  cast_int(L->top - ra) */
          n = (int)(L->top - ra) - 1;
          L->top = L->ci->top;
        }
        if (c == 0) {
          /*   cast_int(*pc++) */
          c = (int)(*pc++);
        }
        /* runtime_check(L, ttistable(ra)); */
        { 
            if (!(ra->tt == LUA_TTABLE) ) { 
                break; 
            }
        };
        /*  hvalue(ra) */
        h = &(ra)->value.gc->h;
        /*                      50  LFIELDS_PER_FLUSH */
        last = ((c-1) * LFIELDS_PER_FLUSH) + n;
        if (last > h->sizearray) { /* needs more space? */
          luaH_resizearray(L, h, last); /* pre-alloc it at once */
        }
        for (; n > 0; n--) {
          TValue *val = ra+n;
          /* setobj2t(L, luaH_setnum(L, h, last--), val); */
          { 
              const TValue *o2 = val; 
              TValue *o1 = luaH_setnum(L, h, last--); 
              o1->value = o2->value; 
              o1->tt = o2->tt; 
              ((void)0); 
          };
          /* luaC_barriert(L, h, val); */
          { 
              if ( ( val->tt >= LUA_TSTRING && ( (val->value.gc->gch.marked) & ((1<<0) | (1<<1)) )) && (((GCObject *)h)->gch.marked & (1<<2) ) ) { 
                  luaC_barrierback(L,h); 
              }
          };
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
        /*            GETARG_Bx(i) */
        p = cl->p->p[ (int)( (i>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0)) ];
        nup = p->nups;
        ncl = luaF_newLclosure(L, nup, cl->env);
        ncl->l.p = p;
        for (j=0; j<nup; j++, pc++) {
          /*   GET_OPCODE(*pc) */
          if ( ((OpCode)( (*pc>>0) & ((~((~(Instruction)0)<<6))<<0)) ) == OP_GETUPVAL) {
            /*                             GETARG_B(*pc) */
            ncl->l.upvals[j] = cl->upvals[ (int)( (*pc>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)) ];
          } else {
            /* lua_assert(GET_OPCODE(*pc) == OP_MOVE); */
            ((void)0);
            /*                                           GETARG_B(*pc) */
            ncl->l.upvals[j] = luaF_findupval(L, base +  (int)( (*pc>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)) );
          }
        }
        /* setclvalue(L, ra, ncl); */
        { 
            TValue *i_o = ra; 
            i_o->value.gc = (GCObject *)(ncl); 
            i_o->tt = LUA_TFUNCTION; 
            ((void)0); 
        };
        /* Protect(luaC_checkGC(L)); */
        { 
            L->savedpc = pc; 
            {
                { 
                    ((void)0); 
                    if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) { 
                        luaC_step(L); 
                    }
                };
            }; 
            base = L->base; 
        };
        continue;
      }
      case OP_VARARG: {
        /*      GETARG_B(i) */
        int b = (int)( (i>> (((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0) ) - 1;
        int j;
        CallInfo *ci = L->ci;
        /*       cast_int(ci->base - ci->func) */
        int n = (int)(ci->base - ci->func) - cl->p->numparams - 1;
        /*          (-1)   LUA_MULTRET */
        if (b == LUA_MULTRET) {
          /* Protect(luaD_checkstack(L, n)); */
          { 
              L->savedpc = pc; 
              {
                if ((char *)L->stack_last - (char *)L->top <= n * (int)sizeof(TValue) ) { 
                    luaD_growstack(L, n); 
                } else { 
                    ((void)0);;
                }
              }; 
              base = L->base; 
          };
          /*   RA(i) */
          ra = base + ( (int)( (i>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0) ) ); /* previous call may change the stack */
          b = n;
          L->top = ra + n;
        }
        for (j = 0; j < b; j++) {
          if (j < n) {
            /* setobjs2s(L, ra + j, ci->base - n + j); */
            { 
                const TValue *o2 = ci->base - n + j; 
                TValue *o1 = ra + j; 
                o1->value = o2->value; 
                o1->tt = o2->tt; 
                ((void)0); 
            };
          }
          else {
            /* setnilvalue(ra + j); */
            (ra + j)->tt = LUA_TNIL;
          }
        }
        continue;
      }
    }
  }
}


