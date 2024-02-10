/*
** $Id: lfunc.c,v 2.12.1.2 2007/12/28 14:58:43 roberto Exp $
** Auxiliary functions to manipulate prototypes and closures
** See Copyright Notice in lua.h
*/


#include <stddef.h>

#define lfunc_c
#define LUA_CORE

#include "lua.h"

#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"



/*

typedef union Closure {
  CClosure c;    // C    Lauguage Closure
  LClosure l;    // lua  Language Closure
} Closure;

*/
//
//  nelems :  upvalue count
//  e      :  function environment
//
Closure *luaF_newCclosure (lua_State *L, int nelems, Table *e) {
  /*
  luaM_malloc(L, sizeCclosure(nelems)

     Why    (nelems-1) * TValue ?
     Because    

        typedef struct CClosure {
          ClosureHeader;
          lua_CFunction f;
          TValue upvalue[1];  // occupy 1 TValue , so use exp : (nelems-1) * sizeof(TValue)    to turn out the exact size of CClosure
        } CClosure;


      ((Closure *)(luaM_realloc_(L, ((void *)0), 0, ((((int)(sizeof(CClosure))) + ((int)(sizeof(TValue)*((nelems)-1))))))))
      (Closure *)(
        luaM_realloc_(L, NULL, 0, ( (int)(sizeof(CClosure))) +  (int)(sizeof(TValue) * (nelems-1) ))
      )
  */

  // 分配一块 (    CClosure + (nelems-1) * TValue  )  大小的内存块
  Closure *c = (Closure *)(
	luaM_realloc_(L, NULL, 0, ( (int)(sizeof(CClosure))) +  (int)(sizeof(TValue) * (nelems-1) ))
  );
	  
  /* luaC_link(L, obj2gco(c), LUA_TFUNCTION); */
  luaC_link(L, ((GCObject *)c), LUA_TFUNCTION);

  c->c.isC = 1; // set C flag : Closure is a kind of C Language 
  c->c.env = e;

  /*               ((lu_byte)((nelems)));  */
  c->c.nupvalues = cast_byte(nelems);
  return c;
}


Closure *luaF_newLclosure (lua_State *L, int nelems, Table *e) {
  /*            
      cast(Closure *, luaM_malloc(L, sizeLclosure(nelems)));   
      ((Closure *)(luaM_realloc_(L, NULL, 0, ((((int)(sizeof(LClosure))) + ((int)(sizeof(TValue *)*((nelems)-1))))))));
  */
  Closure *c = (Closure *)(
	luaM_realloc_(L, NULL, 0, (int)(sizeof(LClosure)) + (int)(sizeof(TValue *) * (nelems-1) ) )
  );

  /* luaC_link(L, obj2gco(c), LUA_TFUNCTION); */
  luaC_link(L, ( (GCObject *)c), LUA_TFUNCTION);
  c->l.isC = 0; // set not C function flag   : 0 => lua function
  c->l.env = e;
  /*               ((lu_byte)((nelems))) */
  c->l.nupvalues = cast_byte(nelems);
  while (nelems--) {
    c->l.upvals[nelems] = NULL;
  }
  return c;
}


UpVal *luaF_newupval (lua_State *L) {
  /*          luaM_new(L, UpVal); */
  UpVal *uv = (UpVal *)( luaM_realloc_(L, NULL, 0, sizeof(UpVal) ) );
  /*           ((GCObject *)(uv))  LUA_TUPVAL (8+2) */
  luaC_link(L, obj2gco(uv), LUA_TUPVAL );
  uv->v = &uv->u.value;
  /* ((uv->v)->tt = LUA_TNIL); */
  setnilvalue(uv->v);
  return uv;
}


