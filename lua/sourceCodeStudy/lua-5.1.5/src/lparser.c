/*
** $Id: lparser.c,v 2.42.1.4 2011/10/21 19:31:42 roberto Exp $
** Lua Parser
** See Copyright Notice in lua.h
*/


#include <string.h>

#define lparser_c
#define LUA_CORE

#include "lua.h"

#include "lcode.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "llex.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"



#define hasmultret(k)		((k) == VCALL || (k) == VVARARG)

#define getlocvar(fs, i)	((fs)->f->locvars[(fs)->actvar[i]])

#define luaY_checklimit(fs,v,l,m)	if ((v)>(l)) errorlimit(fs,l,m)


/*
** nodes for block list (list of active blocks)
*/
typedef struct BlockCnt {
  struct BlockCnt *previous; /* chain */
  int breaklist; /* list of jumps out of this loop */
  lu_byte nactvar; /* # active locals outside the breakable structure */
  lu_byte upval; /* true if some variable in the block is an upvalue */
  lu_byte isbreakable; /* true if `block' is a loop */
} BlockCnt;


/*
** prototypes for recursive non-terminal functions
*/
static void chunk (LexState *ls);
static void expr (LexState *ls, expdesc *v);


static void anchor_token (LexState *ls) {
  if (ls->t.token == TK_NAME || ls->t.token == TK_STRING) {
    TString *ts = ls->t.seminfo.ts;
    luaX_newstring(ls, ((const char *)((ts) + 1)), ts->tsv.len);
  }
}


static void error_expected (LexState *ls, int token) {
  luaX_syntaxerror(ls,
      luaO_pushfstring(ls->L, "'" "%s" "'" " expected", luaX_token2str(ls, token)));
}


static void errorlimit (FuncState *fs, int limit, const char *what) {
  const char *msg = (fs->f->linedefined == 0) ?
    luaO_pushfstring(fs->L, "main function has more than %d %s", limit, what) :
    luaO_pushfstring(fs->L, "function at line %d has more than %d %s",
                            fs->f->linedefined, limit, what);
  luaX_lexerror(fs->ls, msg, 0);
}


static int testnext (LexState *ls, int c) {
  if (ls->t.token == c) {
    luaX_next(ls);
    return 1;
  }
  else {
    return 0;
  }
}


static void check (LexState *ls, int c) {
  if (ls->t.token != c) {
    error_expected(ls, c);
  }
}


static void checknext (LexState *ls, int c) {
  check(ls, c);
  luaX_next(ls);
}


static void check_match (LexState *ls, int what, int who, int where) {
  if (!testnext(ls, what)) {
    if (where == ls->linenumber) {
      error_expected(ls, what);
    } else {
      luaX_syntaxerror(ls, luaO_pushfstring(ls->L,
             "'" "%s" "'" " expected (to close " "'" "%s" "'" " at line %d)",
              luaX_token2str(ls, what), luaX_token2str(ls, who), where));
    }
  }
}


static TString *str_checkname (LexState *ls) {
  TString *ts;
  check(ls, TK_NAME);
  ts = ls->t.seminfo.ts;
  luaX_next(ls);
  return ts;
}


static void init_exp (expdesc *e, expkind k, int i) {
  e->f = e->t = (-1);
  e->k = k;
  e->u.s.info = i;
}


static void codestring (LexState *ls, expdesc *e, TString *s) {
  init_exp(e, VK, luaK_stringK(ls->fs, s));
}


static void checkname(LexState *ls, expdesc *e) {
  codestring(ls, e, str_checkname(ls));
}


static int registerlocalvar (LexState *ls, TString *varname) {
  FuncState *fs = ls->fs;
  Proto *f = fs->f;
  int oldsize = f->sizelocvars;
  if ((fs->nlocvars)+1 > (f->sizelocvars)) ((f->locvars)=((LocVar *)(luaM_growaux_(ls->L,f->locvars,&(f->sizelocvars),sizeof(LocVar),32767,"too many local variables"))));
  while (oldsize < f->sizelocvars) {
    f->locvars[oldsize++].varname = ((void *)0);
  }
  f->locvars[fs->nlocvars].varname = varname;
  { if (((((((GCObject *)((varname)))))->gch.marked) & ((((1<<(0)) | (1<<(1)))))) && ((((((GCObject *)((f)))))->gch.marked) & ((1<<(2))))) luaC_barrierf(ls->L,(((GCObject *)((f)))),(((GCObject *)((varname))))); };
  return fs->nlocvars++;
}


static void new_localvar (LexState *ls, TString *name, int n) {
  FuncState *fs = ls->fs;
  if ((fs->nactvar+n+1)>(200)) errorlimit(fs,200,"local variables");
  fs->actvar[fs->nactvar+n] = ((unsigned short)(registerlocalvar(ls, name)));
}


static void adjustlocalvars (LexState *ls, int nvars) {
  FuncState *fs = ls->fs;
  fs->nactvar = ((lu_byte)((fs->nactvar + nvars)));
  for (; nvars; nvars--) {
    ((fs)->f->locvars[(fs)->actvar[fs->nactvar - nvars]]).startpc = fs->pc;
  }
}


static void removevars (LexState *ls, int tolevel) {
  FuncState *fs = ls->fs;
  while (fs->nactvar > tolevel) {
    ((fs)->f->locvars[(fs)->actvar[--fs->nactvar]]).endpc = fs->pc;
  }
}


static int indexupvalue (FuncState *fs, TString *name, expdesc *v) {
  int i;
  Proto *f = fs->f;
  int oldsize = f->sizeupvalues;
  for (i=0; i<f->nups; i++) {
    if (fs->upvalues[i].k == v->k && fs->upvalues[i].info == v->u.s.info) {
      ((void)0);
      return i;
    }
  }
  /* new one */
  if ((f->nups + 1)>(60)) errorlimit(fs,60,"upvalues");
  if ((f->nups)+1 > (f->sizeupvalues)) ((f->upvalues)=((TString * *)(luaM_growaux_(fs->L,f->upvalues,&(f->sizeupvalues),sizeof(TString *),(2147483647 -2),""))));
  while (oldsize < f->sizeupvalues) {
    f->upvalues[oldsize++] = ((void *)0);
  }
  f->upvalues[f->nups] = name;
  { if (((((((GCObject *)((name)))))->gch.marked) & ((((1<<(0)) | (1<<(1)))))) && ((((((GCObject *)((f)))))->gch.marked) & ((1<<(2))))) luaC_barrierf(fs->L,(((GCObject *)((f)))),(((GCObject *)((name))))); };
  ((void)0);
  fs->upvalues[f->nups].k = ((lu_byte)((v->k)));
  fs->upvalues[f->nups].info = ((lu_byte)((v->u.s.info)));
  return f->nups++;
}


