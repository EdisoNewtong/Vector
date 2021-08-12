/*
** $Id: ldebug.c,v 2.29.1.6 2008/05/08 16:56:26 roberto Exp $
** Debug Interface
** See Copyright Notice in lua.h
*/


#include <stdarg.h>
#include <stddef.h>
#include <string.h>


#define ldebug_c
#define LUA_CORE

#include "lua.h"

#include "lapi.h"
#include "lcode.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"



static const char *getfuncname (lua_State *L, CallInfo *ci, const char **name);


static int currentpc (lua_State *L, CallInfo *ci) {
  /*   (((((ci)->func)->tt) == LUA_TFUNCTION) && (!((&((ci)->func)->value.gc->cl))->c.isC))   */
  if (!isLua(ci)) {
    return -1; /* function is not a Lua function? */
  }
  if (ci == L->ci) {
    ci->savedpc = L->savedpc;
  }

  /*        pcRel(ci->savedpc,                        ci_func(ci)->l.p        );  */
  return (((int)((ci->savedpc) - (((&((ci)->func)->value.gc->cl))->l.p)->code)) - 1);
}


static int currentline (lua_State *L, CallInfo *ci) {
  int pc = currentpc(L, ci);
  if (pc < 0) {
    return -1; /* only active lua functions have current-line information */
  } else {
    /*      getline(ci_func(ci)->l.p, pc); */
    return (((((&((ci)->func)->value.gc->cl))->l.p)->lineinfo) ? (((&((ci)->func)->value.gc->cl))->l.p)->lineinfo[pc] : 0);
  }
}


/*
** this function can be called asynchronous (e.g. during a signal)
*/
extern int lua_sethook (lua_State *L, lua_Hook func, int mask, int count) {
  if (func == NULL || mask == 0) { /* turn off hooks? */
    mask = 0;
    func = NULL;
  }
  L->hook = func;
  L->basehookcount = count;
  /* resethookcount(L); */
  (L->hookcount = L->basehookcount);
  /*           ((lu_byte)((mask))) */
  L->hookmask = cast_byte(mask);
  return 1;
}


extern lua_Hook lua_gethook (lua_State *L) {
  return L->hook;
}


extern int lua_gethookmask (lua_State *L) {
  return L->hookmask;
}


extern int lua_gethookcount (lua_State *L) {
  return L->basehookcount;
}


extern int lua_getstack (lua_State *L, int level, lua_Debug *ar) {
  int status;
  CallInfo *ci;
  ((void) 0); /* lua_lock(L); */
  for (ci = L->ci; level > 0 && ci > L->base_ci; ci--) {
    level--;
    /*  (!((&((ci)->func)->value.gc->cl))->c.isC)  */
    if ( f_isLua(ci) ) { /* Lua function? */
      level -= ci->tailcalls; /* skip lost tail calls */
    }
  }
  if (level == 0 && ci > L->base_ci) { /* level found? */
    status = 1;
    /*         ((int)((ci - L->base_ci)))  */
    ar->i_ci = cast_int(ci - L->base_ci);
  }
  else if (level < 0) { /* level is of a lost tail call? */
    status = 1;
    ar->i_ci = 0;
  }
  else {
    status = 0; /* no such level */
  }
  ((void) 0); /* lua_unlock(L); */
  return status;
}


static Proto *getluaproto (CallInfo *ci) {
  /*      (isLua(ci) ? ci_func(ci)->l.p : NULL); */
  return (
               /*  isLua(ci)   */
               (((((ci)->func)->tt) == LUA_TFUNCTION) && (!((&((ci)->func)->value.gc->cl))->c.isC)) 
               /*                 ci_func(ci)->l.p */
             ? ((&((ci)->func)->value.gc->cl))->l.p 
             : NULL
         );
}


static const char *findlocal (lua_State *L, CallInfo *ci, int n) {
  const char *name;
  Proto *fp = getluaproto(ci);
  if (fp && (name = luaF_getlocalname(fp, n, currentpc(L, ci))) != NULL) {
    return name; /* is a local variable in a Lua function */
  } else {
    StkId limit = (ci == L->ci) ? L->top : (ci+1)->func;
    if (limit - ci->base >= n && n > 0) { /* is 'n' inside 'ci' stack? */
      return "(*temporary)";
    } else {
      return NULL;
    }
  }
}


