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
  if (!(((((ci)->func)->tt) == 6) && (!((&((ci)->func)->value.gc->cl))->c.isC))) {
    return -1; /* function is not a Lua function? */
  }
  if (ci == L->ci) {
    ci->savedpc = L->savedpc;
  }
  return (((int)((ci->savedpc) - (((&((ci)->func)->value.gc->cl))->l.p)->code)) - 1);
}


static int currentline (lua_State *L, CallInfo *ci) {
  int pc = currentpc(L, ci);
  if (pc < 0) {
    return -1; /* only active lua functions have current-line information */
  } else {
    return (((((&((ci)->func)->value.gc->cl))->l.p)->lineinfo) ? (((&((ci)->func)->value.gc->cl))->l.p)->lineinfo[pc] : 0);
  }
}


/*
** this function can be called asynchronous (e.g. during a signal)
*/
extern int lua_sethook (lua_State *L, lua_Hook func, int mask, int count) {
  if (func == ((void *)0) || mask == 0) { /* turn off hooks? */
    mask = 0;
    func = ((void *)0);
  }
  L->hook = func;
  L->basehookcount = count;
  (L->hookcount = L->basehookcount);
  L->hookmask = ((lu_byte)((mask)));
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
  ((void) 0);
  for (ci = L->ci; level > 0 && ci > L->base_ci; ci--) {
    level--;
    if ((!((&((ci)->func)->value.gc->cl))->c.isC)) { /* Lua function? */
      level -= ci->tailcalls; /* skip lost tail calls */
    }
  }
  if (level == 0 && ci > L->base_ci) { /* level found? */
    status = 1;
    ar->i_ci = ((int)((ci - L->base_ci)));
  }
  else if (level < 0) { /* level is of a lost tail call? */
    status = 1;
    ar->i_ci = 0;
  }
  else {
    status = 0; /* no such level */
  }
  ((void) 0);
  return status;
}


static Proto *getluaproto (CallInfo *ci) {
  return ((((((ci)->func)->tt) == 6) && (!((&((ci)->func)->value.gc->cl))->c.isC)) ? ((&((ci)->func)->value.gc->cl))->l.p : ((void *)0));
}


static const char *findlocal (lua_State *L, CallInfo *ci, int n) {
  const char *name;
  Proto *fp = getluaproto(ci);
  if (fp && (name = luaF_getlocalname(fp, n, currentpc(L, ci))) != ((void *)0)) {
    return name; /* is a local variable in a Lua function */
  } else {
    StkId limit = (ci == L->ci) ? L->top : (ci+1)->func;
    if (limit - ci->base >= n && n > 0) { /* is 'n' inside 'ci' stack? */
      return "(*temporary)";
    } else {
      return ((void *)0);
    }
  }
}


extern const char *lua_getlocal (lua_State *L, const lua_Debug *ar, int n) {
  CallInfo *ci = L->base_ci + ar->i_ci;
  const char *name = findlocal(L, ci, n);
  ((void) 0);
  if (name) {
      luaA_pushobject(L, ci->base + (n - 1));
  }
  ((void) 0);
  return name;
}


extern const char *lua_setlocal (lua_State *L, const lua_Debug *ar, int n) {
  CallInfo *ci = L->base_ci + ar->i_ci;
  const char *name = findlocal(L, ci, n);
  ((void) 0);
  if (name) {
    { const TValue *o2=(L->top - 1); TValue *o1=(ci->base + (n - 1)); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
  }
  L->top--; /* pop value */
  ((void) 0);
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
    ar->source = ((const char *)((cl->l.p->source) + 1));
    ar->linedefined = cl->l.p->linedefined;
    ar->lastlinedefined = cl->l.p->lastlinedefined;
    ar->what = (ar->linedefined == 0) ? "main" : "Lua";
  }
  luaO_chunkid(ar->short_src, ar->source, 60);
}


static void info_tailcall (lua_Debug *ar) {
  ar->name = ar->namewhat = "";
  ar->what = "tail";
  ar->lastlinedefined = ar->linedefined = ar->currentline = -1;
  ar->source = "=(tail call)";
  luaO_chunkid(ar->short_src, ar->source, 60);
  ar->nups = 0;
}


