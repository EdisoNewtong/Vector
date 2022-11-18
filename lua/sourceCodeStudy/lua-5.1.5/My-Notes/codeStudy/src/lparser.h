/*
** $Id: lparser.h,v 1.57.1.1 2007/12/27 13:02:25 roberto Exp $
** Lua Parser
** See Copyright Notice in lua.h
*/

#ifndef lparser_h
#define lparser_h

#include "llimits.h"
#include "lobject.h"
#include "lzio.h"


/*
** Expression descriptor
*/

typedef enum {
/*  0 */  VVOID,	/* no value */
/*  1 */  VNIL,
/*  2 */  VTRUE,
/*  3 */  VFALSE,
/*  4 */  VK,		/* info = index of constant in `k' */
/*  5 */  VKNUM,	/* nval = numerical value */
/*  6 */  VLOCAL,	/* info = local register */
/*  7 */  VUPVAL,       /* info = index of upvalue in `upvalues' */
/*  8 */  VGLOBAL,	/* info = index of table; aux = index of global name in `k' */
/*  9 */  VINDEXED,	/* info = table register; aux = index register (or `k') */
/* 10 */  VJMP,		/* info = instruction pc */
/* 11 */  VRELOCABLE,	/* info = instruction pc */
/* 12 */  VNONRELOC,	/* info = result register */
/* 13 */  VCALL,	/* info = instruction pc */
/* 14 */  VVARARG	/* info = instruction pc */
} expkind;

typedef struct expdesc {
  expkind k;
  union {
    struct { int info, aux; } s;
    lua_Number nval;
  } u;
  int t;  /* patch list of `exit when true' */
  int f;  /* patch list of `exit when false' */
} expdesc;


typedef struct upvaldesc {
  lu_byte k;
  lu_byte info;
} upvaldesc;


struct BlockCnt;  /* defined in lparser.c */


/* state needed to generate code for a given function */
typedef struct FuncState {
  Proto *f;  /* current function header */
  Table *h;  /* table to find (and reuse) elements in `k' */
  struct FuncState *prev;  /* enclosing function */
  struct LexState *ls;  /* lexical state */
  struct lua_State *L;  /* copy of the Lua state */
  struct BlockCnt *bl;  /* chain of current blocks */
  int pc;  /* next position to code (equivalent to `ncode') */
  int lasttarget;   /* `pc' of last `jump target' */
  int jpc;  /* list of pending jumps to `pc' */
  int freereg;  /* first free register */
  int nk;  /* number of elements in `k' */
  int np;  /* number of elements in `p' */
  short nlocvars;  /* number of elements in `locvars' */
  lu_byte nactvar;  /* number of active local variables */
  upvaldesc upvalues[LUAI_MAXUPVALUES];  /* upvalues */
  unsigned short actvar[LUAI_MAXVARS];  /* declared-variable stack */
} FuncState;


LUAI_FUNC Proto *luaY_parser (lua_State *L, ZIO *z, Mbuffer *buff,
                                            const char *name);


#endif