extern const char *lua_getlocal (lua_State *L, const lua_Debug *ar, int n) {
  CallInfo *ci = L->base_ci + ar->i_ci;
  const char *name = findlocal(L, ci, n);
  ((void) 0); /* lua_lock(L); */
  if (name) {
      luaA_pushobject(L, ci->base + (n - 1));
  }
  ((void) 0); /* lua_unlock(L); */
  return name;
}


extern const char *lua_setlocal (lua_State *L, const lua_Debug *ar, int n) {
  CallInfo *ci = L->base_ci + ar->i_ci;
  const char *name = findlocal(L, ci, n);
  ((void) 0); /* lua_lock(L); */
  if (name) {
    /* { const TValue *o2=(L->top - 1); TValue *o1=(ci->base + (n - 1)); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
    setobjs2s(L, ci->base + (n - 1), L->top - 1);
  }
  L->top--; /* pop value */
  ((void) 0); /* lua_unlock(L); */
  return name;
}


static void funcinfo (lua_Debug *ar, Closure *cl) {
  if (cl->c.isC) {
    ar->source = "=[C]";
    ar->linedefined = -1;
    ar->lastlinedefined = -1;
    ar->what = "C";
  }
  else {
    /*          ((const char *)((cl->l.p->source) + 1))   */
    ar->source = getstr(cl->l.p->source);
    ar->linedefined = cl->l.p->linedefined;
    ar->lastlinedefined = cl->l.p->lastlinedefined;
    ar->what = (ar->linedefined == 0) ? "main" : "Lua";
  }
  /*                                      LUA_IDSIZE 60 */
  luaO_chunkid(ar->short_src, ar->source, LUA_IDSIZE);
}


static void info_tailcall (lua_Debug *ar) {
  ar->name = ar->namewhat = "";
  ar->what = "tail";
  ar->lastlinedefined = ar->linedefined = ar->currentline = -1;
  ar->source = "=(tail call)";
  /*                                     LUA_IDSIZE 60 */
  luaO_chunkid(ar->short_src, ar->source, LUA_IDSIZE);
  ar->nups = 0;
}


static void collectvalidlines (lua_State *L, Closure *f) {
  if (f == NULL || f->c.isC) {
    /* ((L->top)->tt= LUA_TNIL); */
    setnilvalue(L->top);
  }
  else {
    Table *t = luaH_new(L, 0, 0);
    int *lineinfo = f->l.p->lineinfo;
    int i;
    for (i=0; i<f->l.p->sizelineinfo; i++) {
      /* { TValue *i_o=(luaH_setnum(L, t, lineinfo[i])); i_o->value.b=(1); i_o->tt=1; }; */
      setbvalue(luaH_setnum(L, t, lineinfo[i]), 1);
    }
    /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((t))); i_o->tt=5; ((void)0); }; */
    sethvalue(L, L->top, t); 
  }

  /* incr_top(L); */
  {
      if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) { 
        luaD_growstack(L, 1);
      } else { 
         ((void)0);; 
      }
      L->top++;
  };

}


static int auxgetinfo (lua_State *L, const char *what, lua_Debug *ar,
                    Closure *f, CallInfo *ci) {
  int status = 1;
  if (f == NULL) {
    info_tailcall(ar);
    return status;
  }
  for (; *what; what++) {
    switch (*what) {
      case 'S': {
        funcinfo(ar, f);
        break;
      }
      case 'l': {
        ar->currentline = (ci) ? currentline(L, ci) : -1;
        break;
      }
      case 'u': {
        ar->nups = f->c.nupvalues;
        break;
      }
      case 'n': {
        ar->namewhat = (ci) ? getfuncname(L, ci, &ar->name) : NULL;
        if (ar->namewhat == NULL) {
          ar->namewhat = ""; /* not found */
          ar->name = NULL;
        }
        break;
      }
      case 'L':
      case 'f': { /* handled by lua_getinfo */
        break;
      }
      default: {
        status = 0; /* invalid option */
      }
    }
  }
  return status;
}