static void collectvalidlines (lua_State *L, Closure *f) {
  if (f == ((void *)0) || f->c.isC) {
    ((L->top)->tt=0);
  }
  else {
    Table *t = luaH_new(L, 0, 0);
    int *lineinfo = f->l.p->lineinfo;
    int i;
    for (i=0; i<f->l.p->sizelineinfo; i++) {
      { TValue *i_o=(luaH_setnum(L, t, lineinfo[i])); i_o->value.b=(1); i_o->tt=1; };
    }
    { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((t))); i_o->tt=5; ((void)0); };
  }
  {if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) luaD_growstack(L, 1); else ((void)0);; L->top++;};
}


static int auxgetinfo (lua_State *L, const char *what, lua_Debug *ar,
                    Closure *f, CallInfo *ci) {
  int status = 1;
  if (f == ((void *)0)) {
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
        ar->namewhat = (ci) ? getfuncname(L, ci, &ar->name) : ((void *)0);
        if (ar->namewhat == ((void *)0)) {
          ar->namewhat = ""; /* not found */
          ar->name = ((void *)0);
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
  Closure *f = ((void *)0);
  CallInfo *ci = ((void *)0);
  ((void) 0);
  if (*what == '>') {
    StkId func = L->top - 1;
    { (void)L; };
    what++; /* skip the '>' */
    f = (&(func)->value.gc->cl);
    L->top--; /* pop function */
  }
  else if (ar->i_ci != 0) { /* no tail call? */
    ci = L->base_ci + ar->i_ci;
    ((void)0);
    f = (&(ci->func)->value.gc->cl);
  }
  status = auxgetinfo(L, what, ar, f, ci);
  if (strchr(what, 'f')) {
    if (f == ((void *)0)) {
      ((L->top)->tt=0);
    } else {
      { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((f))); i_o->tt=6; ((void)0); };
    }
    {if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) luaD_growstack(L, 1); else ((void)0);; L->top++;};
  }
  if (strchr(what, 'L')) {
    collectvalidlines(L, f);
  }
  ((void) 0);
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
  if (!(pt->maxstacksize <= 250)) return 0;;
  if (!(pt->numparams+(pt->is_vararg & 1) <= pt->maxstacksize)) return 0;;
  if (!(!(pt->is_vararg & 4) || (pt->is_vararg & 1))) return 0;;
  if (!(pt->sizeupvalues <= pt->nups)) return 0;;
  if (!(pt->sizelineinfo == pt->sizecode || pt->sizelineinfo == 0)) return 0;;
  if (!(pt->sizecode > 0 && (((OpCode)(((pt->code[pt->sizecode-1])>>0) & ((~((~(Instruction)0)<<6))<<0)))) == OP_RETURN)) return 0;;
  return 1;
}


#define checkopenop(pt,pc)	luaG_checkopenop((pt)->code[(pc)+1])

int luaG_checkopenop (Instruction i) {
  switch ((((OpCode)(((i)>>0) & ((~((~(Instruction)0)<<6))<<0))))) {
    case OP_CALL:
    case OP_TAILCALL:
    case OP_RETURN:
    case OP_SETLIST: {
      if (!((((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)))) == 0)) return 0;;
      return 1;
    }
    default: return 0; /* invalid instruction after an open call */
  }
}


static int checkArgMode (const Proto *pt, int r, enum OpArgMask mode) {
  switch (mode) {
    case OpArgN: {
      if (!(r == 0)) return 0;;
      break;
    }
    case OpArgU: {
      break;
    }
    case OpArgR: {
      if (!((r) < (pt)->maxstacksize)) return 0;;
      break;
    }
    case OpArgK: {
      if (!(((r) & (1 << (9 - 1))) ? ((int)(r) & ~(1 << (9 - 1))) < pt->sizek : r < pt->maxstacksize)) return 0;;
      break;
    }
  }
  return 1;
}


