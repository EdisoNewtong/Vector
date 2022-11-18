/*
** $Id: ltm.h,v 2.6.1.1 2007/12/27 13:02:25 roberto Exp $
** Tag methods
** See Copyright Notice in lua.h
*/

#ifndef ltm_h
#define ltm_h


#include "lobject.h"


/*
* WARNING: if you change the order of this enumeration,
* grep "ORDER TM"
*/
typedef enum {
/*  0 */  TM_INDEX,
/*  1 */  TM_NEWINDEX,
/*  2 */  TM_GC,
/*  3 */  TM_MODE,
/*  4 */  TM_EQ,  /* last tag method with `fast' access */
/*  5 */  TM_ADD,
/*  6 */  TM_SUB,
/*  7 */  TM_MUL,
/*  8 */  TM_DIV,
/*  9 */  TM_MOD,
/* 10 */  TM_POW,
/* 11 */  TM_UNM,
/* 12 */  TM_LEN,
/* 13 */  TM_LT,
/* 14 */  TM_LE,
/* 15 */  TM_CONCAT,
/* 16 */  TM_CALL,
/* 17 */  TM_N		/* number of elements in the enum */
} TMS;



#define gfasttm(g,et,e) ((et) == NULL ? NULL : \
  ((et)->flags & (1u<<(e))) ? NULL : luaT_gettm(et, e, (g)->tmname[e]))

#define fasttm(l,et,e)	gfasttm(G(l), et, e)

LUAI_DATA const char *const luaT_typenames[];


LUAI_FUNC const TValue *luaT_gettm (Table *events, TMS event, TString *ename);
LUAI_FUNC const TValue *luaT_gettmbyobj (lua_State *L, const TValue *o,
                                                       TMS event);
LUAI_FUNC void luaT_init (lua_State *L);

#endif