UpVal *luaF_findupval (lua_State *L, StkId level) {
  /*                (L->l_G) */
  global_State *g = G(L);
  GCObject **pp = &L->openupval;
  UpVal *p;
  UpVal *uv;
  /*                         (&((*pp)->uv))  */
  while (*pp != NULL && (p = ngcotouv(*pp))->v >= level) {
    ((void)0); /* lua_assert(p->v != &p->u.value); */
    if (p->v == level) { /* found a corresponding upvalue? */
      /*
	
	   isdead(g, obj2gco(p) )
	   (((((GCObject *)((p)))))->gch.marked & (g->currentwhite ^ ((1<<(0)) | (1<<(1)))) & ((1<<(0)) | (1<<(1))))
	  */
      if (   ((GCObject *)p)->gch.marked 
		  & ( g->currentwhite ^ ( (1<<0) | (1<<1) ) ) 
		  & ( (1<<0) | (1<<1) )
	  ) { /* is it dead? */
        /* changewhite(obj2gco(p)); */
        /* (((((GCObject *)((p)))))->gch.marked ^= ((1<<(0)) | (1<<(1)))); */ /* ressurect it */
		((GCObject *)p)->gch.marked ^= ( (1<<0) | (1<<1) );
      }
      return p;
    }
    pp = &p->next;
  }
  /*    LuaM_new(L, UpVal); */
  uv = (UpVal *)(luaM_realloc_(L, NULL, 0, sizeof(UpVal))); /* not found: create a new one */
  uv->tt = (8 +2); /* LUA_TUPVAL  (8+2) */
  /*            luaC_white(g); */
  uv->marked = (lu_byte)( g->currentwhite & ( (1<<0) | (1<<1) ));
  uv->v = level; /* current value lives in the stack */
  uv->next = *pp; /* chain it in the proper position */
  /*       ( (GCObject *)uv )  */
  *pp = obj2gco(uv);
  uv->u.l.prev = &g->uvhead; /* double link it in `uvhead' list */
  uv->u.l.next = g->uvhead.u.l.next;
  uv->u.l.next->u.l.prev = uv;
  g->uvhead.u.l.next = uv;
  ((void)0); /* lua_assert(uv->u.l.next->u.l.prev == uv && uv->u.l.prev->u.l.next == uv); */
  return uv;
}


static void unlinkupval (UpVal *uv) {
  ((void)0); /* lua_assert(uv->u.l.next->u.l.prev == uv && uv->u.l.prev->u.l.next == uv); */
  uv->u.l.next->u.l.prev = uv->u.l.prev; /* remove from `uvhead' list */
  uv->u.l.prev->u.l.next = uv->u.l.next;
}


void luaF_freeupval (lua_State *L, UpVal *uv) {
  if (uv->v != &uv->u.value) { /* is it open? */
    unlinkupval(uv); /* remove from open list */
  }
  /* luaM_free(L, uv); */
  luaM_realloc_(L, uv, sizeof(*uv), 0); /* free upvalue */
}


void luaF_close (lua_State *L, StkId level) {
  UpVal *uv;
  global_State *g = G(L);  /* (L->l_G); */
  /*                                    ngcotouv(L->openupval)              */
  while (L->openupval != NULL && (uv = (&((L->openupval)->uv)))->v >= level) {
    /*             obj2gco(uv); */
    GCObject *o = (GCObject *)uv;
    ((void)0); /* lua_assert(!isblack(o) && uv->v != &uv->u.value); */
    L->openupval = uv->next; /* remove from `open' list */
    /*
    isdead(g, o)
    ((o)->gch.marked & (g->currentwhite ^ ((1<<(0)) | (1<<(1)))) & ((1<<(0)) | (1<<(1))))

      o->gch.marked 
	& ( g->currentwhite ^ ( (1<<0) | (1<<1) ) ) 
	& ( (1<<0) | (1<<1) )
	*/
    if (  o->gch.marked 
	    & ( g->currentwhite ^ ( (1<<0) | (1<<1) ) ) 
	    & ( (1<<0) | (1<<1) )
	) {
      luaF_freeupval(L, uv); /* free upvalue */
    } else {
      unlinkupval(uv);

      /* { const TValue *o2=(uv->v); TValue *o1=(&uv->u.value); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
      setobj(L, &uv->u.value, uv->v);

      uv->v = &uv->u.value; /* now current value lives here */
      luaC_linkupval(L, uv); /* link upvalue into `gcroot' list */
    }
  }
}