static Instruction symbexec (const Proto *pt, int lastpc, int reg) {
  int pc;
  int last; /* stores position of last instruction that changed `reg' */
  last = pt->sizecode-1; /* points to final return (a `neutral' instruction) */
  if (!(precheck(pt))) return 0;;
  for (pc = 0; pc < lastpc; pc++) {
    Instruction i = pt->code[pc];
    OpCode op = (((OpCode)(((i)>>0) & ((~((~(Instruction)0)<<6))<<0))));
    int a = (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0))));
    int b = 0;
    int c = 0;
    if (!(op < (((int)(OP_VARARG)) + 1))) return 0;;
    if (!((a) < (pt)->maxstacksize)) return 0;;
    switch ((((enum OpMode)(luaP_opmodes[op] & 3)))) {
      case iABC: {
        b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))));
        c = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))));
        if (!(checkArgMode(pt, b, (((enum OpArgMask)((luaP_opmodes[op] >> 4) & 3)))))) return 0;;
        if (!(checkArgMode(pt, c, (((enum OpArgMask)((luaP_opmodes[op] >> 2) & 3)))))) return 0;;
        break;
      }
      case iABx: {
        b = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))));
        if ((((enum OpArgMask)((luaP_opmodes[op] >> 4) & 3))) == OpArgK) {
          if (!(b < pt->sizek)) return 0;;
        }
        break;
      }
      case iAsBx: {
        b = ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))-(((1<<(9 + 9))-1)>>1));
        if ((((enum OpArgMask)((luaP_opmodes[op] >> 4) & 3))) == OpArgR) {
          int dest = pc+1+b;
          if (!(0 <= dest && dest < pt->sizecode)) return 0;;
          if (dest > 0) {
            int j;
            /* check that it does not jump to a setlist count; this
               is tricky, because the count from a previous setlist may
               have the same value of an invalid setlist; so, we must
               go all the way back to the first of them (if any) */
            for (j = 0; j < dest; j++) {
              Instruction d = pt->code[dest-1-j];
              if (!((((OpCode)(((d)>>0) & ((~((~(Instruction)0)<<6))<<0)))) == OP_SETLIST && (((int)(((d)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)))) == 0)) {
                break;
              }
            }
            /* if 'j' is even, previous value is not a setlist (even if
               it looks like one) */
            if (!((j&1) == 0)) return 0;;
          }
        }
        break;
      }
    }
    if ((luaP_opmodes[op] & (1 << 6))) {
      if (a == reg) {
        last = pc; /* change register `a' */
      }
    }
    if ((luaP_opmodes[op] & (1 << 7))) {
      if (!(pc+2 < pt->sizecode)) return 0;; /* check skip */
      if (!((((OpCode)(((pt->code[pc+1])>>0) & ((~((~(Instruction)0)<<6))<<0)))) == OP_JMP)) return 0;;
    }
    switch (op) {
      case OP_LOADBOOL: {
        if (c == 1) { /* does it jump? */
          if (!(pc+2 < pt->sizecode)) return 0;; /* check its jump */
          if (!((((OpCode)(((pt->code[pc+1])>>0) & ((~((~(Instruction)0)<<6))<<0)))) != OP_SETLIST || (((int)(((pt->code[pc+1])>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)))) != 0)) return 0;;
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
        if (!(b < pt->nups)) return 0;;
        break;
      }
      case OP_GETGLOBAL:
      case OP_SETGLOBAL: {
        if (!((((&pt->k[b])->tt) == 4))) return 0;;
        break;
      }
      case OP_SELF: {
        if (!((a+1) < (pt)->maxstacksize)) return 0;;
        if (reg == a+1) {
          last = pc;
        }
        break;
      }
      case OP_CONCAT: {
        if (!(b < c)) return 0;; /* at least two operands */
        break;
      }
      case OP_TFORLOOP: {
        if (!(c >= 1)) return 0;; /* at least one result (control variable) */
        if (!((a+2+c) < (pt)->maxstacksize)) return 0;; /* space for results */
        if (reg >= a+2) {
          last = pc; /* affect all regs above its base */
        }
        break;
      }
      case OP_FORLOOP:
      case OP_FORPREP: {
        if (!((a+3) < (pt)->maxstacksize)) return 0;;
        /* go through */
      }
      case OP_JMP: {
        int dest = pc+1+b;
        /* not full check and jump is forward and do not skip `lastpc'? */
        if (reg != ((1<<8)-1) && pc < dest && dest <= lastpc) {
          pc += b; /* do the jump */
        }
        break;
      }
      case OP_CALL:
      case OP_TAILCALL: {
        if (b != 0) {
          if (!((a+b-1) < (pt)->maxstacksize)) return 0;;
        }
        c--; /* c = num. returns */
        if (c == (-1)) {
          if (!(luaG_checkopenop((pt)->code[(pc)+1]))) return 0;;
        }
        else if (c != 0) {
          if (!((a+c-1) < (pt)->maxstacksize)) return 0;;
        }
        if (reg >= a) {
          last = pc; /* affect all registers above base */
        }
        break;
      }
      case OP_RETURN: {
        b--; /* b = num. returns */
        if (b > 0) {
          if (!((a+b-1) < (pt)->maxstacksize)) return 0;;
        }
        break;
      }
      case OP_SETLIST: {
        if (b > 0) {
          if (!((a + b) < (pt)->maxstacksize)) return 0;;
        }
        if (c == 0) {
          pc++;
          if (!(pc < pt->sizecode - 1)) return 0;;
        }
        break;
      }
      case OP_CLOSURE: {
        int nup, j;
        if (!(b < pt->sizep)) return 0;;
        nup = pt->p[b]->nups;
        if (!(pc + nup < pt->sizecode)) return 0;;
        for (j = 1; j <= nup; j++) {
          OpCode op1 = (((OpCode)(((pt->code[pc + j])>>0) & ((~((~(Instruction)0)<<6))<<0))));
          if (!(op1 == OP_GETUPVAL || op1 == OP_MOVE)) return 0;;
        }
        if (reg != ((1<<8)-1)) { /* tracing? */
          pc += nup; /* do not 'execute' these pseudo-instructions */
        }
        break;
      }
      case OP_VARARG: {
        if (!((pt->is_vararg & 2) && !(pt->is_vararg & 4))) return 0;;
        b--;
        if (b == (-1)) {
          if (!(luaG_checkopenop((pt)->code[(pc)+1]))) return 0;;
        }
        if (!((a+b-1) < (pt)->maxstacksize)) return 0;;
        break;
      }
      default: {
        break;
      }
    }
  }
  return pt->code[last];
}


