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



Closure *luaF_newCclosure (lua_State *L, int nelems, Table *e) {
  Closure *c = ((Closure *)(luaM_realloc_(L, ((void *)0), 0, ((((int)(sizeof(CClosure))) + ((int)(sizeof(TValue)*((nelems)-1))))))));
  luaC_link(L, (((GCObject *)((c)))), 6);
  c->c.isC = 1;
  c->c.env = e;
  c->c.nupvalues = ((lu_byte)((nelems)));
  return c;
}


Closure *luaF_newLclosure (lua_State *L, int nelems, Table *e) {
  Closure *c = ((Closure *)(luaM_realloc_(L, ((void *)0), 0, ((((int)(sizeof(LClosure))) + ((int)(sizeof(TValue *)*((nelems)-1))))))));
  luaC_link(L, (((GCObject *)((c)))), 6);
  c->l.isC = 0;
  c->l.env = e;
  c->l.nupvalues = ((lu_byte)((nelems)));
  while (nelems--) {
    c->l.upvals[nelems] = ((void *)0);
  }
  return c;
}


UpVal *luaF_newupval (lua_State *L) {
  UpVal *uv = ((UpVal *)(luaM_realloc_(L, ((void *)0), 0, (sizeof(UpVal)))));
  luaC_link(L, (((GCObject *)((uv)))), (8 +2));
  uv->v = &uv->u.value;
  ((uv->v)->tt=0);
  return uv;
}


UpVal *luaF_findupval (lua_State *L, StkId level) {
  global_State *g = (L->l_G);
  GCObject **pp = &L->openupval;
  UpVal *p;
  UpVal *uv;
  while (*pp != ((void *)0) && (p = (&((*pp)->uv)))->v >= level) {
    ((void)0);
    if (p->v == level) { /* found a corresponding upvalue? */
      if ((((((GCObject *)((p)))))->gch.marked & (g->currentwhite ^ ((1<<(0)) | (1<<(1)))) & ((1<<(0)) | (1<<(1))))) { /* is it dead? */
        (((((GCObject *)((p)))))->gch.marked ^= ((1<<(0)) | (1<<(1)))); /* ressurect it */
      }
      return p;
    }
    pp = &p->next;
  }
  uv = ((UpVal *)(luaM_realloc_(L, ((void *)0), 0, (sizeof(UpVal))))); /* not found: create a new one */
  uv->tt = (8 +2);
  uv->marked = ((lu_byte)((g)->currentwhite & ((1<<(0)) | (1<<(1)))));
  uv->v = level; /* current value lives in the stack */
  uv->next = *pp; /* chain it in the proper position */
  *pp = (((GCObject *)((uv))));
  uv->u.l.prev = &g->uvhead; /* double link it in `uvhead' list */
  uv->u.l.next = g->uvhead.u.l.next;
  uv->u.l.next->u.l.prev = uv;
  g->uvhead.u.l.next = uv;
  ((void)0);
  return uv;
}


static void unlinkupval (UpVal *uv) {
  ((void)0);
  uv->u.l.next->u.l.prev = uv->u.l.prev; /* remove from `uvhead' list */
  uv->u.l.prev->u.l.next = uv->u.l.next;
}


void luaF_freeupval (lua_State *L, UpVal *uv) {
  if (uv->v != &uv->u.value) { /* is it open? */
    unlinkupval(uv); /* remove from open list */
  }
  luaM_realloc_(L, (uv), sizeof(*(uv)), 0); /* free upvalue */
}


void luaF_close (lua_State *L, StkId level) {
  UpVal *uv;
  global_State *g = (L->l_G);
  while (L->openupval != ((void *)0) && (uv = (&((L->openupval)->uv)))->v >= level) {
    GCObject *o = (((GCObject *)((uv))));
    ((void)0);
    L->openupval = uv->next; /* remove from `open' list */
    if (((o)->gch.marked & (g->currentwhite ^ ((1<<(0)) | (1<<(1)))) & ((1<<(0)) | (1<<(1))))) {
      luaF_freeupval(L, uv); /* free upvalue */
    } else {
      unlinkupval(uv);
      { const TValue *o2=(uv->v); TValue *o1=(&uv->u.value); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
      uv->v = &uv->u.value; /* now current value lives here */
      luaC_linkupval(L, uv); /* link upvalue into `gcroot' list */
    }
  }
}


Proto *luaF_newproto (lua_State *L) {
  Proto *f = ((Proto *)(luaM_realloc_(L, ((void *)0), 0, (sizeof(Proto)))));
  luaC_link(L, (((GCObject *)((f)))), (8 +1));
  f->k = ((void *)0);
  f->sizek = 0;
  f->p = ((void *)0);
  f->sizep = 0;
  f->code = ((void *)0);
  f->sizecode = 0;
  f->sizelineinfo = 0;
  f->sizeupvalues = 0;
  f->nups = 0;
  f->upvalues = ((void *)0);
  f->numparams = 0;
  f->is_vararg = 0;
  f->maxstacksize = 0;
  f->lineinfo = ((void *)0);
  f->sizelocvars = 0;
  f->locvars = ((void *)0);
  f->linedefined = 0;
  f->lastlinedefined = 0;
  f->source = ((void *)0);
  return f;
}


void luaF_freeproto (lua_State *L, Proto *f) {
  ((((size_t)((0)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(Instruction))) ? luaM_realloc_(L, ((f->code)), (f->sizecode)*(sizeof(Instruction)), (0)*(sizeof(Instruction))) : luaM_toobig(L));
  ((((size_t)((0)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(Proto *))) ? luaM_realloc_(L, ((f->p)), (f->sizep)*(sizeof(Proto *)), (0)*(sizeof(Proto *))) : luaM_toobig(L));
  ((((size_t)((0)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(TValue))) ? luaM_realloc_(L, ((f->k)), (f->sizek)*(sizeof(TValue)), (0)*(sizeof(TValue))) : luaM_toobig(L));
  ((((size_t)((0)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(int))) ? luaM_realloc_(L, ((f->lineinfo)), (f->sizelineinfo)*(sizeof(int)), (0)*(sizeof(int))) : luaM_toobig(L));
  ((((size_t)((0)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(struct LocVar))) ? luaM_realloc_(L, ((f->locvars)), (f->sizelocvars)*(sizeof(struct LocVar)), (0)*(sizeof(struct LocVar))) : luaM_toobig(L));
  ((((size_t)((0)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(TString *))) ? luaM_realloc_(L, ((f->upvalues)), (f->sizeupvalues)*(sizeof(TString *)), (0)*(sizeof(TString *))) : luaM_toobig(L));
  luaM_realloc_(L, (f), sizeof(*(f)), 0);
}


void luaF_freeclosure (lua_State *L, Closure *c) {
  int size = (c->c.isC) ? (((int)(sizeof(CClosure))) + ((int)(sizeof(TValue)*((c->c.nupvalues)-1)))) :
                          (((int)(sizeof(LClosure))) + ((int)(sizeof(TValue *)*((c->l.nupvalues)-1))));
  luaM_realloc_(L, (c), (size), 0);
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
        return ((const char *)((f->locvars[i].varname) + 1));
      }
    }
  }
  return ((void *)0); /* not found */
}