static int searchvar (FuncState *fs, TString *n) {
  int i;
  for (i=fs->nactvar-1; i >= 0; i--) {
    if (n == ((fs)->f->locvars[(fs)->actvar[i]]).varname) {
      return i;
    }
  }
  return -1; /* not found */
}


static void markupval (FuncState *fs, int level) {
  BlockCnt *bl = fs->bl;
  while (bl && bl->nactvar > level) {
    bl = bl->previous;
  }
  if (bl) {
    bl->upval = 1;
  }
}


static int singlevaraux (FuncState *fs, TString *n, expdesc *var, int base) {
  if (fs == ((void *)0)) { /* no more levels? */
    init_exp(var, VGLOBAL, ((1<<8)-1)); /* default is global variable */
    return VGLOBAL;
  }
  else {
    int v = searchvar(fs, n); /* look up at current level */
    if (v >= 0) {
      init_exp(var, VLOCAL, v);
      if (!base) {
        markupval(fs, v); /* local will be used as an upval */
      }
      return VLOCAL;
    }
    else { /* not found at current level; try upper one */
      if (singlevaraux(fs->prev, n, var, 0) == VGLOBAL) {
        return VGLOBAL;
      }
      var->u.s.info = indexupvalue(fs, n, var); /* else was LOCAL or UPVAL */
      var->k = VUPVAL; /* upvalue in this level */
      return VUPVAL;
    }
  }
}


static void singlevar (LexState *ls, expdesc *var) {
  TString *varname = str_checkname(ls);
  FuncState *fs = ls->fs;
  if (singlevaraux(fs, varname, var, 1) == VGLOBAL) {
    var->u.s.info = luaK_stringK(fs, varname); /* info points to global name */
  }
}


static void adjust_assign (LexState *ls, int nvars, int nexps, expdesc *e) {
  FuncState *fs = ls->fs;
  int extra = nvars - nexps;
  if (((e->k) == VCALL || (e->k) == VVARARG)) {
    extra++; /* includes call itself */
    if (extra < 0) {
      extra = 0;
    }
    luaK_setreturns(fs, e, extra); /* last exp. provides the difference */
    if (extra > 1) {
      luaK_reserveregs(fs, extra-1);
    }
  }
  else {
    if (e->k != VVOID) {
      luaK_exp2nextreg(fs, e); /* close last expression */
    }
    if (extra > 0) {
      int reg = fs->freereg;
      luaK_reserveregs(fs, extra);
      luaK_nil(fs, reg, extra);
    }
  }
}


static void enterlevel (LexState *ls) {
  if (++ls->L->nCcalls > 200) {
 luaX_lexerror(ls, "chunk has too many syntax levels", 0);
  }
}


static void enterblock (FuncState *fs, BlockCnt *bl, lu_byte isbreakable) {
  bl->breaklist = (-1);
  bl->isbreakable = isbreakable;
  bl->nactvar = fs->nactvar;
  bl->upval = 0;
  bl->previous = fs->bl;
  fs->bl = bl;
  ((void)0);
}


static void leaveblock (FuncState *fs) {
  BlockCnt *bl = fs->bl;
  fs->bl = bl->previous;
  removevars(fs->ls, bl->nactvar);
  if (bl->upval) {
    luaK_codeABC(fs, OP_CLOSE, bl->nactvar, 0, 0);
  }
  /* a block either controls scope or breaks (never both) */
  ((void)0);
  ((void)0);
  fs->freereg = fs->nactvar; /* free registers */
  luaK_patchtohere(fs, bl->breaklist);
}


static void pushclosure (LexState *ls, FuncState *func, expdesc *v) {
  FuncState *fs = ls->fs;
  Proto *f = fs->f;
  int oldsize = f->sizep;
  int i;
  if ((fs->np)+1 > (f->sizep)) ((f->p)=((Proto * *)(luaM_growaux_(ls->L,f->p,&(f->sizep),sizeof(Proto *),((1<<(9 + 9))-1),"constant table overflow"))));
  while (oldsize < f->sizep) {
    f->p[oldsize++] = ((void *)0);
  }
  f->p[fs->np++] = func->f;
  { if (((((((GCObject *)((func->f)))))->gch.marked) & ((((1<<(0)) | (1<<(1)))))) && ((((((GCObject *)((f)))))->gch.marked) & ((1<<(2))))) luaC_barrierf(ls->L,(((GCObject *)((f)))),(((GCObject *)((func->f))))); };
  init_exp(v, VRELOCABLE, luaK_codeABx(fs, OP_CLOSURE, 0, fs->np-1));
  for (i=0; i<func->f->nups; i++) {
    OpCode o = (func->upvalues[i].k == VLOCAL) ? OP_MOVE : OP_GETUPVAL;
    luaK_codeABC(fs, o, 0, func->upvalues[i].info, 0);
  }
}


static void open_func (LexState *ls, FuncState *fs) {
  lua_State *L = ls->L;
  Proto *f = luaF_newproto(L);
  fs->f = f;
  fs->prev = ls->fs; /* linked list of funcstates */
  fs->ls = ls;
  fs->L = L;
  ls->fs = fs;
  fs->pc = 0;
  fs->lasttarget = -1;
  fs->jpc = (-1);
  fs->freereg = 0;
  fs->nk = 0;
  fs->np = 0;
  fs->nlocvars = 0;
  fs->nactvar = 0;
  fs->bl = ((void *)0);
  f->source = ls->source;
  f->maxstacksize = 2; /* registers 0/1 are always valid */
  fs->h = luaH_new(L, 0, 0);
  /* anchor table of constants and prototype (to avoid being collected) */
  { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((fs->h))); i_o->tt=5; ((void)0); };
  {if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) luaD_growstack(L, 1); else ((void)0);; L->top++;};
  { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((f))); i_o->tt=(8 +1); ((void)0); };
  {if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) luaD_growstack(L, 1); else ((void)0);; L->top++;};
}