extern int lua_getinfo (lua_State *L, const char *what, lua_Debug *ar) {
  int status;
  Closure *f = NULL;
  CallInfo *ci = NULL;
  ((void) 0); /* lua_lock(L); */
  if (*what == '>') {
    StkId func = L->top - 1;
    { (void)L; }; /* luai_apicheck(L, ttisfunction(func)); */
    what++; /* skip the '>' */
    /*  (&(func)->value.gc->cl); */
    f = clvalue(func);
    L->top--; /* pop function */
  }
  else if (ar->i_ci != 0) { /* no tail call? */
    ci = L->base_ci + ar->i_ci;
    ((void)0); /* lua_assert(ttisfunction(ci->func)); */
    /*  (&(ci->func)->value.gc->cl); */
    f = clvalue(ci->func);
  }
  status = auxgetinfo(L, what, ar, f, ci);
  if (strchr(what, 'f')) {
    if (f == NULL) {
      /* ((L->top)->tt=LUA_TNIL); */
      setnilvalue(L->top);
    } else {
      /* { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((f))); i_o->tt=LUA_TFUNCTION; ((void)0); }; */
      setclvalue(L, L->top, f);
    }

    /* incr_top(L); */
    {
        if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) {
            luaD_growstack(L, 1); 
        } else { 
            ((void)0);; 
        }
        L->top++;
    };
  }
  if (strchr(what, 'L')) {
    collectvalidlines(L, f);
  }
  ((void) 0); /* lua_unlock(L); */
  return status;
}


/*
** {======================================================
** Symbolic Execution and code checker
** =======================================================
*/

#define check(x)		if (!(x)) return 0;

#define checkjump(pt,pc)	check(0 <= pc && pc < pt->sizecode)

#define checkreg(pt,reg)	check((reg) < (pt)->maxstacksize)



static int precheck (const Proto *pt) {
  /* 
    check(pt->maxstacksize <= MAXSTACK);
                         MAXSTACK 250
  */
  if (!(pt->maxstacksize <= MAXSTACK)) { 
      return 0;
  }

  /*
    VARARG_HASARG   1
    check(pt->numparams+(pt->is_vararg & VARARG_HASARG) <= pt->maxstacksize);
  */
  if (!(pt->numparams+(pt->is_vararg & VARARG_HASARG) <= pt->maxstacksize)) { 
      return 0;
  }
  /*

  VARARG_NEEDSARG  4
  VARARG_HASARG    1

  check(!(pt->is_vararg & VARARG_NEEDSARG) ||
              (pt->is_vararg & VARARG_HASARG));
  */
  if (!(!(pt->is_vararg & VARARG_NEEDSARG) || (pt->is_vararg & VARARG_HASARG))) { 
      return 0;
  }

  /* check(pt->sizeupvalues <= pt->nups); */
  if (!(pt->sizeupvalues <= pt->nups)) {
      return 0;
  }

  /* check(pt->sizelineinfo == pt->sizecode || pt->sizelineinfo == 0); */
  if (!(pt->sizelineinfo == pt->sizecode || pt->sizelineinfo == 0)) { 
      return 0;
  }


  /* check(pt->sizecode > 0 && GET_OPCODE(pt->code[pt->sizecode-1]) == OP_RETURN); */
  if (!(pt->sizecode > 0 && (((OpCode)(((pt->code[pt->sizecode-1])>>0) & ((~((~(Instruction)0)<<6))<<0)))) == OP_RETURN)) { 
      return 0;;
  }

  return 1;
}


#define checkopenop(pt,pc)	luaG_checkopenop((pt)->code[(pc)+1])

int luaG_checkopenop (Instruction i) {
  /* switch ( GET_OPCODE(i) ) */
  switch ((((OpCode)(((i)>>0) & ((~((~(Instruction)0)<<6))<<0))))) {
    case OP_CALL:
    case OP_TAILCALL:
    case OP_RETURN:
    case OP_SETLIST: {
      /*   check(GETARG_B(i) == 0); */
      if (!(( (int)( (i>>23) & ( (~((~(Instruction)0)<<9)) << 0 ) ) ) == 0) ) { 
          return 0;
      }
      return 1;
    }
    default: { 
      return 0; /* invalid instruction after an open call */
    }
  }
}