/* }====================================================== */

int luaG_checkcode (const Proto *pt) {
  return (symbexec(pt, pt->sizecode, ((1<<8)-1)) != 0);
}


static const char *kname (Proto *p, int c) {
  if (((c) & (1 << (9 - 1))) && (((&p->k[((int)(c) & ~(1 << (9 - 1)))])->tt) == 4)) {
    return ((const char *)(((&(&p->k[((int)(c) & ~(1 << (9 - 1)))])->value.gc->ts)) + 1));
  } else {
    return "?";
  }
}


static const char *getobjname (lua_State *L, CallInfo *ci, int stackpos,
                               const char **name) {
  if ((((((ci)->func)->tt) == 6) && (!((&((ci)->func)->value.gc->cl))->c.isC))) { /* a Lua function? */
    Proto *p = ((&((ci)->func)->value.gc->cl))->l.p;
    int pc = currentpc(L, ci);
    Instruction i;
    *name = luaF_getlocalname(p, stackpos+1, pc);
    if (*name) { /* is a local? */
      return "local";
    }
    i = symbexec(p, pc, stackpos); /* try symbolic execution */
    ((void)0);
    switch ((((OpCode)(((i)>>0) & ((~((~(Instruction)0)<<6))<<0))))) {
      case OP_GETGLOBAL: {
        int g = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0)))); /* global index */
        ((void)0);
        *name = ((const char *)(((&(&p->k[g])->value.gc->ts)) + 1));
        return "global";
      }
      case OP_MOVE: {
        int a = (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0))));
        int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)))); /* move from `b' to `a' */
        if (b < a) {
          return getobjname(L, ci, b, name); /* get name for `b' */
        }
        break;
      }
      case OP_GETTABLE: {
        int k = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)))); /* key index */
        *name = kname(p, k);
        return "field";
      }
      case OP_GETUPVAL: {
        int u = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)))); /* upvalue index */
        *name = p->upvalues ? ((const char *)((p->upvalues[u]) + 1)) : "?";
        return "upvalue";
      }
      case OP_SELF: {
        int k = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0)))); /* key index */
        *name = kname(p, k);
        return "method";
      }
      default: {
        break;
      }
    }
  }
  return ((void *)0); /* no useful name found */
}


