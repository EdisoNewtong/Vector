/*
** $Id: lstate.c,v 2.36.1.2 2008/01/03 15:20:39 roberto Exp $
** Global State
** See Copyright Notice in lua.h
*/


#include <stddef.h>

#define lstate_c
#define LUA_CORE

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "llex.h"
#include "lmem.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"


#define state_size(x)	(sizeof(x) + LUAI_EXTRASPACE)
#define fromstate(l)	(cast(lu_byte *, (l)) - LUAI_EXTRASPACE)
#define tostate(l)   (cast(lua_State *, cast(lu_byte *, l) + LUAI_EXTRASPACE))


/*
** Main thread combines a thread state and the global state
*/
typedef struct LG {
  lua_State l;
  global_State g;
} LG;


static void stack_init (lua_State *L1, lua_State *L) {
  /* initialize CallInfo array */
  L1->base_ci = ((CallInfo *)(((((size_t)((8)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(CallInfo))) ? luaM_realloc_(L, (((void *)0)), (0)*(sizeof(CallInfo)), (8)*(sizeof(CallInfo))) : luaM_toobig(L))));
  L1->ci = L1->base_ci;
  L1->size_ci = 8;
  L1->end_ci = L1->base_ci + L1->size_ci - 1;
  /* initialize stack array */
  L1->stack = ((TValue *)(((((size_t)(((2*20) + 5)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(TValue))) ? luaM_realloc_(L, (((void *)0)), (0)*(sizeof(TValue)), ((2*20) + 5)*(sizeof(TValue))) : luaM_toobig(L))));
  L1->stacksize = (2*20) + 5;
  L1->top = L1->stack;
  L1->stack_last = L1->stack+(L1->stacksize - 5)-1;
  /* initialize first ci */
  L1->ci->func = L1->top;
  ((L1->top++)->tt=0); /* `function' entry for this `ci' */
  L1->base = L1->ci->base = L1->top;
  L1->ci->top = L1->top + 20;
}


static void freestack (lua_State *L, lua_State *L1) {
  ((((size_t)((0)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(CallInfo))) ? luaM_realloc_(L, ((L1->base_ci)), (L1->size_ci)*(sizeof(CallInfo)), (0)*(sizeof(CallInfo))) : luaM_toobig(L));
  ((((size_t)((0)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(TValue))) ? luaM_realloc_(L, ((L1->stack)), (L1->stacksize)*(sizeof(TValue)), (0)*(sizeof(TValue))) : luaM_toobig(L));
}


/*
** open parts that may cause memory-allocation errors
*/
static void f_luaopen (lua_State *L, void *ud) {
  global_State *g = (L->l_G);
  ((void)(ud));
  stack_init(L, L); /* init stack */
  { TValue *i_o=((&L->l_gt)); i_o->value.gc=((GCObject *)((luaH_new(L, 0, 2)))); i_o->tt=5; ((void)0); }; /* table of globals */
  { TValue *i_o=((&(L->l_G)->l_registry)); i_o->value.gc=((GCObject *)((luaH_new(L, 0, 2)))); i_o->tt=5; ((void)0); }; /* registry */
  luaS_resize(L, 32); /* initial size of string table */
  luaT_init(L);
  luaX_init(L);
  ((((luaS_newlstr(L, "" "not enough memory", (sizeof("not enough memory")/sizeof(char))-1)))->tsv.marked) |= ((1<<(5))));
  g->GCthreshold = 4*g->totalbytes;
}


static void preinit_state (lua_State *L, global_State *g) {
  (L->l_G) = g;
  L->stack = ((void *)0);
  L->stacksize = 0;
  L->errorJmp = ((void *)0);
  L->hook = ((void *)0);
  L->hookmask = 0;
  L->basehookcount = 0;
  L->allowhook = 1;
  (L->hookcount = L->basehookcount);
  L->openupval = ((void *)0);
  L->size_ci = 0;
  L->nCcalls = L->baseCcalls = 0;
  L->status = 0;
  L->base_ci = L->ci = ((void *)0);
  L->savedpc = ((void *)0);
  L->errfunc = 0;
  (((&L->l_gt))->tt=0);
}


static void close_state (lua_State *L) {
  global_State *g = (L->l_G);
  luaF_close(L, L->stack); /* close all upvalues for this thread */
  luaC_freeall(L); /* collect all objects */
  ((void)0);
  ((void)0);
  ((((size_t)((0)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(TString *))) ? luaM_realloc_(L, (((L->l_G)->strt.hash)), ((L->l_G)->strt.size)*(sizeof(TString *)), (0)*(sizeof(TString *))) : luaM_toobig(L));
  ((((&g->buff)->buffer)=((char *)(((((size_t)((0)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(char))) ? luaM_realloc_(L, ((&g->buff)->buffer), ((&g->buff)->buffsize)*(sizeof(char)), (0)*(sizeof(char))) : luaM_toobig(L))))), (&g->buff)->buffsize = 0);
  freestack(L, L);
  ((void)0);
  (*g->frealloc)(g->ud, (((lu_byte *)((L))) - 0), (sizeof(LG) + 0), 0);
}


lua_State *luaE_newthread (lua_State *L) {
  lua_State *L1 = (((lua_State *)(((lu_byte *)(luaM_realloc_(L, ((void *)0), 0, ((sizeof(lua_State) + 0))))) + 0)));
  luaC_link(L, (((GCObject *)((L1)))), 8);
  preinit_state(L1, (L->l_G));
  stack_init(L1, L); /* init stack */
  { const TValue *o2=((&L->l_gt)); TValue *o1=((&L1->l_gt)); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; /* share table of globals */
  L1->hookmask = L->hookmask;
  L1->basehookcount = L->basehookcount;
  L1->hook = L->hook;
  (L1->hookcount = L1->basehookcount);
  ((void)0);
  return L1;
}


void luaE_freethread (lua_State *L, lua_State *L1) {
  luaF_close(L1, L1->stack); /* close all upvalues for this thread */
  ((void)0);
  ((void)L1);
  freestack(L, L1);
  luaM_realloc_(L, ((((lu_byte *)((L1))) - 0)), ((sizeof(lua_State) + 0)), 0);
}


extern lua_State *lua_newstate (lua_Alloc f, void *ud) {
  int i;
  lua_State *L;
  global_State *g;
  void *l = (*f)(ud, ((void *)0), 0, (sizeof(LG) + 0));
  if (l == ((void *)0)) {
    return ((void *)0);
  }
  L = (((lua_State *)(((lu_byte *)(l)) + 0)));
  g = &((LG *)L)->g;
  L->next = ((void *)0);
  L->tt = 8;
  g->currentwhite = ((1<<(0)) | (1<<(5)));
  L->marked = ((lu_byte)((g)->currentwhite & ((1<<(0)) | (1<<(1)))));
  ((L->marked) |= ((((1<<(5)) | (1<<(6))))));
  preinit_state(L, g);
  g->frealloc = f;
  g->ud = ud;
  g->mainthread = L;
  g->uvhead.u.l.prev = &g->uvhead;
  g->uvhead.u.l.next = &g->uvhead;
  g->GCthreshold = 0; /* mark it as unfinished state */
  g->strt.size = 0;
  g->strt.nuse = 0;
  g->strt.hash = ((void *)0);
  (((&(L->l_G)->l_registry))->tt=0);
  ((&g->buff)->buffer = ((void *)0), (&g->buff)->buffsize = 0);
  g->panic = ((void *)0);
  g->gcstate = 0;
  g->rootgc = (((GCObject *)((L))));
  g->sweepstrgc = 0;
  g->sweepgc = &g->rootgc;
  g->gray = ((void *)0);
  g->grayagain = ((void *)0);
  g->weak = ((void *)0);
  g->tmudata = ((void *)0);
  g->totalbytes = sizeof(LG);
  g->gcpause = 200;
  g->gcstepmul = 200;
  g->gcdept = 0;
  for (i=0; i<(8 +1); i++) {
    g->mt[i] = ((void *)0);
  }
  if (luaD_rawrunprotected(L, f_luaopen, ((void *)0)) != 0) {
    /* memory allocation error: free partial state */
    close_state(L);
    L = ((void *)0);
  }
  else {
    ((void)L);
  }
  return L;
}


static void callallgcTM (lua_State *L, void *ud) {
  ((void)(ud));
  luaC_callGCTM(L); /* call GC metamethods for all udata */
}


extern void lua_close (lua_State *L) {
  L = (L->l_G)->mainthread; /* only the main thread can be closed */
  ((void) 0);
  luaF_close(L, L->stack); /* close all upvalues for this thread */
  luaC_separateudata(L, 1); /* separate udata that have GC metamethods */
  L->errfunc = 0; /* no error function during GC metamethods */
  do { /* repeat until no more errors */
    L->ci = L->base_ci;
    L->base = L->top = L->ci->base;
    L->nCcalls = L->baseCcalls = 0;
  } while (luaD_rawrunprotected(L, callallgcTM, ((void *)0)) != 0);
  ((void)0);
  ((void)L);
  close_state(L);
}