static int checkArgMode (const Proto *pt, int r, enum OpArgMask mode) {
  switch (mode) {
    case OpArgN: {
      /* check(r == 0); */
      if (!(r == 0)) { 
          return 0;;
      }
      break;
    }
    case OpArgU: {
      break;
    }
    case OpArgR: {
      /* checkreg(pt, r); */
      if (!((r) < (pt)->maxstacksize)) { 
          return 0;
      }
      break;
    }
    case OpArgK: {
      /* check(ISK(r) ? INDEXK(r) < pt->sizek : r < pt->maxstacksize); */
      if (!(
                ((r) & (1 << 8))                   /* ISK(r) */
              ? ((int)(r) & ~(1 << 8)) < pt->sizek /* INDEXK(r) */
              : r < pt->maxstacksize)
       )
      {
          return 0;
      }
      break;
    }
  }
  return 1;
}


static Instruction symbexec (const Proto *pt, int lastpc, int reg) {
  int pc;
  int last; /* stores position of last instruction that changed `reg' */
  last = pt->sizecode-1; /* points to final return (a `neutral' instruction) */

  /* check(precheck(pt)); */
  if (!(precheck(pt))) { 
      return 0;
  }

  for (pc = 0; pc < lastpc; pc++) {
    Instruction i = pt->code[pc];
    /*            GET_OPCODE(i); */
    OpCode op = (OpCode)( (i>>0) & ((~((~(Instruction)0)<<6))<<0));
    /*        GETARG_A(i);                                          */
    int a = (int)( (i>>6) & ((~((~(Instruction)0)<<8))<<0) );
    int b = 0;
    int c = 0;
    /* check(op < NUM_OPCODES); */
    if ( !(op < ( (int)(OP_VARARG) + 1) ) ) {
        return 0;
    }

    /* checkreg(pt, a); */
    if (! ( a < (pt)->maxstacksize) ) { 
        return 0;
    }
    /*        getOpMode(op)                   */
    switch ( (enum OpMode)(luaP_opmodes[op] & 3) ) {
      case iABC: {
        /*     GETARG_B(i);  */
        b =  (int)( (i>>23) & ((~((~(Instruction)0)<<9))<<0) );
        /*    GETARG_C(i); */
        c = (int)( (i>>14) & ((~((~(Instruction)0)<<9))<<0) );
        /* check(checkArgMode(pt, b, getBMode(op))); */
        if ( !(checkArgMode(pt, b, ((enum OpArgMask)((luaP_opmodes[op] >> 4) & 3) ) )) ) {
            return 0;
        }

        /* check(checkArgMode(pt, c, getCMode(op))); */
        if ( !(checkArgMode(pt, c, ((enum OpArgMask)((luaP_opmodes[op] >> 2) & 3) ) )) ) {
            return 0;
        }
        break;
      }
      case iABx: {
        /*   GETARG_Bx(i); */
        b = (int)( (i>>14) & ((~((~(Instruction)0)<<18))<<0) );
        /*     getBMode(op) */
        if ( ( (enum OpArgMask)((luaP_opmodes[op] >> 4) & 3) ) == OpArgK ) {
          /* check(b < pt->sizek); */
          if (!(b < pt->sizek)) {
              return 0;
          }
        }
        break;
      }
      case iAsBx: {
        /*   GETARG_sBx(i); */
        b = (((int)( (i>>14) & ((~((~(Instruction)0)<<18))<<0)))) - (((1<<18)-1)>>1);
        /*    getBMode(op) */
        if ( ( (enum OpArgMask)( (luaP_opmodes[op] >> 4) & 3) ) == OpArgR) {
          int dest = pc+1+b;
          /* check(0 <= dest && dest < pt->sizecode); */
          if (!(0 <= dest && dest < pt->sizecode)) {
              return 0;
          }
          if (dest > 0) {
            int j;
            /* check that it does not jump to a setlist count; this
               is tricky, because the count from a previous setlist may
               have the same value of an invalid setlist; so, we must
               go all the way back to the first of them (if any) */
            for (j = 0; j < dest; j++) {
              Instruction d = pt->code[dest-1-j];
              /* if (!(GET_OPCODE(d) == OP_SETLIST && GETARG_C(d) == 0)) */
              if ( !( ( (OpCode)( (d>>0) & ( (~((~(Instruction)0)<<6))<<0 ) ) ) == OP_SETLIST && ( (int)( (d>>14) & ((~((~(Instruction)0)<<9))<<0) ) ) == 0) ) {
                break;
              }
            }
            /* if 'j' is even, previous value is not a setlist (even if
               it looks like one) */
            /* check((j&1) == 0); */
            if ( !( (j & 1) == 0) ) {
                return 0;
            }
          }
        }
        break;
      }
    }

    /*   testAMode(op) */
    if ( (luaP_opmodes[op] & (1 << 6)) ) {
      if (a == reg) {
        last = pc; /* change register `a' */
      }
    }
    /*    testTMode(op) */
    if ( (luaP_opmodes[op] & (1 << 7)) ) {
      /* check(pc+2 < pt->sizecode); */
      if (!(pc+2 < pt->sizecode)) {
          return 0; /* check skip */
      }

      /* check(GET_OPCODE(pt->code[pc+1]) == OP_JMP); */
      if (!( ( (OpCode)( ( (pt->code[pc+1]) >> 0) & ((~((~(Instruction)0)<<6))<<0) ) ) == OP_JMP ) ) {
          return 0;
      }
    }
    switch (op) {
      case OP_LOADBOOL: {
        if (c == 1) { /* does it jump? */
          /* check(pc+2 < pt->sizecode); */
          if (!(pc+2 < pt->sizecode)) {
              return 0; /* check its jump */
          }
          /*
          check(GET_OPCODE(pt->code[pc+1]) != OP_SETLIST ||
                GETARG_C(pt->code[pc+1]) != 0);

          if (!((((OpCode)(((pt->code[pc+1])>>0) & ((~((~(Instruction)0)<<6))<<0)))) != OP_SETLIST || (((int)(((pt->code[pc+1])>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)))) != 0)) return 0;;
          */
          if ( !( ( (OpCode)( ((pt->code[pc+1])>>0) & ((~((~(Instruction)0)<<6))<<0) ) ) != OP_SETLIST 
             || ( (int)( ((pt->code[pc+1])>>14) & ((~((~(Instruction)0)<<9))<<0) )  ) != 0 ) ) 
          {
              return 0;
          }
        }
        break;
      }
      case OP_LOADNIL: {
        if (a <= reg && reg <= b) {
          last = pc; /* set registers from `a' to `b' */
        }
        break;
      }
      case OP_GETUPVAL:
      case OP_SETUPVAL: {
        /* check(b < pt->nups); */
        if (!(b < pt->nups)) {
            return 0;
        }
        break;
      }
      case OP_GETGLOBAL:
      case OP_SETGLOBAL: {
        /* check(ttisstring(&pt->k[b])); */
        if (!( (&pt->k[b])->tt == LUA_TSTRING ) ) {
            return 0;
        }
        break;
      }
      case OP_SELF: {
        /* checkreg(pt, a+1); */
        if ( !( (a+1) < (pt)->maxstacksize ) ) {
            return 0;
        }
        if (reg == a+1) {
          last = pc;
        }
        break;
      }
      case OP_CONCAT: {
        /* check(b < c); */
        if (!(b < c)) {
            return 0; /* at least two operands */
        }
        break;
      }
      case OP_TFORLOOP: {
        /* check(c >= 1); */
        if (!(c >= 1)) {
            return 0; /* at least one result (control variable) */
        }
        /* checkreg(pt, a+2+c); */
        if ( !( (a+2+c) < (pt)->maxstacksize) ) {
            return 0; /* space for results */
        }
        if (reg >= a+2) {
          last = pc; /* affect all regs above its base */
        }
        break;
      }
      case OP_FORLOOP:
      case OP_FORPREP: {
        /* checkreg(pt, a+3); */
        if ( !( (a+3) < (pt)->maxstacksize ) ) {
            return 0;
        }
        /* go through */
      }
      case OP_JMP: {
        int dest = pc+1+b;
        /* not full check and jump is forward and do not skip `lastpc'? */
        /*  NO_REG ((1<<8)-1) */
        if (reg != NO_REG && pc < dest && dest <= lastpc) {
          pc += b; /* do the jump */
        }
        break;
      }
      case OP_CALL:
      case OP_TAILCALL: {
        if (b != 0) {
          /* checkreg(pt, a+b-1); */
          if ( !( (a+b-1) < (pt)->maxstacksize) ) {
              return 0;
          }
        }
        c--; /* c = num. returns */
        /* LUA_MULTRET (-1) */
        if ( c == LUA_MULTRET ) {
          /* check(checkopenop(pt, pc)); */
          if (!(luaG_checkopenop( pt->code[(pc)+1] ))) { 
              return 0;
          }
        }
        else if (c != 0) {
          /* checkreg(pt, a+c-1); */
          if (!( (a+c-1) < pt->maxstacksize)) { 
              return 0;
          }
        }
        if (reg >= a) {
          last = pc; /* affect all registers above base */
        }
        break;
      }
      case OP_RETURN: {
        b--; /* b = num. returns */
        if (b > 0) {
          /* checkreg(pt, a+b-1); */
          if (!( (a+b-1) < pt->maxstacksize ) ) { 
              return 0;
          }
        }
        break;
      }
      case OP_SETLIST: {
        if (b > 0) {
          /* checkreg(pt, a + b); */
          if ( !( (a + b) < pt->maxstacksize ) ) {
              return 0;
          }
        }
        if (c == 0) {
          pc++;
          /* check(pc < pt->sizecode - 1); */
          if ( !(pc < pt->sizecode - 1) ) {
              return 0;
          }
        }
        break;
      }
      case OP_CLOSURE: {
        int nup, j;
        /* check(b < pt->sizep); */
        if (!(b < pt->sizep)) {
            return 0;
        }
        nup = pt->p[b]->nups;

        /* check(pc + nup < pt->sizecode); */
        if (!(pc + nup < pt->sizecode)) {
            return 0;
        }

        for (j = 1; j <= nup; j++) {
          /*            GET_OPCODE(pt->code[pc + j])      */
          OpCode op1 = (((OpCode)(((pt->code[pc + j])>>0) & ((~((~(Instruction)0)<<6))<<0))));

          /* check(op1 == OP_GETUPVAL || op1 == OP_MOVE); */
          if (!(op1 == OP_GETUPVAL || op1 == OP_MOVE)) {
              return 0;
          }
        }
        /* NO_REG ((1<<8)-1) */
        if (reg != NO_REG ) { /* tracing? */
          pc += nup; /* do not 'execute' these pseudo-instructions */
        }
        break;
      }
      case OP_VARARG: {
        /*
             VARARG_ISVARARG  2
             VARARG_NEEDSARG  4

            check((pt->is_vararg & VARARG_ISVARARG) &&
                 !(pt->is_vararg & VARARG_NEEDSARG));
        */
        if ( !( (pt->is_vararg & VARARG_ISVARARG) && !(pt->is_vararg & VARARG_NEEDSARG)) ) {
            return 0;
        }
        b--;
        /* LUA_MULTRET (-1)*/
        if ( b == LUA_MULTRET ) {
          /* check(checkopenop(pt, pc)); */
          if ( !(luaG_checkopenop((pt)->code[(pc)+1])) ) {
              return 0;
          }
        }
        /* checkreg(pt, a+b-1); */
        if (!( (a+b-1) < pt->maxstacksize) ) {
            return 0;
        }
        break;
      }
      default: {
        break;
      }
    }
  }
  return pt->code[last];
}

