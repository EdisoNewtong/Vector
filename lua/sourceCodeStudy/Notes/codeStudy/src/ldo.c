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
  jmp_buf b;      /*  luai_jmpbuf b; */
  volatile int status; /* error code */
};


void luaD_seterrorobj (lua_State *L, int errcode, StkId oldtop) {
  switch (errcode) {
    case LUA_ERRMEM: { /* LUA_ERRMEM   4 */
      /* { TValue *i_o=(oldtop); i_o->value.gc=((GCObject *)(((luaS_newlstr(L, "" "not enough memory", (sizeof("not enough memory")/sizeof(char))-1))))); i_o->tt= LUA_TSTRING; ((void)0); }; */
      setsvalue2s(L, oldtop, luaS_newliteral(L, MEMERRMSG));
      break;
    }
    case LUA_ERRERR: { /*  LUA_ERRERR 5 */
      /* { TValue *i_o=(oldtop); i_o->value.gc=((GCObject *)(((luaS_newlstr(L, "" "error in error handling", (sizeof("error in error handling")/sizeof(char))-1))))); i_o->tt = LUA_TSTRING; ((void)0); }; */
      setsvalue2s(L, oldtop, luaS_newliteral(L, "error in error handling"));
      break;
    }
	case LUA_ERRSYNTAX:  /* LUA_ERRSYNTAX 3 */
	case LUA_ERRRUN:   { /*  LUA_ERRRUN   2 */
      /* { const TValue *o2=(L->top - 1); TValue *o1=(oldtop); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
      setobjs2s(L, oldtop, L->top - 1);  /* error message on current top */
      break;
    }
  }
  L->top = oldtop + 1;
}