static const char *getfuncname (lua_State *L, CallInfo *ci, const char **name) {
  Instruction i;
  if (((((((ci)->func)->tt) == 6) && (!((&((ci)->func)->value.gc->cl))->c.isC)) && ci->tailcalls > 0) || !(((((ci - 1)->func)->tt) == 6) && (!((&((ci - 1)->func)->value.gc->cl))->c.isC))) {
    return ((void *)0); /* calling function is not Lua (or is unknown) */
  }
  ci--; /* calling function */
  i = ((&((ci)->func)->value.gc->cl))->l.p->code[currentpc(L, ci)];
  if ((((OpCode)(((i)>>0) & ((~((~(Instruction)0)<<6))<<0)))) == OP_CALL || (((OpCode)(((i)>>0) & ((~((~(Instruction)0)<<6))<<0)))) == OP_TAILCALL ||
      (((OpCode)(((i)>>0) & ((~((~(Instruction)0)<<6))<<0)))) == OP_TFORLOOP) {
    return getobjname(L, ci, (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0)))), name);
  } else {
    return ((void *)0); /* no useful name can be found */
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
  const char *name = ((void *)0);
  const char *t = luaT_typenames[((o)->tt)];
  const char *kind = (isinstack(L->ci, o)) ?
                         getobjname(L, L->ci, ((int)((o - L->base))), &name) :
                         ((void *)0);
  if (kind) {
    luaG_runerror(L, "attempt to %s %s " "'" "%s" "'" " (a %s value)",
                op, kind, name, t);
  } else {
    luaG_runerror(L, "attempt to %s a %s value", op, t);
  }
}


void luaG_concaterror (lua_State *L, StkId p1, StkId p2) {
  if ((((p1)->tt) == 4) || (((p1)->tt) == 3)) {
    p1 = p2;
  }
  ((void)0);
  luaG_typeerror(L, p1, "concatenate");
}


void luaG_aritherror (lua_State *L, const TValue *p1, const TValue *p2) {
  TValue temp;
  if (luaV_tonumber(p1, &temp) == ((void *)0)) {
    p2 = p1; /* first operand is wrong */
  }
  luaG_typeerror(L, p2, "perform arithmetic on");
}


int luaG_ordererror (lua_State *L, const TValue *p1, const TValue *p2) {
  const char *t1 = luaT_typenames[((p1)->tt)];
  const char *t2 = luaT_typenames[((p2)->tt)];
  if (t1[2] == t2[2]) {
    luaG_runerror(L, "attempt to compare two %s values", t1);
  } else {
    luaG_runerror(L, "attempt to compare %s with %s", t1, t2);
  }
  return 0;
}


static void addinfo (lua_State *L, const char *msg) {
  CallInfo *ci = L->ci;
  if ((((((ci)->func)->tt) == 6) && (!((&((ci)->func)->value.gc->cl))->c.isC))) { /* is Lua code? */
    char buff[60]; /* add file:line information */
    int line = currentline(L, ci);
    luaO_chunkid(buff, ((const char *)((getluaproto(ci)->source) + 1)), 60);
    luaO_pushfstring(L, "%s:%d: %s", buff, line, msg);
  }
}


void luaG_errormsg (lua_State *L) {
  if (L->errfunc != 0) { /* is there an error handling function? */
    StkId errfunc = ((TValue *)((char *)L->stack + (L->errfunc)));
    if (!(((errfunc)->tt) == 6)) {
      luaD_throw(L, 5);
    }
    { const TValue *o2=(L->top - 1); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; /* move argument */
    { const TValue *o2=(errfunc); TValue *o1=(L->top - 1); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; /* push function */
    {if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) luaD_growstack(L, 1); else ((void)0);; L->top++;};
    luaD_call(L, L->top - 2, 1); /* call it */
  }
  luaD_throw(L, 2);
}


void luaG_runerror (lua_State *L, const char *fmt, ...) {
  va_list argp;
  __builtin_va_start(argp,fmt);
  addinfo(L, luaO_pushvfstring(L, fmt, argp));
  __builtin_va_end(argp);
  luaG_errormsg(L);
}