#undef check
#undef checkjump
#undef checkreg

/* }====================================================== */

int luaG_checkcode (const Proto *pt) {
  /*                                  NO_REG ((1<<8)-1) */
  return (symbexec(pt, pt->sizecode, NO_REG) != 0);
}


static const char *kname (Proto *p, int c) {
  /* (ISK(c)           &&     ttisstring(&p->k[INDEXK(c)]))   */
  /*                              [ INDEXK(c)            ]    */
  if (((c) & (1 << 8)) && (((&p->k[((int)(c) & ~(1 << 8))])->tt) == LUA_TSTRING)) {
    /* return ((const char *)(((&(&p->k[((int)(c) & ~(1 << 8))])->value.gc->ts)) + 1)); */
    return svalue(&p->k[INDEXK(c)]);
  } else {
    return "?";
  }
}


static const char *getobjname (lua_State *L, CallInfo *ci, int stackpos,
                               const char **name) {
  /* ( ( ( (ci->func)->tt) == LUA_TFUNCTION) && (!((&((ci)->func)->value.gc->cl))->c.isC ) ) */
  if ( isLua(ci) ) { /* a Lua function? */
    /*                             ci_func(ci)->l.p; */
    Proto *p = ((&((ci)->func)->value.gc->cl))->l.p;
    int pc = currentpc(L, ci);
    Instruction i;
    *name = luaF_getlocalname(p, stackpos+1, pc);
    if (*name) { /* is a local? */
      return "local";
    }
    i = symbexec(p, pc, stackpos); /* try symbolic execution */
    ((void)0); /* lua_assert(pc != -1); */
    /*      GET_OPCODE(i)                                           */
    switch ((((OpCode)(((i)>>0) & ((~((~(Instruction)0)<<6))<<0))))) {
      case OP_GETGLOBAL: {
        /*       GETARG_Bx(i); */
        int g = (((int)((i>>14) & ((~((~(Instruction)0)<<18))<<0)))); /* global index */
        ((void)0);  /* lua_assert(ttisstring(&p->k[g])); */
        /*    =  svalue(&p->k[g]); */
        *name = ((const char *)(((&(&p->k[g])->value.gc->ts)) + 1));
        return "global";
      }
      case OP_MOVE: {
        /*        GETARG_A(i); */
        int a = (int)( (i>>6)  & ((~((~(Instruction)0)<<8))<<0) );
        /*        GETARG_B(i); */
        int b = (int)( (i>>23) & ((~((~(Instruction)0)<<9))<<0) ); /* move from `b' to `a' */
        if (b < a) {
          return getobjname(L, ci, b, name); /* get name for `b' */
        }
        break;
      }
      case OP_GETTABLE: {
        /*        GETARG_C(i); */
        int k = (int)( (i>>14) & ((~((~(Instruction)0)<<9))<<0) ); /* key index */
        *name = kname(p, k);
        return "field";
      }
      case OP_GETUPVAL: {
        /*        GETARG_C(i); */
        int u = (int)( (i>>23) & ((~((~(Instruction)0)<<9))<<0) ); /* upvalue index */
        /*                     getstr(p->upvalues[u])                    */
        *name = p->upvalues ? ((const char *)((p->upvalues[u]) + 1)) : "?";
        return "upvalue";
      }
      case OP_SELF: {
        /*        GETARG_B(i); */
        int k = (int)( (i>>14) & ((~((~(Instruction)0)<<9))<<0) ); /* key index */
        *name = kname(p, k);
        return "method";
      }
      default: {
        break;
      }
    }
  }
  return NULL; /* no useful name found */
}