static void restore_stack_limit (lua_State *L) {
  ((void)0); /* lua_assert(L->stack_last - L->stack == L->stacksize - EXTRA_STACK - 1); */
  /*               LUAI_MAXCALLS  20000 */
  if (L->size_ci > LUAI_MAXCALLS ) { /* there was an overflow? */
    /*            ((int)((L->ci - L->base_ci)));     */
    int inuse = cast_int(L->ci - L->base_ci); 
    /*              LUAI_MAXCALLS  20000 */
    if ( inuse + 1 < LUAI_MAXCALLS ) { /* can `undo' overflow? */
      luaD_reallocCI(L, LUAI_MAXCALLS);  /* LUAI_MAXCALLS 20000 */
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
  L->errorJmp = NULL;
}


void luaD_throw (lua_State *L, int errcode) {
  if (L->errorJmp) {
    L->errorJmp->status = errcode;
    /* _longjmp((L->errorJmp)->b, 1); */
    LUAI_THROW(L, L->errorJmp);
  }
  else {
    /*           cast_byte(errcode); */
    L->status = ((lu_byte)((errcode)));
    /*  (L->l_G)->panic    */
    if (G(L)->panic) {
      resetstack(L, errcode);
      ((void) 0); /* lua_unlock(L); */
      /* (L->l_G)->panic(L)    */
      G(L)->panic(L);
    }
    /*   EXIT_FAILURE  1; */
    exit(EXIT_FAILURE);
  }
}


int luaD_rawrunprotected (lua_State *L, Pfunc f, void *ud) {
  struct lua_longjmp lj;
  lj.status = 0;
  lj.previous = L->errorJmp; /* chain new error handler */
  L->errorJmp = &lj;
  /*
       LUAI_TRY(L, &lj,
         (*f)(L, ud);
       );
  */

  if ( _setjmp( (&lj)->b ) == 0 ) { 
	  (*f)(L, ud); 
  };

  L->errorJmp = lj.previous; /* restore old error handler */
  return lj.status;
}

/* }====================================================== */


static void correctstack (lua_State *L, TValue *oldstack) {
  CallInfo *ci;
  GCObject *up;
  L->top = (L->top - oldstack) + L->stack;
  for (up = L->openupval; up != NULL; up = up->gch.next) {
    /* (&((up)->uv))->v = ((&((up)->uv))->v - oldstack) + L->stack; */
    gco2uv(up)->v = (gco2uv(up)->v - oldstack) + L->stack;
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
  /*                           EXTRA_STACK 5 */
  int realsize = newsize + 1 + EXTRA_STACK;
  ((void)0); /* lua_assert(L->stack_last - L->stack == L->stacksize - EXTRA_STACK - 1); */
  /* 
     LuaM_reallocvector(L, L->stack, L->stacksize, realsize, TValue);

     #define MAX_SIZET	((size_t)(~(size_t)0)-2)
	
  */
  L->stack = (TValue *)
	           (
		           ( (  ( (size_t)(realsize+1)) <= MAX_SIZET / sizeof(TValue) ) 
			       ? luaM_realloc_(L, L->stack, L->stacksize*sizeof(TValue), realsize*sizeof(TValue) ) 
			       : luaM_toobig(L) )
			   );
  L->stacksize = realsize;
  L->stack_last = L->stack+newsize;
  correctstack(L, oldstack);
}


void luaD_reallocCI (lua_State *L, int newsize) {
  CallInfo *oldci = L->base_ci;
  /* luaM_reallocvector(L, L->base_ci, L->size_ci, newsize, CallInfo); */
  L->base_ci = (CallInfo *)(
		                     ( ((size_t)(newsize+1)) <= MAX_SIZET / sizeof(CallInfo) ) 
		                     ? luaM_realloc_(L, L->base_ci, L->size_ci*sizeof(CallInfo), newsize*sizeof(CallInfo) ) 
		                     : luaM_toobig(L)
                           );
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
  /*               LUAI_MAXCALLS  20000 */
  if (L->size_ci > LUAI_MAXCALLS ) { /* overflow while handling overflow? */
    luaD_throw(L, LUA_ERRERR); /* LUA_ERRERR 5 */
  } else {
    luaD_reallocCI(L, 2*L->size_ci);
    /*               LUAI_MAXCALLS  20000 */
    if (L->size_ci > LUAI_MAXCALLS ) {
      luaG_runerror(L, "stack overflow");
    }
  }
  return ++L->ci;
}


void luaD_callhook (lua_State *L, int event, int line) {
  lua_Hook hook = L->hook;
  if (hook && L->allowhook) {
    /*              ((char *)(L->top) - (char *)L->stack)  */
    ptrdiff_t top = savestack(L, L->top);
    /*                 ((char *)(L->ci->top) - (char *)L->stack)  */
    ptrdiff_t ci_top = savestack(L, L->ci->top);
    lua_Debug ar;
    ar.event = event;
    ar.currentline = line;
    if ( event == LUA_HOOKTAILRET ) { /* LUA_HOOKTAILRET 4 */
      ar.i_ci = 0; /* tail call; no debug information about it */
    } else {
      /*        ((int)((L->ci - L->base_ci)))   */
      ar.i_ci = cast_int(L->ci - L->base_ci);
    }

    /* luaD_checkstack(L, LUA_MINSTACK); */
	/*   LUA_MINSTACK   20               */
    /* ensure minimum stack size         */
    if ((char *)L->stack_last - (char *)L->top <= LUA_MINSTACK * (int)sizeof(TValue) ) { 
		luaD_growstack(L, LUA_MINSTACK); 
	} else {
		((void)0); /* ensure minimum stack size */
	}
    /*                    LUA_MINSTACK  20      */
    L->ci->top = L->top + LUA_MINSTACK;
    ((void)0); /* lua_assert(L->ci->top <= L->stack_last); */
    L->allowhook = 0; /* cannot call hooks inside a hook */
    ((void) 0); /* lua_unlock(L); */
    (*hook)(L, &ar);
    ((void) 0); /* lua_lock(L); */
    ((void)0);  /* lua_assert(!L->allowhook); */
    L->allowhook = 1;
    /*            restorestack(L, ci_top); */
    L->ci->top = (TValue *)( (char *)L->stack + ci_top );
    /*         restorestack(L, top); */
    L->top = (TValue *)( (char *)L->stack + top );
  }
}


static StkId adjust_varargs (lua_State *L, Proto *p, int actual) {
  int i;
  int nfixargs = p->numparams;
  Table *htab = NULL;
  StkId base, fixed;
  for (; actual < nfixargs; ++actual) {
    /* ((L->top++)->tt = LUA_TNIL); */
    setnilvalue(L->top++);
   }
#if defined(LUA_COMPAT_VARARG)
  /* VARARG_NEEDSARG 4 */
  if ( p->is_vararg & VARARG_NEEDSARG ) { /* compat. with old-style vararg? */
    int nvar = actual - nfixargs; /* number of extra arguments */
    ((void)0); /* lua_assert(p->is_vararg & VARARG_HASARG); */

    /* luaC_checkGC(L); */
    { 
	  ((void)0); 
      if ( G(L)->totalbytes >= G(L)->GCthreshold ) {
          luaC_step(L); 
	  }
	};

    /* luaD_checkstack(L, p->maxstacksize); */
    if ( (char *)L->stack_last - (char *)L->top <= (p->maxstacksize) * (int)sizeof(TValue) )  {
		luaD_growstack(L, p->maxstacksize); 
    } else { 
		((void)0);
	}

    htab = luaH_new(L, nvar, 1); /* create `arg' table */
    for (i=0; i<nvar; i++) { /* put extra arguments into `arg' table */
      /* { const TValue *o2=(L->top - nvar + i); TValue *o1=(luaH_setnum(L, htab, i+1)); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
      setobj2n(L, luaH_setnum(L, htab, i+1), L->top - nvar + i);
    }
    /* store counter in field `n' */
    /* { TValue *i_o=(luaH_setstr(L, htab, (luaS_newlstr(L, "" "n", (sizeof("n")/sizeof(char))-1)))); i_o->value.n=(((lua_Number)((nvar)))); i_o->tt= LUA_TNUMBER; }; */
    setnvalue(luaH_setstr(L, htab, luaS_newliteral(L, "n")), cast_num(nvar));
  }
#endif
  /* move fixed parameters to final position */
  fixed = L->top - actual; /* first fixed argument */
  base = L->top; /* final position of first argument */
  for (i=0; i<nfixargs; i++) {
    /* { const TValue *o2=(fixed+i); TValue *o1=(L->top++); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
    setobjs2s(L, L->top++, fixed+i);
    /* ((fixed+i)->tt = LUA_TNIL); */
    setnilvalue(fixed+i);
  }
  /* add `arg' parameter */
  if (htab) {
    /* { TValue *i_o=(L->top++); i_o->value.gc=((GCObject *)((htab))); i_o->tt= LUA_TTABLE; ((void)0); }; */
    sethvalue(L, L->top++, htab);
    ((void)0); /* lua_assert(iswhite(obj2gco(htab))); */
  }
  return base;
}


static StkId tryfuncTM (lua_State *L, StkId func) {
  const TValue *tm = luaT_gettmbyobj(L, func, TM_CALL);
  StkId p;
  /*                 savestack(L, func);              */
  ptrdiff_t funcr = ((char *)(func) - (char *)L->stack);
  /* ttisfunction(tm) */
  if ( !ttisfunction(tm) ) {
    luaG_typeerror(L, func, "call");
  }
  /* Open a hole inside the stack at `func' */
  for (p = L->top; p > func; p--) {
    /* { const TValue *o2=(p-1); TValue *o1=(p); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
    setobjs2s(L, p, p-1);
  }

  /* incr_top(L); */
  {
	  if ((char *)L->stack_last - (char *)L->top <= 1*(int)sizeof(TValue) ) { 
		  luaD_growstack(L, 1); 
	  } else { 
		  ((void)0);
	  } 
	  L->top++;
  };

  /*      restorestack(L, funcr); */
  func = ((TValue *)((char *)L->stack + (funcr))); /* previous call may change stack */

  /* { const TValue *o2=(tm); TValue *o1=(func); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
  setobj2s(L, func, tm); /* tag method is the new function to be called */

  return func;
}


#define inc_ci(L) \
  ((L->ci == L->end_ci) ? growCI(L) : \
   (condhardstacktests(luaD_reallocCI(L, L->size_ci)), ++L->ci))


int luaD_precall (lua_State *L, StkId func, int nresults) {
  LClosure *cl;
  ptrdiff_t funcr;
  /*    (((func)->tt) == LUA_TFUNCTION)  */
  if ( !ttisfunction(func) ) { /* `func' is not a function? */
    func = tryfuncTM(L, func); /* check the `function' tag method */
  }
  /*        savestack(L, func);             */
  funcr = ((char *)(func) - (char *)L->stack);
  /*   &clvalue(func)->l; */
  cl = &(&(func)->value.gc->cl)->l;

  L->ci->savedpc = L->savedpc;
  if (!cl->isC) { /* Lua function? prepare its call */
    CallInfo *ci;
    StkId st, base;
    Proto *p = cl->p;
    /* luaD_checkstack(L, p->maxstacksize); */
    if ( (char *)L->stack_last - (char *)L->top <= p->maxstacksize * (int)sizeof(TValue) ) { 
		luaD_growstack(L, p->maxstacksize); 
	} else { 
		((void)0);
	}

    /*      restorestack(L, funcr); */
    func = ((TValue *)((char *)L->stack + (funcr)));

    if (!p->is_vararg) { /* no varargs? */
      base = func + 1;
      if (L->top > base + p->numparams) {
        L->top = base + p->numparams;
      }
    }
    else { /* vararg function */
      /*           cast_int(L->top - func)   */
      int nargs = ((int)((L->top - func))) - 1;
      base = adjust_varargs(L, p, nargs);
      /*        restorestack(L, funcr);      */
      func = ((TValue *)((char *)L->stack + (funcr))); /* previous call may change the stack */
    }

    /*   inc_ci(L); */
    ci =  (L->ci == L->end_ci) 
		   ? growCI(L) 
		   : ( ((void)0), ++L->ci); /* now `enter' new function */

    ci->func = func;
    L->base = ci->base = base;
    ci->top = L->base + p->maxstacksize;
    ((void)0); /* lua_assert(ci->top <= L->stack_last); */
    L->savedpc = p->code; /* starting point */
    ci->tailcalls = 0;
    ci->nresults = nresults;
    for (st = L->top; st < ci->top; st++) {
      /* ((st)->tt = LUA_TNIL); */
      setnilvalue(st);
    }
    L->top = ci->top;
    /*                LUA_MASKCALL (1 << 0) */
    if ( L->hookmask & LUA_MASKCALL ) {
      L->savedpc++; /* hooks assume 'pc' is already incremented */
      /*          LUA_HOOKCALL 0 */
      luaD_callhook(L, LUA_HOOKCALL, -1);
      L->savedpc--; /* correct 'pc' */
    }
    return PCRLUA; /* PCRLUA 0 */
  }
  else { /* if is a C function, call it */
    CallInfo *ci;
    int n;
    /* luaD_checkstack(L, LUA_MINSTACK); */
    /* ensure minimum stack size */
    if ((char *)L->stack_last - (char *)L->top <= LUA_MINSTACK*(int)sizeof(TValue)) { 
		luaD_growstack(L, LUA_MINSTACK); 
	} else { 
		((void)0); 
	}
    /* ci = inc_ci(L); */
    /* now `enter' new function */
    ci = (L->ci == L->end_ci) 
		? growCI(L) 
		: ( ((void)0), ++L->ci );

    /*          restorestack(L, funcr); */
    ci->func = ((TValue *)((char *)L->stack + (funcr)));

    L->base = ci->base = ci->func + 1;
    ci->top = L->top + LUA_MINSTACK; /* LUA_MINSTACK 20 */
    ((void)0); /* lua_assert(ci->top <= L->stack_last); */
    ci->nresults = nresults;
    /*                LUA_MASKCALL  (1 << 0) */
    if (L->hookmask & LUA_MASKCALL) {
      /*            LUA_HOOKCALL 0           */
      luaD_callhook(L, 0, -1);
    }
    ((void) 0); /* lua_unlock(L); */
    /*                      (*curr_func(L)->c.f)(L); */
    n = (*((&(L->ci->func)->value.gc->cl))->c.f)(L); /* do the actual call */
    ((void) 0); /* lua_lock(L); */
    if (n < 0) { /* yielding? */
      return PCRYIELD; /* PCRYIELD 2 */
    } else {
      luaD_poscall(L, L->top - n);
      return 1;
    }
  }
}


static StkId callrethooks (lua_State *L, StkId firstResult) {
  /*              savestack(L, firstResult); */
  ptrdiff_t fr = ((char *)(firstResult) - (char *)L->stack); /* next call may change stack */
  /*               LUA_HOOKRET 1 */
  luaD_callhook(L, LUA_HOOKRET, -1);

  /*  
   
     f_isLua(L->ci) 
     !((&((L->ci)->func)->value.gc->cl))->c.isC
   */
  if ( !((&((L->ci)->func)->value.gc->cl))->c.isC ) { /* Lua function? */
    /*                    LUA_MASKRET (1 << 1)*/
    while ((L->hookmask & LUA_MASKRET) && L->ci->tailcalls-- ) { /* tail calls */
      /*               LUA_HOOKTAILRET 4 */
      luaD_callhook(L, LUA_HOOKTAILRET, -1);
    }
  }

  /*      restorestack(L, fr); */
  return ((TValue *)((char *)L->stack + (fr)));
}


int luaD_poscall (lua_State *L, StkId firstResult) {
  StkId res;
  int wanted, i;
  CallInfo *ci;
  /*                 LUA_MASKRET (1 << 1)*/
  if ( L->hookmask & LUA_MASKRET ) {
    firstResult = callrethooks(L, firstResult);
  }
  ci = L->ci--;
  res = ci->func; /* res == final position of 1st result */
  wanted = ci->nresults;
  L->base = (ci - 1)->base; /* restore base */
  L->savedpc = (ci - 1)->savedpc; /* restore savedpc */
  /* move results to correct place */
  for (i = wanted; i != 0 && firstResult < L->top; i--) {
    /* { const TValue *o2=(firstResult++); TValue *o1=(res++); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
    setobjs2s(L, res++, firstResult++);
  }
  while (i-- > 0) {
    /* ((res++)->tt = LUA_TNIL); */
    setnilvalue(res++);
  }
  L->top = res;

  /*             LUA_MULTRET (-1) */
  return (wanted - LUA_MULTRET); /* 0 iff wanted == LUA_MULTRET */
}


/*
** Call a function (C or Lua). The function to be called is at *func.
** The arguments are on the stack, right after the function.
** When returns, all the results are on the stack, starting at the original
** function position.
*/
void luaD_call (lua_State *L, StkId func, int nResults) {
  /*                 LUAI_MAXCCALLS 200 */
  if (++L->nCcalls >= LUAI_MAXCCALLS ) {
    if (L->nCcalls == LUAI_MAXCCALLS ) {
      luaG_runerror(L, "C stack overflow");
    }
    else if (L->nCcalls >= (LUAI_MAXCCALLS + (LUAI_MAXCCALLS>>3))) {
      /* LUA_ERRERR 5 */
      luaD_throw(L, LUA_ERRERR); /* error while handing stack error */
    }
  }
  /*                                     PCRLUA  0  */
  if (luaD_precall(L, func, nResults) == PCRLUA) { /* is a Lua function? */
    luaV_execute(L, 1); /* call it */
  }
  L->nCcalls--;

  /* luaC_checkGC(L); */
  { 
	  ((void)0); 
	  if ( G(L)->totalbytes >= G(L)->GCthreshold ) {
		  luaC_step(L); 
	  }
  };
}


static void resume (lua_State *L, void *ud) {
  /*               ((StkId)(ud)) */
  StkId firstArg = cast(StkId, ud);
  CallInfo *ci = L->ci;
  if (L->status == 0) { /* start coroutine? */
    ((void)0); /* lua_assert(ci == L->base_ci && firstArg > L->base); */
    /*                                LUA_MULTRET (-1)    PCRLUA 0 */
    if ( luaD_precall(L, firstArg - 1, LUA_MULTRET )   != PCRLUA ) {
      return;
    }
  }
  else { /* resuming from previous yield */
    ((void)0); /* lua_assert(L->status == LUA_YIELD); */
    L->status = 0;
    /*   f_isLua(ci)                                  
         (!((&((ci)->func)->value.gc->cl))->c.isC)
	*/
    if ( !f_isLua(ci) ) { /* `common' yield? */
      /* finish interrupted execution of `OP_CALL' */

	  /*
      lua_assert(GET_OPCODE(*((ci-1)->savedpc - 1)) == OP_CALL ||
                 GET_OPCODE(*((ci-1)->savedpc - 1)) == OP_TAILCALL);
	  */
      ((void)0);

      if (luaD_poscall(L, firstArg)) { /* complete it... */
        L->top = L->ci->top; /* and correct top if not multiple results */
      }
    }
    else { /* yielded inside a hook: just continue its execution */
      L->base = L->ci->base;
    }
  }
  /* cast_int(L->ci - L->base_ci) */
  luaV_execute(L, cast_int(L->ci - L->base_ci) );
}


static int resume_error (lua_State *L, const char *msg) {
  L->top = L->ci->base;
  /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)(((luaS_newlstr(L, msg, strlen(msg)))))); i_o->tt = LUA_TSTRING; ((void)0); }; */
  setsvalue2s(L, L->top, luaS_new(L, msg));
  /* incr_top(L); */
  {
	  if ((char *)L->stack_last - (char *)L->top <= 1*(int)sizeof(TValue)) { 
		  luaD_growstack(L, 1);
      } else {
		  ((void)0);
	  }
	  L->top++;
  };
  ((void) 0); /* lua_unlock(L); */

  /* LUA_ERRRUN 2 */
  return LUA_ERRRUN;
}


extern int lua_resume (lua_State *L, int nargs) {
  int status;
  ((void) 0); /* lua_lock(L); */
  /*                LUA_YIELD 1   */
  if (L->status !=  LUA_YIELD && (L->status != 0 || L->ci != L->base_ci)) {
      return resume_error(L, "cannot resume non-suspended coroutine");
  }
  if (L->nCcalls >= LUAI_MAXCCALLS ) { /* LUAI_MAXCCALLS 200 */
    return resume_error(L, "C stack overflow");
  }
  ((void)L); /* luai_userstateresume(L, nargs); */
  ((void)0); /* lua_assert(L->errfunc == 0); */
  L->baseCcalls = ++L->nCcalls;
  status = luaD_rawrunprotected(L, resume, L->top - nargs);
  if (status != 0) { /* error? */
    /*          ((lu_byte)((status))); */
    L->status = cast_byte(status); /* mark thread as `dead' */
    luaD_seterrorobj(L, status, L->top);
    L->ci->top = L->top;
  }
  else {
    ((void)0); /* lua_assert(L->nCcalls == L->baseCcalls); */
    status = L->status;
  }

  --L->nCcalls;
  ((void) 0); /* lua_unlock(L); */

  return status;
}


extern int lua_yield (lua_State *L, int nresults) {
  ((void)L);  /* luai_userstateyield(L, nresults); */
  ((void) 0); /* lua_lock(L); */
  if (L->nCcalls > L->baseCcalls) {
    luaG_runerror(L, "attempt to yield across metamethod/C-call boundary");
  }
  L->base = L->top - nresults; /* protect stack slots below */
  L->status = 1;
  ((void) 0); /* lua_unlock(L); */
  return -1;
}


int luaD_pcall (lua_State *L, Pfunc func, void *u,
                ptrdiff_t old_top, ptrdiff_t ef) {
  int status;
  unsigned short oldnCcalls = L->nCcalls;
  /*                  saveci(L, L->ci); */
  ptrdiff_t old_ci = ((char *)(L->ci) - (char *)L->base_ci);
  lu_byte old_allowhooks = L->allowhook;
  ptrdiff_t old_errfunc = L->errfunc;
  L->errfunc = ef;
  status = luaD_rawrunprotected(L, func, u);
  if (status != 0) { /* an error occurred? */
    /*              restorestack(L, old_top);                */
    StkId oldtop = ((TValue *)((char *)L->stack + (old_top)));
    luaF_close(L, oldtop); /* close eventual pending closures */
    luaD_seterrorobj(L, status, oldtop);
    L->nCcalls = oldnCcalls;
    /*       restoreci(L, old_ci);                      */
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
  /*                  cast(struct SParser *, ud); */
  struct SParser *p = ((struct SParser *)(ud));
  int c = luaZ_lookahead(p->z);

  /* luaC_checkGC(L); */
  { 
	  ((void)0); 
	  if ( G(L)->totalbytes >= G(L)->GCthreshold) { 
		  luaC_step(L); 
	  }
  };

  /* LUA_SIGNATURE  "\033Lua" */
  tf = ( (c == LUA_SIGNATURE[0]) ? luaU_undump : luaY_parser)(L, p->z,
                                                             &p->buff, p->name);

  /*                                   hvalue(gt(L)              */
  cl = luaF_newLclosure(L, tf->nups, (&((&L->l_gt))->value.gc->h));
  cl->l.p = tf;
  for (i = 0; i < tf->nups; i++) { /* initialize eventual upvalues */
    cl->l.upvals[i] = luaF_newupval(L);
  }
  /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((cl))); i_o->tt = LUA_TFUNCTION; ((void)0); }; */
  setclvalue(L, L->top, cl);

  /* incr_top(L); */
  {
	  if ((char *)L->stack_last - (char *)L->top <= 1*(int)sizeof(TValue) ) { 
		  luaD_growstack(L, 1); 
	  } else { 
		  ((void)0); 
	  } 
      L->top++;
  };
}


int luaD_protectedparser (lua_State *L, ZIO *z, const char *name) {
  struct SParser p;
  int status;
  p.z = z; p.name = name;

  /*  luaZ_initbuffer(L, &p.buff); */
  ( 
    (&p.buff)->buffer = NULL, 
	(&p.buff)->buffsize = 0
  );
  /*                                    savestack(L, L->top) */
  status = luaD_pcall(L, f_parser, &p, ((char *)(L->top) - (char *)L->stack), L->errfunc);

  /* luaZ_freebuffer(L, &p.buff); */
  ( 
    ( 
      (&p.buff)->buffer =
      ( (char *)( 
     			  ( ( (size_t)(1) ) <= MAX_SIZET/sizeof(char) ) 
     			  ? luaM_realloc_(L, (&p.buff)->buffer, ( (&p.buff)->buffsize) * sizeof(char), 0*sizeof(char) ) 
     			  : luaM_toobig(L)  
     			) 
	  ) 
    ), 
    (&p.buff)->buffsize = 0
  ); 
  return status;
}