Proto *luaF_newproto (lua_State *L) {
  /*          luaM_new(L, Proto); */
  Proto *f = (Proto *)( luaM_realloc_(L, NULL, 0, sizeof(Proto) ) );

  /*             obj2gco(f), LUA_TPROTO (8 +1)  */
  luaC_link(L, ((GCObject *)f), LUA_TPROTO);

  f->k = NULL;
  f->sizek = 0;
  f->p = NULL;
  f->sizep = 0;
  f->code = NULL;
  f->sizecode = 0;
  f->sizelineinfo = 0;
  f->sizeupvalues = 0;
  f->nups = 0;
  f->upvalues = NULL;
  f->numparams = 0;
  f->is_vararg = 0;
  f->maxstacksize = 0;
  f->lineinfo = NULL;
  f->sizelocvars = 0;
  f->locvars = NULL;
  f->linedefined = 0;
  f->lastlinedefined = 0;
  f->source = NULL;
  return f;
}


void luaF_freeproto (lua_State *L, Proto *f) {

  /* 
     #define luaM_freearray(L, b, n, t)   luaM_reallocv(L, (b), n, 0, sizeof(t)) 

     MAX_SIZET/(e)
    
  */

  /* luaM_freearray(L, f->code, f->sizecode, Instruction); */
  ( ( (size_t)1) <= MAX_SIZET/(sizeof(Instruction) ) 
   ? luaM_realloc_(L, f->code, f->sizecode * (sizeof(Instruction)), 0*( sizeof(Instruction) )) 
   : luaM_toobig(L)
  );

  /* luaM_freearray(L, f->p, f->sizep, Proto *); */
  ( ( (size_t)1) <= MAX_SIZET/(sizeof(Proto *) ) 
	? luaM_realloc_(L, f->p, f->sizep * (sizeof(Proto *)), 0 * ( sizeof(Proto *) )) 
	: luaM_toobig(L)
  );

  /* luaM_freearray(L, f->k, f->sizek, TValue); */
  ( ( (size_t)1) <= MAX_SIZET/(sizeof(TValue))
	? luaM_realloc_(L, f->k, f->sizek * (sizeof(TValue)), 0 * (sizeof(TValue)) ) 
	: luaM_toobig(L) );

  /* luaM_freearray(L, f->lineinfo, f->sizelineinfo, int); */
  ( ((size_t)1) <= MAX_SIZET/(sizeof(int)) 
	? luaM_realloc_(L, f->lineinfo,  f->sizelineinfo * (sizeof(int)), 0*(sizeof(int))) 
	: luaM_toobig(L) );

  /* luaM_freearray(L, f->locvars, f->sizelocvars, struct LocVar); */
  ( ((size_t)1) <= MAX_SIZET/(sizeof(struct LocVar)) 
   ? luaM_realloc_(L, f->locvars, f->sizelocvars * (sizeof(struct LocVar)), 0*(sizeof(struct LocVar))) 
   : luaM_toobig(L) );

  /* luaM_freearray(L, f->upvalues, f->sizeupvalues, TString *); */
  ( ((size_t)1) <= MAX_SIZET/(sizeof(TString *)) 
   ? luaM_realloc_(L, f->upvalues, f->sizeupvalues * (sizeof(TString *)), (0)*(sizeof(TString *))) 
   : luaM_toobig(L) );

  /* luaM_free(L, f); */
  luaM_realloc_(L, f, sizeof(*f), 0);
}


void luaF_freeclosure (lua_State *L, Closure *c) {
  /*
  int size = (c->c.isC) ? sizeCclosure(c->c.nupvalues) :
                          sizeLclosure(c->l.nupvalues);
  */
  /*                       sizeCclosure(c->c.nupvalues) */
  int size = (c->c.isC) ?  (int)( sizeof(CClosure) ) +  (int)(sizeof(TValue)  * (c->c.nupvalues-1)) :
  /*                       sizeLclosure(c->l.nupvalues) */
                           (int)( sizeof(LClosure) ) +  (int)(sizeof(TValue *) * (c->l.nupvalues-1));

  /* luaM_freemem(L, c, size); */
  luaM_realloc_(L, c, size, 0);
}


/*
** Look for n-th local variable at line `line' in function `func'.
** Returns NULL if not found.
*/
const char *luaF_getlocalname (const Proto *f, int local_number, int pc) {
  int i;
  for (i = 0; i<f->sizelocvars && f->locvars[i].startpc <= pc; i++) {
    if (pc < f->locvars[i].endpc) { /* is variable active? */
      local_number--;
      if (local_number == 0) {
        /*      getstr(f->locvars[i].varname);             */
        return ((const char *)((f->locvars[i].varname) + 1));
      }
    }
  }
  return NULL; /* not found */
}