static const char *getfuncname (lua_State *L, CallInfo *ci, const char **name) {
  Instruction i;
  /*
     if ((isLua(ci) && ci->tailcalls > 0) || !isLua(ci - 1))

     isLua(ci)
     (((((ci)->func)->tt) == LUA_TFUNCTION) && (!((&((ci)->func)->value.gc->cl))->c.isC))

    (((((ci - 1)->func)->tt) == LUA_TFUNCTION) && (!((&((ci - 1)->func)->value.gc->cl))->c.isC))

  */
  if (  ( isLua(ci) 
         && ci->tailcalls > 0 ) 
        || !isLua(ci - 1)) 
  {
    return NULL; /* calling function is not Lua (or is unknown) */
  }
  ci--; /* calling function */
  /*                      ci_func(ci)->l.p->code[currentpc(L, ci)] */
  i = (&(ci->func)->value.gc->cl)->l.p->code[currentpc(L, ci)];
  /*
    GET_OPCODE(i) (((OpCode)(((i)>>0) & ((~((~(Instruction)0)<<6))<<0))))
  */
  if ( GET_OPCODE(i) == OP_CALL || GET_OPCODE(i) == OP_TAILCALL ||
       GET_OPCODE(i) == OP_TFORLOOP) {
    /* 
     GETARG_A(i) 
     ( (int)( (i>>6) & ((~((~(Instruction)0)<<8))<<0) ) )
    */
    return getobjname(L, ci, GETARG_A(i) , name);
  } else {
    return NULL; /* no useful name can be found */
  }
}


