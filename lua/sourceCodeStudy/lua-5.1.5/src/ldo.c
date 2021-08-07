/*
** $Id: ldo.c,v 2.38.1.4 2012/01/18 02:27:10 roberto Exp $
** Stack and Call structure of Lua
** See Copyright Notice in lua.h
*/


#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#define ldo_c
#define LUA_CORE

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lundump.h"
#include "lvm.h"
#include "lzio.h"




/*
** {======================================================
** Error-recovery functions
** =======================================================
*/


/* chain list of long jump buffers */
struct lua_longjmp {
  struct lua_longjmp *previous;
  jmp_buf b;
  volatile int status; /* error code */
};


void luaD_seterrorobj (lua_State *L, int errcode, StkId oldtop) {
  switch (errcode) {
    case 4: {
      { TValue *i_o=(oldtop); i_o->value.gc=((GCObject *)(((luaS_newlstr(L, "" "not enough memory", (sizeof("not enough memory")/sizeof(char))-1))))); i_o->tt=4; ((void)0); };
      break;
    }
    case 5: {
      { TValue *i_o=(oldtop); i_o->value.gc=((GCObject *)(((luaS_newlstr(L, "" "error in error handling", (sizeof("error in error handling")/sizeof(char))-1))))); i_o->tt=4; ((void)0); };
      break;
    }
    case 3:
    case 2: {
      { const TValue *o2=(L->top - 1); TValue *o1=(oldtop); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; /* error message on current top */
      break;
    }
  }
  L->top = oldtop + 1;
}


static void restore_stack_limit (lua_State *L) {
  ((void)0);
  if (L->size_ci > 20000) { /* there was an overflow? */
    int inuse = ((int)((L->ci - L->base_ci)));
    if (inuse + 1 < 20000) { /* can `undo' overflow? */
      luaD_reallocCI(L, 20000);
    }
  }
}


static void resetstack (lua_State *L, int status) {
  L->ci = L->base_ci;
  L->base = L->ci->base;
  luaF_close(L, L->base); /* close eventual pending closures */
  luaD_seterrorobj(L, status, L->base);
  L->nCcalls = L->baseCcalls;
  L->allowhook = 1;
  restore_stack_limit(L);
  L->errfunc = 0;
  L->errorJmp = ((void *)0);
}


void luaD_throw (lua_State *L, int errcode) {
  if (L->errorJmp) {
    L->errorJmp->status = errcode;
    _longjmp((L->errorJmp)->b, 1);
  }
  else {
    L->status = ((lu_byte)((errcode)));
    if ((L->l_G)->panic) {
      resetstack(L, errcode);
      ((void) 0);
      (L->l_G)->panic(L);
    }
    exit(1);
  }
}


int luaD_rawrunprotected (lua_State *L, Pfunc f, void *ud) {
  struct lua_longjmp lj;
  lj.status = 0;
  lj.previous = L->errorJmp; /* chain new error handler */
  L->errorJmp = &lj;
  if (_setjmp((&lj)->b) == 0) { (*f)(L, ud); };
  L->errorJmp = lj.previous; /* restore old error handler */
  return lj.status;
}

/* }====================================================== */


static void correctstack (lua_State *L, TValue *oldstack) {
  CallInfo *ci;
  GCObject *up;
  L->top = (L->top - oldstack) + L->stack;
  for (up = L->openupval; up != ((void *)0); up = up->gch.next) {
    (&((up)->uv))->v = ((&((up)->uv))->v - oldstack) + L->stack;
  }
  for (ci = L->base_ci; ci <= L->ci; ci++) {
    ci->top = (ci->top - oldstack) + L->stack;
    ci->base = (ci->base - oldstack) + L->stack;
    ci->func = (ci->func - oldstack) + L->stack;
  }
  L->base = (L->base - oldstack) + L->stack;
}


void luaD_reallocstack (lua_State *L, int newsize) {
  TValue *oldstack = L->stack;
  int realsize = newsize + 1 + 5;
  ((void)0);
  ((L->stack)=((TValue *)(((((size_t)((realsize)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(TValue))) ? luaM_realloc_(L, (L->stack), (L->stacksize)*(sizeof(TValue)), (realsize)*(sizeof(TValue))) : luaM_toobig(L)))));
  L->stacksize = realsize;
  L->stack_last = L->stack+newsize;
  correctstack(L, oldstack);
}


void luaD_reallocCI (lua_State *L, int newsize) {
  CallInfo *oldci = L->base_ci;
  ((L->base_ci)=((CallInfo *)(((((size_t)((newsize)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(CallInfo))) ? luaM_realloc_(L, (L->base_ci), (L->size_ci)*(sizeof(CallInfo)), (newsize)*(sizeof(CallInfo))) : luaM_toobig(L)))));
  L->size_ci = newsize;
  L->ci = (L->ci - oldci) + L->base_ci;
  L->end_ci = L->base_ci + L->size_ci - 1;
}


void luaD_growstack (lua_State *L, int n) {
  if (n <= L->stacksize) { /* double size is enough? */
    luaD_reallocstack(L, 2*L->stacksize);
  } else {
    luaD_reallocstack(L, L->stacksize + n);
  }
}


static CallInfo *growCI (lua_State *L) {
  if (L->size_ci > 20000) { /* overflow while handling overflow? */
    luaD_throw(L, 5);
  } else {
    luaD_reallocCI(L, 2*L->size_ci);
    if (L->size_ci > 20000) {
      luaG_runerror(L, "stack overflow");
    }
  }
  return ++L->ci;
}


void luaD_callhook (lua_State *L, int event, int line) {
  lua_Hook hook = L->hook;
  if (hook && L->allowhook) {
    ptrdiff_t top = ((char *)(L->top) - (char *)L->stack);
    ptrdiff_t ci_top = ((char *)(L->ci->top) - (char *)L->stack);
    lua_Debug ar;
    ar.event = event;
    ar.currentline = line;
    if (event == 4) {
      ar.i_ci = 0; /* tail call; no debug information about it */
    } else {
      ar.i_ci = ((int)((L->ci - L->base_ci)));
    }
    if ((char *)L->stack_last - (char *)L->top <= (20)*(int)sizeof(TValue)) luaD_growstack(L, 20); else ((void)0);; /* ensure minimum stack size */
    L->ci->top = L->top + 20;
    ((void)0);
    L->allowhook = 0; /* cannot call hooks inside a hook */
    ((void) 0);
    (*hook)(L, &ar);
    ((void) 0);
    ((void)0);
    L->allowhook = 1;
    L->ci->top = ((TValue *)((char *)L->stack + (ci_top)));
    L->top = ((TValue *)((char *)L->stack + (top)));
  }
}


static StkId adjust_varargs (lua_State *L, Proto *p, int actual) {
  int i;
  int nfixargs = p->numparams;
  Table *htab = ((void *)0);
  StkId base, fixed;
  for (; actual < nfixargs; ++actual) {
    ((L->top++)->tt=0);
   }
  if (p->is_vararg & 4) { /* compat. with old-style vararg? */
    int nvar = actual - nfixargs; /* number of extra arguments */
    ((void)0);
    { ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };
    if ((char *)L->stack_last - (char *)L->top <= (p->maxstacksize)*(int)sizeof(TValue)) luaD_growstack(L, p->maxstacksize); else ((void)0);;
    htab = luaH_new(L, nvar, 1); /* create `arg' table */
    for (i=0; i<nvar; i++) { /* put extra arguments into `arg' table */
      { const TValue *o2=(L->top - nvar + i); TValue *o1=(luaH_setnum(L, htab, i+1)); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
    }
    /* store counter in field `n' */
    { TValue *i_o=(luaH_setstr(L, htab, (luaS_newlstr(L, "" "n", (sizeof("n")/sizeof(char))-1)))); i_o->value.n=(((lua_Number)((nvar)))); i_o->tt=3; };
  }
  /* move fixed parameters to final position */
  fixed = L->top - actual; /* first fixed argument */
  base = L->top; /* final position of first argument */
  for (i=0; i<nfixargs; i++) {
    { const TValue *o2=(fixed+i); TValue *o1=(L->top++); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
    ((fixed+i)->tt=0);
  }
  /* add `arg' parameter */
  if (htab) {
    { TValue *i_o=(L->top++); i_o->value.gc=((GCObject *)((htab))); i_o->tt=5; ((void)0); };
    ((void)0);
  }
  return base;
}


static StkId tryfuncTM (lua_State *L, StkId func) {
  const TValue *tm = luaT_gettmbyobj(L, func, TM_CALL);
  StkId p;
  ptrdiff_t funcr = ((char *)(func) - (char *)L->stack);
  if (!(((tm)->tt) == 6)) {
    luaG_typeerror(L, func, "call");
  }
  /* Open a hole inside the stack at `func' */
  for (p = L->top; p > func; p--) {
    { const TValue *o2=(p-1); TValue *o1=(p); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
  }
  {if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) luaD_growstack(L, 1); else ((void)0);; L->top++;};
  func = ((TValue *)((char *)L->stack + (funcr))); /* previous call may change stack */
  { const TValue *o2=(tm); TValue *o1=(func); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; /* tag method is the new function to be called */
  return func;
}


int luaD_precall (lua_State *L, StkId func, int nresults) {
  LClosure *cl;
  ptrdiff_t funcr;
  if (!(((func)->tt) == 6)) { /* `func' is not a function? */
    func = tryfuncTM(L, func); /* check the `function' tag method */
  }
  funcr = ((char *)(func) - (char *)L->stack);
  cl = &(&(func)->value.gc->cl)->l;
  L->ci->savedpc = L->savedpc;
  if (!cl->isC) { /* Lua function? prepare its call */
    CallInfo *ci;
    StkId st, base;
    Proto *p = cl->p;
    if ((char *)L->stack_last - (char *)L->top <= (p->maxstacksize)*(int)sizeof(TValue)) luaD_growstack(L, p->maxstacksize); else ((void)0);;
    func = ((TValue *)((char *)L->stack + (funcr)));
    if (!p->is_vararg) { /* no varargs? */
      base = func + 1;
      if (L->top > base + p->numparams) {
        L->top = base + p->numparams;
      }
    }
    else { /* vararg function */
      int nargs = ((int)((L->top - func))) - 1;
      base = adjust_varargs(L, p, nargs);
      func = ((TValue *)((char *)L->stack + (funcr))); /* previous call may change the stack */
    }
    ci = ((L->ci == L->end_ci) ? growCI(L) : (((void)0), ++L->ci)); /* now `enter' new function */
    ci->func = func;
    L->base = ci->base = base;
    ci->top = L->base + p->maxstacksize;
    ((void)0);
    L->savedpc = p->code; /* starting point */
    ci->tailcalls = 0;
    ci->nresults = nresults;
    for (st = L->top; st < ci->top; st++) {
      ((st)->tt=0);
    }
    L->top = ci->top;
    if (L->hookmask & (1 << 0)) {
      L->savedpc++; /* hooks assume 'pc' is already incremented */
      luaD_callhook(L, 0, -1);
      L->savedpc--; /* correct 'pc' */
    }
    return 0;
  }
  else { /* if is a C function, call it */
    CallInfo *ci;
    int n;
    if ((char *)L->stack_last - (char *)L->top <= (20)*(int)sizeof(TValue)) luaD_growstack(L, 20); else ((void)0);; /* ensure minimum stack size */
    ci = ((L->ci == L->end_ci) ? growCI(L) : (((void)0), ++L->ci)); /* now `enter' new function */
    ci->func = ((TValue *)((char *)L->stack + (funcr)));
    L->base = ci->base = ci->func + 1;
    ci->top = L->top + 20;
    ((void)0);
    ci->nresults = nresults;
    if (L->hookmask & (1 << 0)) {
      luaD_callhook(L, 0, -1);
    }
    ((void) 0);
    n = (*((&(L->ci->func)->value.gc->cl))->c.f)(L); /* do the actual call */
    ((void) 0);
    if (n < 0) { /* yielding? */
      return 2;
    } else {
      luaD_poscall(L, L->top - n);
      return 1;
    }
  }
}


static StkId callrethooks (lua_State *L, StkId firstResult) {
  ptrdiff_t fr = ((char *)(firstResult) - (char *)L->stack); /* next call may change stack */
  luaD_callhook(L, 1, -1);
  if ((!((&((L->ci)->func)->value.gc->cl))->c.isC)) { /* Lua function? */
    while ((L->hookmask & (1 << 1)) && L->ci->tailcalls--) { /* tail calls */
      luaD_callhook(L, 4, -1);
    }
  }
  return ((TValue *)((char *)L->stack + (fr)));
}


int luaD_poscall (lua_State *L, StkId firstResult) {
  StkId res;
  int wanted, i;
  CallInfo *ci;
  if (L->hookmask & (1 << 1)) {
    firstResult = callrethooks(L, firstResult);
  }
  ci = L->ci--;
  res = ci->func; /* res == final position of 1st result */
  wanted = ci->nresults;
  L->base = (ci - 1)->base; /* restore base */
  L->savedpc = (ci - 1)->savedpc; /* restore savedpc */
  /* move results to correct place */
  for (i = wanted; i != 0 && firstResult < L->top; i--) {
    { const TValue *o2=(firstResult++); TValue *o1=(res++); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
  }
  while (i-- > 0) {
    ((res++)->tt=0);
  }
  L->top = res;
  return (wanted - (-1)); /* 0 iff wanted == LUA_MULTRET */
}


/*
** Call a function (C or Lua). The function to be called is at *func.
** The arguments are on the stack, right after the function.
** When returns, all the results are on the stack, starting at the original
** function position.
*/
void luaD_call (lua_State *L, StkId func, int nResults) {
  if (++L->nCcalls >= 200) {
    if (L->nCcalls == 200) {
      luaG_runerror(L, "C stack overflow");
    }
    else if (L->nCcalls >= (200 + (200>>3))) {
      luaD_throw(L, 5); /* error while handing stack error */
    }
  }
  if (luaD_precall(L, func, nResults) == 0) { /* is a Lua function? */
    luaV_execute(L, 1); /* call it */
  }
  L->nCcalls--;
  { ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };
}


static void resume (lua_State *L, void *ud) {
  StkId firstArg = ((StkId)(ud));
  CallInfo *ci = L->ci;
  if (L->status == 0) { /* start coroutine? */
    ((void)0);
    if (luaD_precall(L, firstArg - 1, (-1)) != 0) {
      return;
    }
  }
  else { /* resuming from previous yield */
    ((void)0);
    L->status = 0;
    if (!(!((&((ci)->func)->value.gc->cl))->c.isC)) { /* `common' yield? */
      /* finish interrupted execution of `OP_CALL' */
      ((void)0);
      if (luaD_poscall(L, firstArg)) { /* complete it... */
        L->top = L->ci->top; /* and correct top if not multiple results */
      }
    }
    else { /* yielded inside a hook: just continue its execution */
      L->base = L->ci->base;
    }
  }
  luaV_execute(L, ((int)((L->ci - L->base_ci))));
}


static int resume_error (lua_State *L, const char *msg) {
  L->top = L->ci->base;
  { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)(((luaS_newlstr(L, msg, strlen(msg)))))); i_o->tt=4; ((void)0); };
  {if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) luaD_growstack(L, 1); else ((void)0);; L->top++;};
  ((void) 0);
  return 2;
}


extern int lua_resume (lua_State *L, int nargs) {
  int status;
  ((void) 0);
  if (L->status != 1 && (L->status != 0 || L->ci != L->base_ci)) {
      return resume_error(L, "cannot resume non-suspended coroutine");
  }
  if (L->nCcalls >= 200) {
    return resume_error(L, "C stack overflow");
  }
  ((void)L);
  ((void)0);
  L->baseCcalls = ++L->nCcalls;
  status = luaD_rawrunprotected(L, resume, L->top - nargs);
  if (status != 0) { /* error? */
    L->status = ((lu_byte)((status))); /* mark thread as `dead' */
    luaD_seterrorobj(L, status, L->top);
    L->ci->top = L->top;
  }
  else {
    ((void)0);
    status = L->status;
  }
  --L->nCcalls;
  ((void) 0);
  return status;
}


extern int lua_yield (lua_State *L, int nresults) {
  ((void)L);
  ((void) 0);
  if (L->nCcalls > L->baseCcalls) {
    luaG_runerror(L, "attempt to yield across metamethod/C-call boundary");
  }
  L->base = L->top - nresults; /* protect stack slots below */
  L->status = 1;
  ((void) 0);
  return -1;
}


int luaD_pcall (lua_State *L, Pfunc func, void *u,
                ptrdiff_t old_top, ptrdiff_t ef) {
  int status;
  unsigned short oldnCcalls = L->nCcalls;
  ptrdiff_t old_ci = ((char *)(L->ci) - (char *)L->base_ci);
  lu_byte old_allowhooks = L->allowhook;
  ptrdiff_t old_errfunc = L->errfunc;
  L->errfunc = ef;
  status = luaD_rawrunprotected(L, func, u);
  if (status != 0) { /* an error occurred? */
    StkId oldtop = ((TValue *)((char *)L->stack + (old_top)));
    luaF_close(L, oldtop); /* close eventual pending closures */
    luaD_seterrorobj(L, status, oldtop);
    L->nCcalls = oldnCcalls;
    L->ci = ((CallInfo *)((char *)L->base_ci + (old_ci)));
    L->base = L->ci->base;
    L->savedpc = L->ci->savedpc;
    L->allowhook = old_allowhooks;
    restore_stack_limit(L);
  }
  L->errfunc = old_errfunc;
  return status;
}


/*
** Execute a protected parser.
*/
struct SParser { /* data to `f_parser' */
  ZIO *z;
  Mbuffer buff; /* buffer to be used by the scanner */
  const char *name;
};

static void f_parser (lua_State *L, void *ud) {
  int i;
  Proto *tf;
  Closure *cl;
  struct SParser *p = ((struct SParser *)(ud));
  int c = luaZ_lookahead(p->z);
  { ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };
  tf = ((c == "\033Lua"[0]) ? luaU_undump : luaY_parser)(L, p->z,
                                                             &p->buff, p->name);
  cl = luaF_newLclosure(L, tf->nups, (&((&L->l_gt))->value.gc->h));
  cl->l.p = tf;
  for (i = 0; i < tf->nups; i++) { /* initialize eventual upvalues */
    cl->l.upvals[i] = luaF_newupval(L);
  }
  { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((cl))); i_o->tt=6; ((void)0); };
  {if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) luaD_growstack(L, 1); else ((void)0);; L->top++;};
}


int luaD_protectedparser (lua_State *L, ZIO *z, const char *name) {
  struct SParser p;
  int status;
  p.z = z; p.name = name;
  ((&p.buff)->buffer = ((void *)0), (&p.buff)->buffsize = 0);
  status = luaD_pcall(L, f_parser, &p, ((char *)(L->top) - (char *)L->stack), L->errfunc);
  ((((&p.buff)->buffer)=((char *)(((((size_t)((0)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(char))) ? luaM_realloc_(L, ((&p.buff)->buffer), ((&p.buff)->buffsize)*(sizeof(char)), (0)*(sizeof(char))) : luaM_toobig(L))))), (&p.buff)->buffsize = 0);
  return status;
}
