/*
** $Id: lcode.c,v 2.25.1.5 2011/01/31 14:53:16 roberto Exp $
** Code generator for Lua
** See Copyright Notice in lua.h
*/


#include <stdlib.h>

#define lcode_c
#define LUA_CORE

#include "lua.h"

#include "lcode.h"
#include "ldebug.h"
#include "ldo.h"
#include "lgc.h"
#include "llex.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "ltable.h"


#define hasjumps(e)	((e)->t != (e)->f)


static int isnumeral(expdesc *e) {
  return (e->k == VKNUM && e->t == (-1) && e->f == (-1));
}


void luaK_nil (FuncState *fs, int from, int n) {
  Instruction *previous;
  if (fs->pc > fs->lasttarget) { /* no jumps to current position? */
    if (fs->pc == 0) { /* function start? */
      if (from >= fs->nactvar) {
        return; /* positions are already clean */
      }
    }
    else {
      previous = &fs->f->code[fs->pc-1];
      if ((((OpCode)(((*previous)>>0) & ((~((~(Instruction)0)<<6))<<0)))) == OP_LOADNIL) {
        int pfrom = (((int)(((*previous)>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0))));
        int pto = (((int)(((*previous)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))));
        if (pfrom <= from && from <= pto+1) { /* can connect both? */
          if (from+n-1 > pto) {
            ((*previous) = (((*previous)&(~((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))) | ((((Instruction)(from+n-1))<<(((0 + 6) + 8) + 9))&((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))));
          }
          return;
        }
      }
    }
  }
  luaK_codeABC(fs, OP_LOADNIL, from, from+n-1, 0); /* else no optimization */
}


int luaK_jump (FuncState *fs) {
  int jpc = fs->jpc; /* save list of jumps to here */
  int j;
  fs->jpc = (-1);
  j = luaK_codeABx(fs,OP_JMP,0,((-1))+(((1<<(9 + 9))-1)>>1));
  luaK_concat(fs, &j, jpc); /* keep them on hold */
  return j;
}


void luaK_ret (FuncState *fs, int first, int nret) {
  luaK_codeABC(fs, OP_RETURN, first, nret+1, 0);
}


static int condjump (FuncState *fs, OpCode op, int A, int B, int C) {
  luaK_codeABC(fs, op, A, B, C);
  return luaK_jump(fs);
}


static void fixjump (FuncState *fs, int pc, int dest) {
  Instruction *jmp = &fs->f->code[pc];
  int offset = dest-(pc+1);
  ((void)0);
  if (abs(offset) > (((1<<(9 + 9))-1)>>1)) {
    luaX_syntaxerror(fs->ls, "control structure too long");
  }
  (((*jmp)) = ((((*jmp))&(~((~((~(Instruction)0)<<(9 + 9)))<<((0 + 6) + 8)))) | ((((Instruction)(((unsigned int)((offset)+(((1<<(9 + 9))-1)>>1)))))<<((0 + 6) + 8))&((~((~(Instruction)0)<<(9 + 9)))<<((0 + 6) + 8)))));
}


/*
** returns current `pc' and marks it as a jump target (to avoid wrong
** optimizations with consecutive instructions not in the same basic block).
*/
int luaK_getlabel (FuncState *fs) {
  fs->lasttarget = fs->pc;
  return fs->pc;
}


static int getjump (FuncState *fs, int pc) {
  int offset = ((((int)(((fs->f->code[pc])>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))-(((1<<(9 + 9))-1)>>1));
  if (offset == (-1)) { /* point to itself represents end of list */
    return (-1); /* end of list */
  }
  else {
    return (pc+1)+offset; /* turn offset into absolute position */
  }
}


static Instruction *getjumpcontrol (FuncState *fs, int pc) {
  Instruction *pi = &fs->f->code[pc];
  if (pc >= 1 && (luaP_opmodes[(((OpCode)(((*(pi-1))>>0) & ((~((~(Instruction)0)<<6))<<0))))] & (1 << 7))) {
    return pi-1;
  }
  else {
    return pi;
  }
}


/*
** check whether list has any jump that do not produce a value
** (or produce an inverted value)
*/
static int need_value (FuncState *fs, int list) {
  for (; list != (-1); list = getjump(fs, list)) {
    Instruction i = *getjumpcontrol(fs, list);
    if ((((OpCode)(((i)>>0) & ((~((~(Instruction)0)<<6))<<0)))) != OP_TESTSET) {
      return 1;
    }
  }
  return 0; /* not found */
}


static int patchtestreg (FuncState *fs, int node, int reg) {
  Instruction *i = getjumpcontrol(fs, node);
  if ((((OpCode)(((*i)>>0) & ((~((~(Instruction)0)<<6))<<0)))) != OP_TESTSET) {
    return 0; /* cannot patch other instructions */
  }
  if (reg != ((1<<8)-1) && reg != (((int)(((*i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))) {
    ((*i) = (((*i)&(~((~((~(Instruction)0)<<8))<<(0 + 6)))) | ((((Instruction)(reg))<<(0 + 6))&((~((~(Instruction)0)<<8))<<(0 + 6)))));
  }
  else { /* no register to put value or register already has the value */
    *i = ((((Instruction)(OP_TEST))<<0) | (((Instruction)((((int)(((*i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))))<<(0 + 6)) | (((Instruction)(0))<<(((0 + 6) + 8) + 9)) | (((Instruction)((((int)(((*i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))))<<((0 + 6) + 8)));
  }
  return 1;
}


static void removevalues (FuncState *fs, int list) {
  for (; list != (-1); list = getjump(fs, list)) {
    patchtestreg(fs, list, ((1<<8)-1));
  }
}


static void patchlistaux (FuncState *fs, int list, int vtarget, int reg,
                          int dtarget) {
  while (list != (-1)) {
    int next = getjump(fs, list);
    if (patchtestreg(fs, list, reg)) {
      fixjump(fs, list, vtarget);
    } else {
      fixjump(fs, list, dtarget); /* jump to default target */
    }
    list = next;
  }
}


static void dischargejpc (FuncState *fs) {
  patchlistaux(fs, fs->jpc, fs->pc, ((1<<8)-1), fs->pc);
  fs->jpc = (-1);
}


void luaK_patchlist (FuncState *fs, int list, int target) {
  if (target == fs->pc) {
    luaK_patchtohere(fs, list);
  } else {
    ((void)0);
    patchlistaux(fs, list, target, ((1<<8)-1), target);
  }
}


void luaK_patchtohere (FuncState *fs, int list) {
  luaK_getlabel(fs);
  luaK_concat(fs, &fs->jpc, list);
}


void luaK_concat (FuncState *fs, int *l1, int l2) {
  if (l2 == (-1)) {
    return;
  } else if (*l1 == (-1)) {
    *l1 = l2;
  } else {
    int list = *l1;
    int next;
    while ((next = getjump(fs, list)) != (-1)) { /* find last element */
      list = next;
    }
    fixjump(fs, list, l2);
  }
}


void luaK_checkstack (FuncState *fs, int n) {
  int newstack = fs->freereg + n;
  if (newstack > fs->f->maxstacksize) {
    if (newstack >= 250) {
      luaX_syntaxerror(fs->ls, "function or expression too complex");
    }
    fs->f->maxstacksize = ((lu_byte)((newstack)));
  }
}


void luaK_reserveregs (FuncState *fs, int n) {
  luaK_checkstack(fs, n);
  fs->freereg += n;
}


static void freereg (FuncState *fs, int reg) {
  if (!((reg) & (1 << (9 - 1))) && reg >= fs->nactvar) {
    fs->freereg--;
    ((void)0);
  }
}


static void freeexp (FuncState *fs, expdesc *e) {
  if (e->k == VNONRELOC) {
    freereg(fs, e->u.s.info);
  }
}


static int addk (FuncState *fs, TValue *k, TValue *v) {
  lua_State *L = fs->L;
  TValue *idx = luaH_set(L, fs->h, k);
  Proto *f = fs->f;
  int oldsize = f->sizek;
  if ((((idx)->tt) == 3)) {
    ((void)0);
    return ((int)((((idx)->value.n))));
  }
  else { /* constant not found; create a new entry */
    { TValue *i_o=(idx); i_o->value.n=(((lua_Number)((fs->nk)))); i_o->tt=3; };
    if ((fs->nk)+1 > (f->sizek)) ((f->k)=((TValue *)(luaM_growaux_(L,f->k,&(f->sizek),sizeof(TValue),((1<<(9 + 9))-1),"constant table overflow"))));
    while (oldsize < f->sizek) {
      ((&f->k[oldsize++])->tt=0);
    }
    { const TValue *o2=(v); TValue *o1=(&f->k[fs->nk]); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
    { if (((((v)->tt) >= 4) && (((((v)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) && ((((((GCObject *)((f)))))->gch.marked) & ((1<<(2))))) luaC_barrierf(L,(((GCObject *)((f)))),((v)->value.gc)); };
    return fs->nk++;
  }
}


int luaK_stringK (FuncState *fs, TString *s) {
  TValue o;
  { TValue *i_o=(&o); i_o->value.gc=((GCObject *)((s))); i_o->tt=4; ((void)0); };
  return addk(fs, &o, &o);
}


int luaK_numberK (FuncState *fs, lua_Number r) {
  TValue o;
  { TValue *i_o=(&o); i_o->value.n=(r); i_o->tt=3; };
  return addk(fs, &o, &o);
}


static int boolK (FuncState *fs, int b) {
  TValue o;
  { TValue *i_o=(&o); i_o->value.b=(b); i_o->tt=1; };
  return addk(fs, &o, &o);
}


static int nilK (FuncState *fs) {
  TValue k, v;
  ((&v)->tt=0);
  /* cannot use nil as key; instead use table itself to represent nil */
  { TValue *i_o=(&k); i_o->value.gc=((GCObject *)((fs->h))); i_o->tt=5; ((void)0); };
  return addk(fs, &k, &v);
}


void luaK_setreturns (FuncState *fs, expdesc *e, int nresults) {
  if (e->k == VCALL) { /* expression is an open function call? */
    ((((fs)->f->code[(e)->u.s.info])) = (((((fs)->f->code[(e)->u.s.info]))&(~((~((~(Instruction)0)<<9))<<((0 + 6) + 8)))) | ((((Instruction)(nresults+1))<<((0 + 6) + 8))&((~((~(Instruction)0)<<9))<<((0 + 6) + 8)))));
  }
  else if (e->k == VVARARG) {
    ((((fs)->f->code[(e)->u.s.info])) = (((((fs)->f->code[(e)->u.s.info]))&(~((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))) | ((((Instruction)(nresults+1))<<(((0 + 6) + 8) + 9))&((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))));
    ((((fs)->f->code[(e)->u.s.info])) = (((((fs)->f->code[(e)->u.s.info]))&(~((~((~(Instruction)0)<<8))<<(0 + 6)))) | ((((Instruction)(fs->freereg))<<(0 + 6))&((~((~(Instruction)0)<<8))<<(0 + 6)))));
    luaK_reserveregs(fs, 1);
  }
}


void luaK_setoneret (FuncState *fs, expdesc *e) {
  if (e->k == VCALL) { /* expression is an open function call? */
    e->k = VNONRELOC;
    e->u.s.info = (((int)(((((fs)->f->code[(e)->u.s.info]))>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0))));
  }
  else if (e->k == VVARARG) {
    ((((fs)->f->code[(e)->u.s.info])) = (((((fs)->f->code[(e)->u.s.info]))&(~((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))) | ((((Instruction)(2))<<(((0 + 6) + 8) + 9))&((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))));
    e->k = VRELOCABLE; /* can relocate its simple result */
  }
}


void luaK_dischargevars (FuncState *fs, expdesc *e) {
  switch (e->k) {
    case VLOCAL: {
      e->k = VNONRELOC;
      break;
    }
    case VUPVAL: {
      e->u.s.info = luaK_codeABC(fs, OP_GETUPVAL, 0, e->u.s.info, 0);
      e->k = VRELOCABLE;
      break;
    }
    case VGLOBAL: {
      e->u.s.info = luaK_codeABx(fs, OP_GETGLOBAL, 0, e->u.s.info);
      e->k = VRELOCABLE;
      break;
    }
    case VINDEXED: {
      freereg(fs, e->u.s.aux);
      freereg(fs, e->u.s.info);
      e->u.s.info = luaK_codeABC(fs, OP_GETTABLE, 0, e->u.s.info, e->u.s.aux);
      e->k = VRELOCABLE;
      break;
    }
    case VVARARG:
    case VCALL: {
      luaK_setoneret(fs, e);
      break;
    }
    default: break; /* there is one value available (somewhere) */
  }
}


static int code_label (FuncState *fs, int A, int b, int jump) {
  luaK_getlabel(fs); /* those instructions may be jump targets */
  return luaK_codeABC(fs, OP_LOADBOOL, A, b, jump);
}


static void discharge2reg (FuncState *fs, expdesc *e, int reg) {
  luaK_dischargevars(fs, e);
  switch (e->k) {
    case VNIL: {
      luaK_nil(fs, reg, 1);
      break;
    }
    case VFALSE: case VTRUE: {
      luaK_codeABC(fs, OP_LOADBOOL, reg, e->k == VTRUE, 0);
      break;
    }
    case VK: {
      luaK_codeABx(fs, OP_LOADK, reg, e->u.s.info);
      break;
    }
    case VKNUM: {
      luaK_codeABx(fs, OP_LOADK, reg, luaK_numberK(fs, e->u.nval));
      break;
    }
    case VRELOCABLE: {
      Instruction *pc = &((fs)->f->code[(e)->u.s.info]);
      ((*pc) = (((*pc)&(~((~((~(Instruction)0)<<8))<<(0 + 6)))) | ((((Instruction)(reg))<<(0 + 6))&((~((~(Instruction)0)<<8))<<(0 + 6)))));
      break;
    }
    case VNONRELOC: {
      if (reg != e->u.s.info)
        luaK_codeABC(fs, OP_MOVE, reg, e->u.s.info, 0);
      break;
    }
    default: {
      ((void)0);
      return; /* nothing to do... */
    }
  }
  e->u.s.info = reg;
  e->k = VNONRELOC;
}


static void discharge2anyreg (FuncState *fs, expdesc *e) {
  if (e->k != VNONRELOC) {
    luaK_reserveregs(fs, 1);
    discharge2reg(fs, e, fs->freereg-1);
  }
}


static void exp2reg (FuncState *fs, expdesc *e, int reg) {
  discharge2reg(fs, e, reg);
  if (e->k == VJMP) {
    luaK_concat(fs, &e->t, e->u.s.info); /* put this jump in `t' list */
  }
  if (((e)->t != (e)->f)) {
    int final; /* position after whole expression */
    int p_f = (-1); /* position of an eventual LOAD false */
    int p_t = (-1); /* position of an eventual LOAD true */
    if (need_value(fs, e->t) || need_value(fs, e->f)) {
      int fj = (e->k == VJMP) ? (-1) : luaK_jump(fs);
      p_f = code_label(fs, reg, 0, 1);
      p_t = code_label(fs, reg, 1, 0);
      luaK_patchtohere(fs, fj);
    }
    final = luaK_getlabel(fs);
    patchlistaux(fs, e->f, final, reg, p_f);
    patchlistaux(fs, e->t, final, reg, p_t);
  }
  e->f = e->t = (-1);
  e->u.s.info = reg;
  e->k = VNONRELOC;
}


void luaK_exp2nextreg (FuncState *fs, expdesc *e) {
  luaK_dischargevars(fs, e);
  freeexp(fs, e);
  luaK_reserveregs(fs, 1);
  exp2reg(fs, e, fs->freereg - 1);
}


int luaK_exp2anyreg (FuncState *fs, expdesc *e) {
  luaK_dischargevars(fs, e);
  if (e->k == VNONRELOC) {
    if (!((e)->t != (e)->f)) {
      return e->u.s.info; /* exp is already in a register */
    }
    if (e->u.s.info >= fs->nactvar) { /* reg. is not a local? */
      exp2reg(fs, e, e->u.s.info); /* put value on it */
      return e->u.s.info;
    }
  }
  luaK_exp2nextreg(fs, e); /* default */
  return e->u.s.info;
}


void luaK_exp2val (FuncState *fs, expdesc *e) {
  if (((e)->t != (e)->f)) {
    luaK_exp2anyreg(fs, e);
  } else {
    luaK_dischargevars(fs, e);
  }
}


int luaK_exp2RK (FuncState *fs, expdesc *e) {
  luaK_exp2val(fs, e);
  switch (e->k) {
    case VKNUM:
    case VTRUE:
    case VFALSE:
    case VNIL: {
      if (fs->nk <= ((1 << (9 - 1)) - 1)) { /* constant fit in RK operand? */
        e->u.s.info = (e->k == VNIL) ? nilK(fs) :
                      (e->k == VKNUM) ? luaK_numberK(fs, e->u.nval) :
                                        boolK(fs, (e->k == VTRUE));
        e->k = VK;
        return ((e->u.s.info) | (1 << (9 - 1)));
      }
      else {
        break;
      }
    }
    case VK: {
      if (e->u.s.info <= ((1 << (9 - 1)) - 1)) { /* constant fit in argC? */
        return ((e->u.s.info) | (1 << (9 - 1)));
      }
      else {
        break;
      }
    }
    default: {
      break;
    }
  }
  /* not a constant in the right range: put it in a register */
  return luaK_exp2anyreg(fs, e);
}


void luaK_storevar (FuncState *fs, expdesc *var, expdesc *ex) {
  switch (var->k) {
    case VLOCAL: {
      freeexp(fs, ex);
      exp2reg(fs, ex, var->u.s.info);
      return;
    }
    case VUPVAL: {
      int e = luaK_exp2anyreg(fs, ex);
      luaK_codeABC(fs, OP_SETUPVAL, e, var->u.s.info, 0);
      break;
    }
    case VGLOBAL: {
      int e = luaK_exp2anyreg(fs, ex);
      luaK_codeABx(fs, OP_SETGLOBAL, e, var->u.s.info);
      break;
    }
    case VINDEXED: {
      int e = luaK_exp2RK(fs, ex);
      luaK_codeABC(fs, OP_SETTABLE, var->u.s.info, var->u.s.aux, e);
      break;
    }
    default: {
      ((void)0); /* invalid var kind to store */
      break;
    }
  }
  freeexp(fs, ex);
}


void luaK_self (FuncState *fs, expdesc *e, expdesc *key) {
  int func;
  luaK_exp2anyreg(fs, e);
  freeexp(fs, e);
  func = fs->freereg;
  luaK_reserveregs(fs, 2);
  luaK_codeABC(fs, OP_SELF, func, e->u.s.info, luaK_exp2RK(fs, key));
  freeexp(fs, key);
  e->u.s.info = func;
  e->k = VNONRELOC;
}


static void invertjump (FuncState *fs, expdesc *e) {
  Instruction *pc = getjumpcontrol(fs, e->u.s.info);
  ((void)0);
  ((*pc) = (((*pc)&(~((~((~(Instruction)0)<<8))<<(0 + 6)))) | ((((Instruction)(!((((int)(((*pc)>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0)))))))<<(0 + 6))&((~((~(Instruction)0)<<8))<<(0 + 6)))));
}


static int jumponcond (FuncState *fs, expdesc *e, int cond) {
  if (e->k == VRELOCABLE) {
    Instruction ie = ((fs)->f->code[(e)->u.s.info]);
    if ((((OpCode)(((ie)>>0) & ((~((~(Instruction)0)<<6))<<0)))) == OP_NOT) {
      fs->pc--; /* remove previous OP_NOT */
      return condjump(fs, OP_TEST, (((int)(((ie)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0)))), 0, !cond);
    }
    /* else go through */
  }
  discharge2anyreg(fs, e);
  freeexp(fs, e);
  return condjump(fs, OP_TESTSET, ((1<<8)-1), e->u.s.info, cond);
}


void luaK_goiftrue (FuncState *fs, expdesc *e) {
  int pc; /* pc of last jump */
  luaK_dischargevars(fs, e);
  switch (e->k) {
    case VK:
    case VKNUM:
    case VTRUE: {
      pc = (-1); /* always true; do nothing */
      break;
    }
    case VJMP: {
      invertjump(fs, e);
      pc = e->u.s.info;
      break;
    }
    default: {
      pc = jumponcond(fs, e, 0);
      break;
    }
  }
  luaK_concat(fs, &e->f, pc); /* insert last jump in `f' list */
  luaK_patchtohere(fs, e->t);
  e->t = (-1);
}


static void luaK_goiffalse (FuncState *fs, expdesc *e) {
  int pc; /* pc of last jump */
  luaK_dischargevars(fs, e);
  switch (e->k) {
    case VNIL:
    case VFALSE: {
      pc = (-1); /* always false; do nothing */
      break;
    }
    case VJMP: {
      pc = e->u.s.info;
      break;
    }
    default: {
      pc = jumponcond(fs, e, 1);
      break;
    }
  }
  luaK_concat(fs, &e->t, pc); /* insert last jump in `t' list */
  luaK_patchtohere(fs, e->f);
  e->f = (-1);
}


static void codenot (FuncState *fs, expdesc *e) {
  luaK_dischargevars(fs, e);
  switch (e->k) {
    case VNIL:
    case VFALSE: {
      e->k = VTRUE;
      break;
    }
    case VK:
    case VKNUM:
    case VTRUE: {
      e->k = VFALSE;
      break;
    }
    case VJMP: {
      invertjump(fs, e);
      break;
    }
    case VRELOCABLE:
    case VNONRELOC: {
      discharge2anyreg(fs, e);
      freeexp(fs, e);
      e->u.s.info = luaK_codeABC(fs, OP_NOT, 0, e->u.s.info, 0);
      e->k = VRELOCABLE;
      break;
    }
    default: {
      ((void)0); /* cannot happen */
      break;
    }
  }
  /* interchange true and false lists */
  { int temp = e->f; e->f = e->t; e->t = temp; }
  removevalues(fs, e->f);
  removevalues(fs, e->t);
}


void luaK_indexed (FuncState *fs, expdesc *t, expdesc *k) {
  t->u.s.aux = luaK_exp2RK(fs, k);
  t->k = VINDEXED;
}


static int constfolding (OpCode op, expdesc *e1, expdesc *e2) {
  lua_Number v1, v2, r;
  if (!isnumeral(e1) || !isnumeral(e2)) {
    return 0;
  }
  v1 = e1->u.nval;
  v2 = e2->u.nval;
  switch (op) {
    case OP_ADD: {
      r = ((v1)+(v2));
      break;
    }
    case OP_SUB: {
      r = ((v1)-(v2));
      break;
    }
    case OP_MUL: {
      r = ((v1)*(v2));
      break;
    }
    case OP_DIV: {
      if (v2 == 0) {
        return 0; /* do not attempt to divide by 0 */
      }
      r = ((v1)/(v2));
      break;
    }
    case OP_MOD: {
      if (v2 == 0) {
        return 0; /* do not attempt to divide by 0 */
      }
      r = ((v1) - floor((v1)/(v2))*(v2));
      break;
    }
    case OP_POW: {
      r = (pow(v1,v2));
      break;
    }
    case OP_UNM: {
      r = (-(v1));
      break;
    }
    case OP_LEN: {
      return 0; /* no constant folding for 'len' */
    }
    default: {
      ((void)0);
      r = 0;
      break;
    }
  }
  if ((!(((r))==((r))))) {
    return 0; /* do not attempt to produce NaN */
  }
  e1->u.nval = r;
  return 1;
}


static void codearith (FuncState *fs, OpCode op, expdesc *e1, expdesc *e2) {
  if (constfolding(op, e1, e2)) {
    return;
  } else {
    int o2 = (op != OP_UNM && op != OP_LEN) ? luaK_exp2RK(fs, e2) : 0;
    int o1 = luaK_exp2RK(fs, e1);
    if (o1 > o2) {
      freeexp(fs, e1);
      freeexp(fs, e2);
    }
    else {
      freeexp(fs, e2);
      freeexp(fs, e1);
    }
    e1->u.s.info = luaK_codeABC(fs, op, 0, o1, o2);
    e1->k = VRELOCABLE;
  }
}


static void codecomp (FuncState *fs, OpCode op, int cond, expdesc *e1,
                                                          expdesc *e2) {
  int o1 = luaK_exp2RK(fs, e1);
  int o2 = luaK_exp2RK(fs, e2);
  freeexp(fs, e2);
  freeexp(fs, e1);
  if (cond == 0 && op != OP_EQ) {
    int temp; /* exchange args to replace by `<' or `<=' */
    temp = o1; o1 = o2; o2 = temp; /* o1 <==> o2 */
    cond = 1;
  }
  e1->u.s.info = condjump(fs, op, cond, o1, o2);
  e1->k = VJMP;
}


void luaK_prefix (FuncState *fs, UnOpr op, expdesc *e) {
  expdesc e2;
  e2.t = e2.f = (-1); e2.k = VKNUM; e2.u.nval = 0;
  switch (op) {
    case OPR_MINUS: {
      if (!isnumeral(e)) {
        luaK_exp2anyreg(fs, e); /* cannot operate on non-numeric constants */
      }
      codearith(fs, OP_UNM, e, &e2);
      break;
    }
    case OPR_NOT: {
      codenot(fs, e);
      break;
    }
    case OPR_LEN: {
      luaK_exp2anyreg(fs, e); /* cannot operate on constants */
      codearith(fs, OP_LEN, e, &e2);
      break;
    }
    default: {
      ((void)0);
    }
  }
}


void luaK_infix (FuncState *fs, BinOpr op, expdesc *v) {
  switch (op) {
    case OPR_AND: {
      luaK_goiftrue(fs, v);
      break;
    }
    case OPR_OR: {
      luaK_goiffalse(fs, v);
      break;
    }
    case OPR_CONCAT: {
      luaK_exp2nextreg(fs, v); /* operand must be on the `stack' */
      break;
    }
    case OPR_ADD:
    case OPR_SUB:
    case OPR_MUL:
    case OPR_DIV:
    case OPR_MOD:
    case OPR_POW: {
      if (!isnumeral(v)) {
        luaK_exp2RK(fs, v);
      }
      break;
    }
    default: {
      luaK_exp2RK(fs, v);
      break;
    }
  }
}


void luaK_posfix (FuncState *fs, BinOpr op, expdesc *e1, expdesc *e2) {
  switch (op) {
    case OPR_AND: {
      ((void)0); /* list must be closed */
      luaK_dischargevars(fs, e2);
      luaK_concat(fs, &e2->f, e1->f);
      *e1 = *e2;
      break;
    }
    case OPR_OR: {
      ((void)0); /* list must be closed */
      luaK_dischargevars(fs, e2);
      luaK_concat(fs, &e2->t, e1->t);
      *e1 = *e2;
      break;
    }
    case OPR_CONCAT: {
      luaK_exp2val(fs, e2);
      if (e2->k == VRELOCABLE && (((OpCode)(((((fs)->f->code[(e2)->u.s.info]))>>0) & ((~((~(Instruction)0)<<6))<<0)))) == OP_CONCAT) {
        ((void)0);
        freeexp(fs, e1);
        ((((fs)->f->code[(e2)->u.s.info])) = (((((fs)->f->code[(e2)->u.s.info]))&(~((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))) | ((((Instruction)(e1->u.s.info))<<(((0 + 6) + 8) + 9))&((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))));
        e1->k = VRELOCABLE; e1->u.s.info = e2->u.s.info;
      }
      else {
        luaK_exp2nextreg(fs, e2); /* operand must be on the 'stack' */
        codearith(fs, OP_CONCAT, e1, e2);
      }
      break;
    }
    case OPR_ADD: {
      codearith(fs, OP_ADD, e1, e2);
      break;
    }
    case OPR_SUB: {
      codearith(fs, OP_SUB, e1, e2);
      break;
    }
    case OPR_MUL: {
      codearith(fs, OP_MUL, e1, e2);
      break;
    }
    case OPR_DIV: {
      codearith(fs, OP_DIV, e1, e2);
      break;
    }
    case OPR_MOD: {
      codearith(fs, OP_MOD, e1, e2);
      break;
    }
    case OPR_POW: {
      codearith(fs, OP_POW, e1, e2);
      break;
    }
    case OPR_EQ: {
      codecomp(fs, OP_EQ, 1, e1, e2);
      break;
    }
    case OPR_NE: {
      codecomp(fs, OP_EQ, 0, e1, e2);
      break;
    }
    case OPR_LT: {
      codecomp(fs, OP_LT, 1, e1, e2);
      break;
    }
    case OPR_LE: {
      codecomp(fs, OP_LE, 1, e1, e2);
      break;
    }
    case OPR_GT: {
      codecomp(fs, OP_LT, 0, e1, e2);
      break;
    }
    case OPR_GE: {
      codecomp(fs, OP_LE, 0, e1, e2);
      break;
    }
    default: {
      ((void)0);
    }
  }
}


void luaK_fixline (FuncState *fs, int line) {
  fs->f->lineinfo[fs->pc - 1] = line;
}


static int luaK_code (FuncState *fs, Instruction i, int line) {
  Proto *f = fs->f;
  dischargejpc(fs); /* `pc' will change */
  /* put new instruction in code array */
  if ((fs->pc)+1 > (f->sizecode)) ((f->code)=((Instruction *)(luaM_growaux_(fs->L,f->code,&(f->sizecode),sizeof(Instruction),(2147483647 -2),"code size overflow"))));
  f->code[fs->pc] = i;
  /* save corresponding line information */
  if ((fs->pc)+1 > (f->sizelineinfo)) ((f->lineinfo)=((int *)(luaM_growaux_(fs->L,f->lineinfo,&(f->sizelineinfo),sizeof(int),(2147483647 -2),"code size overflow"))));
  f->lineinfo[fs->pc] = line;
  return fs->pc++;
}


int luaK_codeABC (FuncState *fs, OpCode o, int a, int b, int c) {
  ((void)0);
  ((void)0);
  ((void)0);
  return luaK_code(fs, ((((Instruction)(o))<<0) | (((Instruction)(a))<<(0 + 6)) | (((Instruction)(b))<<(((0 + 6) + 8) + 9)) | (((Instruction)(c))<<((0 + 6) + 8))), fs->ls->lastline);
}


int luaK_codeABx (FuncState *fs, OpCode o, int a, unsigned int bc) {
  ((void)0);
  ((void)0);
  return luaK_code(fs, ((((Instruction)(o))<<0) | (((Instruction)(a))<<(0 + 6)) | (((Instruction)(bc))<<((0 + 6) + 8))), fs->ls->lastline);
}


void luaK_setlist (FuncState *fs, int base, int nelems, int tostore) {
  int c = (nelems - 1)/50 + 1;
  int b = (tostore == (-1)) ? 0 : tostore;
  ((void)0);
  if (c <= ((1<<9)-1)) {
    luaK_codeABC(fs, OP_SETLIST, base, b, c);
  } else {
    luaK_codeABC(fs, OP_SETLIST, base, b, 0);
    luaK_code(fs, ((Instruction)(c)), fs->ls->lastline);
  }
  fs->freereg = base + 1; /* free registers with list values */
}