/* only ANSI way to check whether a pointer points to an array */
static int isinstack (CallInfo *ci, const TValue *o) {
  StkId p;
  for (p = ci->base; p < ci->top; p++) {
    if (o == p) {
      return 1;
    }
  }
  return 0;
}


void luaG_typeerror (lua_State *L, const TValue *o, const char *op) {
  const char *name = NULL;
  /*                              ttype(o) */
  const char *t = luaT_typenames[((o)->tt)];
  const char *kind = (isinstack(L->ci, o)) ?
                         /*                   ((int)((o - L->base))) */
                         getobjname(L, L->ci, cast_int(o - L->base), &name) :
                         NULL;
  if (kind) {
    luaG_runerror(L, "attempt to %s %s " "'" "%s" "'" " (a %s value)",
                op, kind, name, t);
  } else {
    luaG_runerror(L, "attempt to %s a %s value", op, t);
  }
}


void luaG_concaterror (lua_State *L, StkId p1, StkId p2) {
  /*  ttisstring(p1)    || ttisnumber(p1) */
  if ( ( p1->tt == LUA_TSTRING ) || ( p1->tt == LUA_TNUMBER ) ) {
    p1 = p2;
  }
  ((void)0); /* lua_assert(!ttisstring(p1) && !ttisnumber(p1)); */
  luaG_typeerror(L, p1, "concatenate");
}