static void close_func (LexState *ls) {
  lua_State *L = ls->L;
  FuncState *fs = ls->fs;
  Proto *f = fs->f;
  removevars(ls, 0);
  luaK_ret(fs, 0, 0); /* final return */
  ((f->code)=((Instruction *)(((((size_t)((fs->pc)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(Instruction))) ? luaM_realloc_(L, (f->code), (f->sizecode)*(sizeof(Instruction)), (fs->pc)*(sizeof(Instruction))) : luaM_toobig(L)))));
  f->sizecode = fs->pc;
  ((f->lineinfo)=((int *)(((((size_t)((fs->pc)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(int))) ? luaM_realloc_(L, (f->lineinfo), (f->sizelineinfo)*(sizeof(int)), (fs->pc)*(sizeof(int))) : luaM_toobig(L)))));
  f->sizelineinfo = fs->pc;
  ((f->k)=((TValue *)(((((size_t)((fs->nk)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(TValue))) ? luaM_realloc_(L, (f->k), (f->sizek)*(sizeof(TValue)), (fs->nk)*(sizeof(TValue))) : luaM_toobig(L)))));
  f->sizek = fs->nk;
  ((f->p)=((Proto * *)(((((size_t)((fs->np)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(Proto *))) ? luaM_realloc_(L, (f->p), (f->sizep)*(sizeof(Proto *)), (fs->np)*(sizeof(Proto *))) : luaM_toobig(L)))));
  f->sizep = fs->np;
  ((f->locvars)=((LocVar *)(((((size_t)((fs->nlocvars)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(LocVar))) ? luaM_realloc_(L, (f->locvars), (f->sizelocvars)*(sizeof(LocVar)), (fs->nlocvars)*(sizeof(LocVar))) : luaM_toobig(L)))));
  f->sizelocvars = fs->nlocvars;
  ((f->upvalues)=((TString * *)(((((size_t)((f->nups)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(TString *))) ? luaM_realloc_(L, (f->upvalues), (f->sizeupvalues)*(sizeof(TString *)), (f->nups)*(sizeof(TString *))) : luaM_toobig(L)))));
  f->sizeupvalues = f->nups;
  ((void)0);
  ((void)0);
  ls->fs = fs->prev;
  /* last token read was anchored in defunct function; must reanchor it */
  if (fs) {
    anchor_token(ls);
  }
  L->top -= 2; /* remove table and prototype from the stack */
}


Proto *luaY_parser (lua_State *L, ZIO *z, Mbuffer *buff, const char *name) {
  struct LexState lexstate;
  struct FuncState funcstate;
  lexstate.buff = buff;
  luaX_setinput(L, &lexstate, z, (luaS_newlstr(L, name, strlen(name))));
  open_func(&lexstate, &funcstate);
  funcstate.f->is_vararg = 2; /* main func. is always vararg */
  luaX_next(&lexstate); /* read first token */
  chunk(&lexstate);
  check(&lexstate, TK_EOS);
  close_func(&lexstate);
  ((void)0);
  ((void)0);
  ((void)0);
  return funcstate.f;
}


/*============================================================*/
/* GRAMMAR RULES */
/*============================================================*/
static void field (LexState *ls, expdesc *v) {
  /* field -> ['.' | ':'] NAME */
  FuncState *fs = ls->fs;
  expdesc key;
  luaK_exp2anyreg(fs, v);
  luaX_next(ls); /* skip the dot or colon */
  checkname(ls, &key);
  luaK_indexed(fs, v, &key);
}


static void yindex (LexState *ls, expdesc *v) {
  /* index -> '[' expr ']' */
  luaX_next(ls); /* skip the '[' */
  expr(ls, v);
  luaK_exp2val(ls->fs, v);
  checknext(ls, ']');
}


/*
** {======================================================================
** Rules for Constructors
** =======================================================================
*/
struct ConsControl {
  expdesc v; /* last list item read */
  expdesc *t; /* table descriptor */
  int nh; /* total number of `record' elements */
  int na; /* total number of array elements */
  int tostore; /* number of array elements pending to be stored */
};


static void recfield (LexState *ls, struct ConsControl *cc) {
  /* recfield -> (NAME | `['exp1`]') = exp1 */
  FuncState *fs = ls->fs;
  int reg = ls->fs->freereg;
  expdesc key, val;
  int rkkey;
  if (ls->t.token == TK_NAME) {
    if ((cc->nh)>((2147483647 -2))) errorlimit(fs,(2147483647 -2),"items in a constructor");
    checkname(ls, &key);
  }
  else { /* ls->t.token == '[' */
    yindex(ls, &key);
  }
  cc->nh++;
  checknext(ls, '=');
  rkkey = luaK_exp2RK(fs, &key);
  expr(ls, &val);
  luaK_codeABC(fs, OP_SETTABLE, cc->t->u.s.info, rkkey, luaK_exp2RK(fs, &val));
  fs->freereg = reg; /* free registers */
}


static void closelistfield (FuncState *fs, struct ConsControl *cc) {
  if (cc->v.k == VVOID) {
    return; /* there is no list item */
  }
  luaK_exp2nextreg(fs, &cc->v);
  cc->v.k = VVOID;
  if (cc->tostore == 50) {
    luaK_setlist(fs, cc->t->u.s.info, cc->na, cc->tostore); /* flush */
    cc->tostore = 0; /* no more items pending */
  }
}


static void lastlistfield (FuncState *fs, struct ConsControl *cc) {
  if (cc->tostore == 0) {
    return;
  }
  if (((cc->v.k) == VCALL || (cc->v.k) == VVARARG)) {
    luaK_setreturns(fs, &cc->v, (-1));
    luaK_setlist(fs, cc->t->u.s.info, cc->na, (-1));
    cc->na--; /* do not count last expression (unknown number of elements) */
  }
  else {
    if (cc->v.k != VVOID) {
      luaK_exp2nextreg(fs, &cc->v);
    }
    luaK_setlist(fs, cc->t->u.s.info, cc->na, cc->tostore);
  }
}


static void listfield (LexState *ls, struct ConsControl *cc) {
  expr(ls, &cc->v);
  if ((cc->na)>((2147483647 -2))) errorlimit(ls->fs,(2147483647 -2),"items in a constructor");
  cc->na++;
  cc->tostore++;
}


static void constructor (LexState *ls, expdesc *t) {
  /* constructor -> ?? */
  FuncState *fs = ls->fs;
  int line = ls->linenumber;
  int pc = luaK_codeABC(fs, OP_NEWTABLE, 0, 0, 0);
  struct ConsControl cc;
  cc.na = cc.nh = cc.tostore = 0;
  cc.t = t;
  init_exp(t, VRELOCABLE, pc);
  init_exp(&cc.v, VVOID, 0); /* no value (yet) */
  luaK_exp2nextreg(ls->fs, t); /* fix it at stack top (for gc) */
  checknext(ls, '{');
  do {
    ((void)0);
    if (ls->t.token == '}') {
      break;
    }
    closelistfield(fs, &cc);
    switch(ls->t.token) {
      case TK_NAME: { /* may be listfields or recfields */
        luaX_lookahead(ls);
        if (ls->lookahead.token != '=') { /* expression? */
          listfield(ls, &cc);
        } else {
          recfield(ls, &cc);
        }
        break;
      }
      case '[': { /* constructor_item -> recfield */
        recfield(ls, &cc);
        break;
      }
      default: { /* constructor_part -> listfield */
        listfield(ls, &cc);
        break;
      }
    }
  } while (testnext(ls, ',') || testnext(ls, ';'));
  check_match(ls, '}', '{', line);
  lastlistfield(fs, &cc);
  ((fs->f->code[pc]) = (((fs->f->code[pc])&(~((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))) | ((((Instruction)(luaO_int2fb(cc.na)))<<(((0 + 6) + 8) + 9))&((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9))))); /* set initial array size */
  ((fs->f->code[pc]) = (((fs->f->code[pc])&(~((~((~(Instruction)0)<<9))<<((0 + 6) + 8)))) | ((((Instruction)(luaO_int2fb(cc.nh)))<<((0 + 6) + 8))&((~((~(Instruction)0)<<9))<<((0 + 6) + 8))))); /* set initial table size */
}


/* }====================================================================== */
static void parlist (LexState *ls) {
  /* parlist -> [ param { `,' param } ] */
  FuncState *fs = ls->fs;
  Proto *f = fs->f;
  int nparams = 0;
  f->is_vararg = 0;
  if (ls->t.token != ')') { /* is `parlist' not empty? */
    do {
      switch (ls->t.token) {
        case TK_NAME: { /* param -> NAME */
          new_localvar(ls, str_checkname(ls), nparams++);
          break;
        }
        case TK_DOTS: { /* param -> `...' */
          luaX_next(ls);
          /* use `arg' as default name */
          new_localvar(ls, luaX_newstring(ls, "" "arg", (sizeof("arg")/sizeof(char))-1), nparams++);
          f->is_vararg = 1 | 4;
          f->is_vararg |= 2;
          break;
        }
        default: {
          luaX_syntaxerror(ls, "<name> or " "'" "..." "'" " expected");
        }
      }
    } while (!f->is_vararg && testnext(ls, ','));
  }
  adjustlocalvars(ls, nparams);
  f->numparams = ((lu_byte)((fs->nactvar - (f->is_vararg & 1))));
  luaK_reserveregs(fs, fs->nactvar); /* reserve register for parameters */
}


static void body (LexState *ls, expdesc *e, int needself, int line) {
  /* body ->  `(' parlist `)' chunk END */
  FuncState new_fs;
  open_func(ls, &new_fs);
  new_fs.f->linedefined = line;
  checknext(ls, '(');
  if (needself) {
    new_localvar(ls, luaX_newstring(ls, "" "self", (sizeof("self")/sizeof(char))-1), 0);
    adjustlocalvars(ls, 1);
  }
  parlist(ls);
  checknext(ls, ')');
  chunk(ls);
  new_fs.f->lastlinedefined = ls->linenumber;
  check_match(ls, TK_END, TK_FUNCTION, line);
  close_func(ls);
  pushclosure(ls, &new_fs, e);
}


static int explist1 (LexState *ls, expdesc *v) {
  /* explist1 -> expr { `,' expr } */
  int n = 1; /* at least one expression */
  expr(ls, v);
  while (testnext(ls, ',')) {
    luaK_exp2nextreg(ls->fs, v);
    expr(ls, v);
    n++;
  }
  return n;
}


static void funcargs (LexState *ls, expdesc *f) {
  FuncState *fs = ls->fs;
  expdesc args;
  int base, nparams;
  int line = ls->linenumber;
  switch (ls->t.token) {
    case '(': { /* funcargs -> `(' [ explist1 ] `)' */
      if (line != ls->lastline) {
        luaX_syntaxerror(ls,"ambiguous syntax (function call x new statement)");
      }
      luaX_next(ls);
      if (ls->t.token == ')') { /* arg list is empty? */
        args.k = VVOID;
      } else {
        explist1(ls, &args);
        luaK_setreturns(fs, &args, (-1));
      }
      check_match(ls, ')', '(', line);
      break;
    }
    case '{': { /* funcargs -> constructor */
      constructor(ls, &args);
      break;
    }
    case TK_STRING: { /* funcargs -> STRING */
      codestring(ls, &args, ls->t.seminfo.ts);
      luaX_next(ls); /* must use `seminfo' before `next' */
      break;
    }
    default: {
      luaX_syntaxerror(ls, "function arguments expected");
      return;
    }
  }
  ((void)0);
  base = f->u.s.info; /* base register for call */
  if (((args.k) == VCALL || (args.k) == VVARARG)) {
    nparams = (-1); /* open call */
  } else {
    if (args.k != VVOID) {
      luaK_exp2nextreg(fs, &args); /* close last argument */
    }
    nparams = fs->freereg - (base+1);
  }
  init_exp(f, VCALL, luaK_codeABC(fs, OP_CALL, base, nparams+1, 2));
  luaK_fixline(fs, line);
  fs->freereg = base+1; /* call remove function and arguments and leaves
                            (unless changed) one result */
}


/*
** {======================================================================
** Expression parsing
** =======================================================================
*/
static void prefixexp (LexState *ls, expdesc *v) {
  /* prefixexp -> NAME | '(' expr ')' */
  switch (ls->t.token) {
    case '(': {
      int line = ls->linenumber;
      luaX_next(ls);
      expr(ls, v);
      check_match(ls, ')', '(', line);
      luaK_dischargevars(ls->fs, v);
      return;
    }
    case TK_NAME: {
      singlevar(ls, v);
      return;
    }
    default: {
      luaX_syntaxerror(ls, "unexpected symbol");
      return;
    }
  }
}


static void primaryexp (LexState *ls, expdesc *v) {
  /* primaryexp ->
        prefixexp { `.' NAME | `[' exp `]' | `:' NAME funcargs | funcargs } */
  FuncState *fs = ls->fs;
  prefixexp(ls, v);
  for (;;) {
    switch (ls->t.token) {
      case '.': { /* field */
        field(ls, v);
        break;
      }
      case '[': { /* `[' exp1 `]' */
        expdesc key;
        luaK_exp2anyreg(fs, v);
        yindex(ls, &key);
        luaK_indexed(fs, v, &key);
        break;
      }
      case ':': { /* `:' NAME funcargs */
        expdesc key;
        luaX_next(ls);
        checkname(ls, &key);
        luaK_self(fs, v, &key);
        funcargs(ls, v);
        break;
      }
      case '(':
      case TK_STRING:
      case '{': { /* funcargs */
        luaK_exp2nextreg(fs, v);
        funcargs(ls, v);
        break;
      }
      default: {
        return;
      }
    }
  }
}


static void simpleexp (LexState *ls, expdesc *v) {
  /* simpleexp -> NUMBER | STRING | NIL | true | false | ... |
                  constructor | FUNCTION body | primaryexp */
  switch (ls->t.token) {
    case TK_NUMBER: {
      init_exp(v, VKNUM, 0);
      v->u.nval = ls->t.seminfo.r;
      break;
    }
    case TK_STRING: {
      codestring(ls, v, ls->t.seminfo.ts);
      break;
    }
    case TK_NIL: {
      init_exp(v, VNIL, 0);
      break;
    }
    case TK_TRUE: {
      init_exp(v, VTRUE, 0);
      break;
    }
    case TK_FALSE: {
      init_exp(v, VFALSE, 0);
      break;
    }
    case TK_DOTS: { /* vararg */
      FuncState *fs = ls->fs;
      { if (!(fs->f->is_vararg)) luaX_syntaxerror(ls, "cannot use " "'" "..." "'" " outside a vararg function"); };
      fs->f->is_vararg &= ~4; /* don't need 'arg' */
      init_exp(v, VVARARG, luaK_codeABC(fs, OP_VARARG, 0, 1, 0));
      break;
    }
    case '{': { /* constructor */
      constructor(ls, v);
      return;
    }
    case TK_FUNCTION: {
      luaX_next(ls);
      body(ls, v, 0, ls->linenumber);
      return;
    }
    default: {
      primaryexp(ls, v);
      return;
    }
  }
  luaX_next(ls);
}


static UnOpr getunopr (int op) {
  switch (op) {
    case TK_NOT: {
      return OPR_NOT;
    }
    case '-': {
      return OPR_MINUS;
    }
    case '#': {
      return OPR_LEN;
    }
    default: {
      return OPR_NOUNOPR;
    }
  }
}


static BinOpr getbinopr (int op) {
  switch (op) {
    case '+': {
      return OPR_ADD;
    }
    case '-': {
      return OPR_SUB;
    }
    case '*': {
      return OPR_MUL;
    }
    case '/': {
      return OPR_DIV;
    }
    case '%': {
      return OPR_MOD;
    }
    case '^': {
      return OPR_POW;
    }
    case TK_CONCAT: {
      return OPR_CONCAT;
    }
    case TK_NE: {
      return OPR_NE;
    }
    case TK_EQ: {
      return OPR_EQ;
    }
    case '<': {
      return OPR_LT;
    }
    case TK_LE: {
      return OPR_LE;
    }
    case '>': {
      return OPR_GT;
    }
    case TK_GE: {
      return OPR_GE;
    }
    case TK_AND: {
      return OPR_AND;
    }
    case TK_OR: {
      return OPR_OR;
    }
    default: {
      return OPR_NOBINOPR;
    }
  }
}


static const struct {
  lu_byte left; /* left priority for each binary operator */
  lu_byte right; /* right priority */
} priority[] = { /* ORDER OPR */
   {6, 6}, {6, 6}, {7, 7}, {7, 7}, {7, 7}, /* `+' `-' `/' `%' */
   {10, 9}, {5, 4}, /* power and concat (right associative) */
   {3, 3}, {3, 3}, /* equality and inequality */
   {3, 3}, {3, 3}, {3, 3}, {3, 3}, /* order */
   {2, 2}, {1, 1} /* logical (and/or) */
};


/*
** subexpr -> (simpleexp | unop subexpr) { binop subexpr }
** where `binop' is any binary operator with a priority higher than `limit'
*/
static BinOpr subexpr (LexState *ls, expdesc *v, unsigned int limit) {
  BinOpr op;
  UnOpr uop;
  enterlevel(ls);
  uop = getunopr(ls->t.token);
  if (uop != OPR_NOUNOPR) {
    luaX_next(ls);
    subexpr(ls, v, 8);
    luaK_prefix(ls->fs, uop, v);
  }
  else {
    simpleexp(ls, v);
  }
  /* expand while operators have priorities higher than `limit' */
  op = getbinopr(ls->t.token);
  while (op != OPR_NOBINOPR && priority[op].left > limit) {
    expdesc v2;
    BinOpr nextop;
    luaX_next(ls);
    luaK_infix(ls->fs, op, v);
    /* read sub-expression with higher priority */
    nextop = subexpr(ls, &v2, priority[op].right);
    luaK_posfix(ls->fs, op, v, &v2);
    op = nextop;
  }
  ((ls)->L->nCcalls--);
  return op; /* return first untreated operator */
}


static void expr (LexState *ls, expdesc *v) {
  subexpr(ls, v, 0);
}


/* }==================================================================== */
/*
** {======================================================================
** Rules for Statements
** =======================================================================
*/
static int block_follow (int token) {
  switch (token) {
    case TK_ELSE:
    case TK_ELSEIF:
    case TK_END:
    case TK_UNTIL:
    case TK_EOS: {
      return 1;
    }
    default: {
      return 0;
    }
  }
}


static void block (LexState *ls) {
  /* block -> chunk */
  FuncState *fs = ls->fs;
  BlockCnt bl;
  enterblock(fs, &bl, 0);
  chunk(ls);
  ((void)0);
  leaveblock(fs);
}


/*
** structure to chain all variables in the left-hand side of an
** assignment
*/
struct LHS_assign {
  struct LHS_assign *prev;
  expdesc v; /* variable (global, local, upvalue, or indexed) */
};


/*
** check whether, in an assignment to a local variable, the local variable
** is needed in a previous assignment (to a table). If so, save original
** local value in a safe place and use this safe copy in the previous
** assignment.
*/
static void check_conflict (LexState *ls, struct LHS_assign *lh, expdesc *v) {
  FuncState *fs = ls->fs;
  int extra = fs->freereg; /* eventual position to save local variable */
  int conflict = 0;
  for (; lh; lh = lh->prev) {
    if (lh->v.k == VINDEXED) {
      if (lh->v.u.s.info == v->u.s.info) { /* conflict? */
        conflict = 1;
        lh->v.u.s.info = extra; /* previous assignment will use safe copy */
      }
      if (lh->v.u.s.aux == v->u.s.info) { /* conflict? */
        conflict = 1;
        lh->v.u.s.aux = extra; /* previous assignment will use safe copy */
      }
    }
  }
  if (conflict) {
    luaK_codeABC(fs, OP_MOVE, fs->freereg, v->u.s.info, 0); /* make copy */
    luaK_reserveregs(fs, 1);
  }
}


static void assignment (LexState *ls, struct LHS_assign *lh, int nvars) {
  expdesc e;
  { if (!(VLOCAL <= lh->v.k && lh->v.k <= VINDEXED)) luaX_syntaxerror(ls, "syntax error"); };
  if (testnext(ls, ',')) { /* assignment -> `,' primaryexp assignment */
    struct LHS_assign nv;
    nv.prev = lh;
    primaryexp(ls, &nv.v);
    if (nv.v.k == VLOCAL) {
      check_conflict(ls, lh, &nv.v);
    }
    if ((nvars)>(200 - ls->L->nCcalls)) errorlimit(ls->fs,200 - ls->L->nCcalls,"variables in assignment");
    assignment(ls, &nv, nvars+1);
  }
  else { /* assignment -> `=' explist1 */
    int nexps;
    checknext(ls, '=');
    nexps = explist1(ls, &e);
    if (nexps != nvars) {
      adjust_assign(ls, nvars, nexps, &e);
      if (nexps > nvars) {
        ls->fs->freereg -= nexps - nvars; /* remove extra values */
      }
    }
    else {
      luaK_setoneret(ls->fs, &e); /* close last expression */
      luaK_storevar(ls->fs, &lh->v, &e);
      return; /* avoid default */
    }
  }
  init_exp(&e, VNONRELOC, ls->fs->freereg-1); /* default assignment */
  luaK_storevar(ls->fs, &lh->v, &e);
}


static int cond (LexState *ls) {
  /* cond -> exp */
  expdesc v;
  expr(ls, &v); /* read condition */
  if (v.k == VNIL) {
    v.k = VFALSE; /* `falses' are all equal here */
  }
  luaK_goiftrue(ls->fs, &v);
  return v.f;
}


static void breakstat (LexState *ls) {
  FuncState *fs = ls->fs;
  BlockCnt *bl = fs->bl;
  int upval = 0;
  while (bl && !bl->isbreakable) {
    upval |= bl->upval;
    bl = bl->previous;
  }
  if (!bl) {
    luaX_syntaxerror(ls, "no loop to break");
  }
  if (upval) {
    luaK_codeABC(fs, OP_CLOSE, bl->nactvar, 0, 0);
  }
  luaK_concat(fs, &bl->breaklist, luaK_jump(fs));
}


static void whilestat (LexState *ls, int line) {
  /* whilestat -> WHILE cond DO block END */
  FuncState *fs = ls->fs;
  int whileinit;
  int condexit;
  BlockCnt bl;
  luaX_next(ls); /* skip WHILE */
  whileinit = luaK_getlabel(fs);
  condexit = cond(ls);
  enterblock(fs, &bl, 1);
  checknext(ls, TK_DO);
  block(ls);
  luaK_patchlist(fs, luaK_jump(fs), whileinit);
  check_match(ls, TK_END, TK_WHILE, line);
  leaveblock(fs);
  luaK_patchtohere(fs, condexit); /* false conditions finish the loop */
}


static void repeatstat (LexState *ls, int line) {
  /* repeatstat -> REPEAT block UNTIL cond */
  int condexit;
  FuncState *fs = ls->fs;
  int repeat_init = luaK_getlabel(fs);
  BlockCnt bl1, bl2;
  enterblock(fs, &bl1, 1); /* loop block */
  enterblock(fs, &bl2, 0); /* scope block */
  luaX_next(ls); /* skip REPEAT */
  chunk(ls);
  check_match(ls, TK_UNTIL, TK_REPEAT, line);
  condexit = cond(ls); /* read condition (inside scope block) */
  if (!bl2.upval) { /* no upvalues? */
    leaveblock(fs); /* finish scope */
    luaK_patchlist(ls->fs, condexit, repeat_init); /* close the loop */
  }
  else { /* complete semantics when there are upvalues */
    breakstat(ls); /* if condition then break */
    luaK_patchtohere(ls->fs, condexit); /* else... */
    leaveblock(fs); /* finish scope... */
    luaK_patchlist(ls->fs, luaK_jump(fs), repeat_init); /* and repeat */
  }
  leaveblock(fs); /* finish loop */
}


static int exp1 (LexState *ls) {
  expdesc e;
  int k;
  expr(ls, &e);
  k = e.k;
  luaK_exp2nextreg(ls->fs, &e);
  return k;
}


static void forbody (LexState *ls, int base, int line, int nvars, int isnum) {
  /* forbody -> DO block */
  BlockCnt bl;
  FuncState *fs = ls->fs;
  int prep, endfor;
  adjustlocalvars(ls, 3); /* control variables */
  checknext(ls, TK_DO);
  prep = isnum ? luaK_codeABx(fs,OP_FORPREP,base,((-1))+(((1<<(9 + 9))-1)>>1)) : luaK_jump(fs);
  enterblock(fs, &bl, 0); /* scope for declared variables */
  adjustlocalvars(ls, nvars);
  luaK_reserveregs(fs, nvars);
  block(ls);
  leaveblock(fs); /* end of scope for declared variables */
  luaK_patchtohere(fs, prep);
  endfor = (isnum) ? luaK_codeABx(fs,OP_FORLOOP,base,((-1))+(((1<<(9 + 9))-1)>>1)) :
                     luaK_codeABC(fs, OP_TFORLOOP, base, 0, nvars);
  luaK_fixline(fs, line); /* pretend that `OP_FOR' starts the loop */
  luaK_patchlist(fs, (isnum ? endfor : luaK_jump(fs)), prep + 1);
}


static void fornum (LexState *ls, TString *varname, int line) {
  /* fornum -> NAME = exp1,exp1[,exp1] forbody */
  FuncState *fs = ls->fs;
  int base = fs->freereg;
  new_localvar(ls, luaX_newstring(ls, "" "(for index)", (sizeof("(for index)")/sizeof(char))-1), 0);
  new_localvar(ls, luaX_newstring(ls, "" "(for limit)", (sizeof("(for limit)")/sizeof(char))-1), 1);
  new_localvar(ls, luaX_newstring(ls, "" "(for step)", (sizeof("(for step)")/sizeof(char))-1), 2);
  new_localvar(ls, varname, 3);
  checknext(ls, '=');
  exp1(ls); /* initial value */
  checknext(ls, ',');
  exp1(ls); /* limit */
  if (testnext(ls, ',')) {
    exp1(ls); /* optional step */
  } else { /* default step = 1 */
    luaK_codeABx(fs, OP_LOADK, fs->freereg, luaK_numberK(fs, 1));
    luaK_reserveregs(fs, 1);
  }
  forbody(ls, base, line, 1, 1);
}


static void forlist (LexState *ls, TString *indexname) {
  /* forlist -> NAME {,NAME} IN explist1 forbody */
  FuncState *fs = ls->fs;
  expdesc e;
  int nvars = 0;
  int line;
  int base = fs->freereg;
  /* create control variables */
  new_localvar(ls, luaX_newstring(ls, "" "(for generator)", (sizeof("(for generator)")/sizeof(char))-1), nvars++);
  new_localvar(ls, luaX_newstring(ls, "" "(for state)", (sizeof("(for state)")/sizeof(char))-1), nvars++);
  new_localvar(ls, luaX_newstring(ls, "" "(for control)", (sizeof("(for control)")/sizeof(char))-1), nvars++);
  /* create declared variables */
  new_localvar(ls, indexname, nvars++);
  while (testnext(ls, ',')) {
    new_localvar(ls, str_checkname(ls), nvars++);
  }
  checknext(ls, TK_IN);
  line = ls->linenumber;
  adjust_assign(ls, 3, explist1(ls, &e), &e);
  luaK_checkstack(fs, 3); /* extra space to call generator */
  forbody(ls, base, line, nvars - 3, 0);
}


static void forstat (LexState *ls, int line) {
  /* forstat -> FOR (fornum | forlist) END */
  FuncState *fs = ls->fs;
  TString *varname;
  BlockCnt bl;
  enterblock(fs, &bl, 1); /* scope for loop and control variables */
  luaX_next(ls); /* skip `for' */
  varname = str_checkname(ls); /* first variable name */
  switch (ls->t.token) {
    case '=': {
      fornum(ls, varname, line);
      break;
    }
    case ',':
    case TK_IN: {
      forlist(ls, varname);
      break;
    }
    default: {
      luaX_syntaxerror(ls, "'" "=" "'" " or " "'" "in" "'" " expected");
    }
  }
  check_match(ls, TK_END, TK_FOR, line);
  leaveblock(fs); /* loop scope (`break' jumps to this point) */
}


static int test_then_block (LexState *ls) {
  /* test_then_block -> [IF | ELSEIF] cond THEN block */
  int condexit;
  luaX_next(ls); /* skip IF or ELSEIF */
  condexit = cond(ls);
  checknext(ls, TK_THEN);
  block(ls); /* `then' part */
  return condexit;
}


static void ifstat (LexState *ls, int line) {
  /* ifstat -> IF cond THEN block {ELSEIF cond THEN block} [ELSE block] END */
  FuncState *fs = ls->fs;
  int flist;
  int escapelist = (-1);
  flist = test_then_block(ls); /* IF cond THEN block */
  while (ls->t.token == TK_ELSEIF) {
    luaK_concat(fs, &escapelist, luaK_jump(fs));
    luaK_patchtohere(fs, flist);
    flist = test_then_block(ls); /* ELSEIF cond THEN block */
  }
  if (ls->t.token == TK_ELSE) {
    luaK_concat(fs, &escapelist, luaK_jump(fs));
    luaK_patchtohere(fs, flist);
    luaX_next(ls); /* skip ELSE (after patch, for correct line info) */
    block(ls); /* `else' part */
  }
  else {
    luaK_concat(fs, &escapelist, flist);
  }
  luaK_patchtohere(fs, escapelist);
  check_match(ls, TK_END, TK_IF, line);
}


static void localfunc (LexState *ls) {
  expdesc v, b;
  FuncState *fs = ls->fs;
  new_localvar(ls, str_checkname(ls), 0);
  init_exp(&v, VLOCAL, fs->freereg);
  luaK_reserveregs(fs, 1);
  adjustlocalvars(ls, 1);
  body(ls, &b, 0, ls->linenumber);
  luaK_storevar(fs, &v, &b);
  /* debug information will only see the variable after this point! */
  ((fs)->f->locvars[(fs)->actvar[fs->nactvar - 1]]).startpc = fs->pc;
}


static void localstat (LexState *ls) {
  /* stat -> LOCAL NAME {`,' NAME} [`=' explist1] */
  int nvars = 0;
  int nexps;
  expdesc e;
  do {
    new_localvar(ls, str_checkname(ls), nvars++);
  } while (testnext(ls, ','));
  if (testnext(ls, '=')) {
    nexps = explist1(ls, &e);
  } else {
    e.k = VVOID;
    nexps = 0;
  }
  adjust_assign(ls, nvars, nexps, &e);
  adjustlocalvars(ls, nvars);
}


static int funcname (LexState *ls, expdesc *v) {
  /* funcname -> NAME {field} [`:' NAME] */
  int needself = 0;
  singlevar(ls, v);
  while (ls->t.token == '.') {
    field(ls, v);
  }
  if (ls->t.token == ':') {
    needself = 1;
    field(ls, v);
  }
  return needself;
}


static void funcstat (LexState *ls, int line) {
  /* funcstat -> FUNCTION funcname body */
  int needself;
  expdesc v, b;
  luaX_next(ls); /* skip FUNCTION */
  needself = funcname(ls, &v);
  body(ls, &b, needself, line);
  luaK_storevar(ls->fs, &v, &b);
  luaK_fixline(ls->fs, line); /* definition `happens' in the first line */
}


static void exprstat (LexState *ls) {
  /* stat -> func | assignment */
  FuncState *fs = ls->fs;
  struct LHS_assign v;
  primaryexp(ls, &v.v);
  if (v.v.k == VCALL) { /* stat -> func */
    ((((fs)->f->code[(&v.v)->u.s.info])) = (((((fs)->f->code[(&v.v)->u.s.info]))&(~((~((~(Instruction)0)<<9))<<((0 + 6) + 8)))) | ((((Instruction)(1))<<((0 + 6) + 8))&((~((~(Instruction)0)<<9))<<((0 + 6) + 8))))); /* call statement uses no results */
  } else { /* stat -> assignment */
    v.prev = ((void *)0);
    assignment(ls, &v, 1);
  }
}


static void retstat (LexState *ls) {
  /* stat -> RETURN explist */
  FuncState *fs = ls->fs;
  expdesc e;
  int first, nret; /* registers with returned values */
  luaX_next(ls); /* skip RETURN */
  if (block_follow(ls->t.token) || ls->t.token == ';') {
    first = nret = 0; /* return no values */
  } else {
    nret = explist1(ls, &e); /* optional return values */
    if (((e.k) == VCALL || (e.k) == VVARARG)) {
      luaK_setreturns(fs, &e, (-1));
      if (e.k == VCALL && nret == 1) { /* tail call? */
        ((((fs)->f->code[(&e)->u.s.info])) = (((((fs)->f->code[(&e)->u.s.info]))&(~((~((~(Instruction)0)<<6))<<0))) | ((((Instruction)(OP_TAILCALL))<<0)&((~((~(Instruction)0)<<6))<<0))));
        ((void)0);
      }
      first = fs->nactvar;
      nret = (-1); /* return all values */
    }
    else {
      if (nret == 1) { /* only one single value? */
        first = luaK_exp2anyreg(fs, &e);
      } else {
        luaK_exp2nextreg(fs, &e); /* values must go to the `stack' */
        first = fs->nactvar; /* return all `active' values */
        ((void)0);
      }
    }
  }
  luaK_ret(fs, first, nret);
}


static int statement (LexState *ls) {
  int line = ls->linenumber; /* may be needed for error messages */
  switch (ls->t.token) {
    case TK_IF: { /* stat -> ifstat */
      ifstat(ls, line);
      return 0;
    }
    case TK_WHILE: { /* stat -> whilestat */
      whilestat(ls, line);
      return 0;
    }
    case TK_DO: { /* stat -> DO block END */
      luaX_next(ls); /* skip DO */
      block(ls);
      check_match(ls, TK_END, TK_DO, line);
      return 0;
    }
    case TK_FOR: { /* stat -> forstat */
      forstat(ls, line);
      return 0;
    }
    case TK_REPEAT: { /* stat -> repeatstat */
      repeatstat(ls, line);
      return 0;
    }
    case TK_FUNCTION: {
      funcstat(ls, line); /* stat -> funcstat */
      return 0;
    }
    case TK_LOCAL: { /* stat -> localstat */
      luaX_next(ls); /* skip LOCAL */
      if (testnext(ls, TK_FUNCTION)) { /* local function? */
        localfunc(ls);
      } else {
        localstat(ls);
      }
      return 0;
    }
    case TK_RETURN: { /* stat -> retstat */
      retstat(ls);
      return 1; /* must be last statement */
    }
    case TK_BREAK: { /* stat -> breakstat */
      luaX_next(ls); /* skip BREAK */
      breakstat(ls);
      return 1; /* must be last statement */
    }
    default: {
      exprstat(ls);
      return 0; /* to avoid warnings */
    }
  }
}


static void chunk (LexState *ls) {
  /* chunk -> { stat [`;'] } */
  int islast = 0;
  enterlevel(ls);
  while (!islast && !block_follow(ls->t.token)) {
    islast = statement(ls);
    testnext(ls, ';');
    ((void)0);
    ls->fs->freereg = ls->fs->nactvar; /* free registers */
  }
  ((ls)->L->nCcalls--);
}

/* }====================================================================== */