void luaG_aritherror (lua_State *L, const TValue *p1, const TValue *p2) {
  TValue temp;
  if (luaV_tonumber(p1, &temp) == NULL) {
    p2 = p1; /* first operand is wrong */
  }
  luaG_typeerror(L, p2, "perform arithmetic on");
}


int luaG_ordererror (lua_State *L, const TValue *p1, const TValue *p2) {
  /*                              ttype(p1) ((p1)->tt) */
  const char *t1 = luaT_typenames[ttype(p1)];
  /*                              ttype(p2) ((p2)->tt) */
  const char *t2 = luaT_typenames[ttype(p2)];
  if (t1[2] == t2[2]) {
    luaG_runerror(L, "attempt to compare two %s values", t1);
  } else {
    luaG_runerror(L, "attempt to compare %s with %s", t1, t2);
  }
  return 0;
}


static void addinfo (lua_State *L, const char *msg) {
  CallInfo *ci = L->ci;
  /*
   isLua(ci)
   (((((ci)->func)->tt) == 6) && (!((&((ci)->func)->value.gc->cl))->c.isC))
  */
  if ( isLua(ci) ) { /* is Lua code? */
    /* LUA_IDSIZE 60 */
    char buff[LUA_IDSIZE]; /* add file:line information */
    int line = currentline(L, ci);
    /*                  ((const char *)((getluaproto(ci)->source) + 1))                */
    /*                                                                  LUA_IDSIZE  60 */
    luaO_chunkid(buff, getstr(getluaproto(ci)->source), LUA_IDSIZE);
    luaO_pushfstring(L, "%s:%d: %s", buff, line, msg);
  }
}


void luaG_errormsg (lua_State *L) {
  if (L->errfunc != 0) { /* is there an error handling function? */
    /*               restorestack(L, L->errfunc);                */
    StkId errfunc = ((TValue *)((char *)L->stack + (L->errfunc)));
    /*   (((errfunc)->tt) == LUA_TFUNCTION ) */
    if (!ttisfunction(errfunc)) {
      /*            LUA_ERRERR 5 */
      luaD_throw(L, LUA_ERRERR);
    }
    /* { const TValue *o2=(L->top - 1); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
    setobjs2s(L, L->top, L->top - 1);  /* move argument */
    /* { const TValue *o2=(errfunc); TValue *o1=(L->top - 1); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
    setobjs2s(L, L->top - 1, errfunc);
    /* incr_top(L); */
    {
        if ( (char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue) ) { 
            luaD_growstack(L, 1); 
        } else { 
            ((void)0);; 
        } 
        L->top++;
    };

    luaD_call(L, L->top - 2, 1); /* call it */
  }
  /*            LUA_ERRRUN  2 */
  luaD_throw(L, LUA_ERRRUN);
}


void luaG_runerror (lua_State *L, const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  addinfo(L, luaO_pushvfstring(L, fmt, argp));
  va_end(argp);
  luaG_errormsg(L);
}


