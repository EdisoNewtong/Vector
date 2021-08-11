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
  /*                               NO_JUMP = (-1)           */
  return (e->k == VKNUM && e->t == NO_JUMP && e->f == NO_JUMP);
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

	  /*
	  	  typedef lu_int32 Instruction;

	  	  lu_int32 left_Operand  = (*previous) >> 0;
	  	  lu_int32 right_Operand = ((~((~(Instruction)0)<<6))<<0);
	  	  (OpCode)( left_Operand &  right_Operand )
	  */
	  	  
	  /*
	   
             typedef enum { ... } OpCode;
	         GET_OPCODE(*previous)
	  	  ( (OpCode)( ( (*previous) >> 0 )   & ((~((~(Instruction)0)<<6))<<0) ) ) 

	  */
      if ( ( (OpCode)( ( (*previous) >> 0 )   & ((~((~(Instruction)0)<<6))<<0) ) )  == OP_LOADNIL ) {

		/*

			   #define POS_OP		0
			   #define SIZE_OP		6

			   #define POS_A		(POS_OP + SIZE_OP)

			   #define SIZE_A		8
			   
			   #define MASK1(n,p)	( (~((~(Instruction)0)<<n))<<p )

			   #define GETARG_A(i)	(cast(int, ((i)>>POS_A) & MASK1(SIZE_A,0)))
				(int)(  ( (*previous) >> 6)   &  (  (~((~(Instruction)0)<<8))<<0  ) )


        int pfrom = GETARG_A(*previous);
		*/
        int pfrom = (int)(  ( (*previous) >> 6)   &  (  (~((~(Instruction)0)<<8))<<0  ) );

		/*

			#define GETARG_B(i)	(cast(int, ((i)>>POS_B) & MASK1(SIZE_B,0)))

			#define SIZE_B		9

			#define SIZE_C		9

			#define POS_C		(POS_A + SIZE_A)   // (6 + 8) = (14)
			#define POS_B		(POS_C + SIZE_C)   // (14+9)  = 23

			(int)(  ( (*previous) >> 23)   & (  (~((~(Instruction)0)<< 9 ))<<0  ) )

        int pto = GETARG_B(*previous);
		*/
        int pto = (int)(  ( (*previous) >> 23)   & (  (~((~(Instruction)0)<< 9 ))<<0  ) );

        if (pfrom <= from && from <= pto+1) { /* can connect both? */
          if (from+n-1 > pto) {
			/*

			#define GETARG_B(i)	(cast(int, ((i)>>POS_B) & MASK1(SIZE_B,0)))

			#define SIZE_B		9

			#define SIZE_C		9

			#define POS_C		(POS_A + SIZE_A)   // (6 + 8) = (14)
			#define POS_B		(POS_C + SIZE_C)   // (14+9)  = 23

			(int)(  ( (*previous) >> 23)   & (  (~((~(Instruction)0)<< 9 ))<<0  ) )

			#define SETARG_B(i,b)	((i) = (((i)&MASK0(SIZE_B,POS_B)) | \
				((cast(Instruction, b)<<POS_B)&MASK1(SIZE_B,POS_B))))

			#define MASK0(n,p)	(~MASK1(n,p))

			#define MASK1(n,p)	((~((~(Instruction)0)<<n))<<p)

				 ( (*previous) = ( (*previous)             &  ( ~((~((~(Instruction)0)<<9))<<23) ) ) ) 
			|    ( ( ((Instruction)(from+n-1)) << 23 )     &  (  ((~((~(Instruction)0)<<9))<<23) ) );


            ((*previous) = (((*previous)&(~((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))) | ((((Instruction)(from+n-1))<<(((0 + 6) + 8) + 9))&((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))));
            SETARG_B(*previous, from+n-1);
			*/
				 (  (*previous) = 
				  ( (*previous) &  ( ~((~((~(Instruction)0)<<9))<<23) ) ) 
			|     ( ( ((Instruction)(from+n-1)) << 23 )   &  (  ((~((~(Instruction)0)<<9))<<23) ) ) );
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
  /*      NO_JUMP  (-1)*/
  fs->jpc = NO_JUMP;

  /*
  	#define luaK_codeAsBx(fs,o,A,sBx)	luaK_codeABx(fs,o,A,(sBx)+MAXARG_sBx)
  
  	#define SIZE_C		9
  	#define SIZE_B		9
  	#define SIZE_Bx		(SIZE_C + SIZE_B)
  
    #define MAXARG_Bx        ((1<<SIZE_Bx)-1)
  
    #define MAXARG_sBx        (MAXARG_Bx>>1)         
  
  	luaK_codeABx( fs, OP_JMP, 0, (NO_JUMP) + ( ( (1<<18) -1 ) >> 1 ) );
  
  j = luaK_codeAsBx(fs, OP_JMP, 0, NO_JUMP);
    = luaK_codeABx( fs, OP_JMP, 0, (NO_JUMP) + ( ( (1<<18) -1 ) >> 1 ) );
  */
  j = luaK_codeABx( fs, OP_JMP, 0, (NO_JUMP) + ( ( (1<<18) -1 ) >> 1 ) );
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
  ((void)0); /* lua_assert(dest != NO_JUMP); */
  /* MAXARG_sBx   ( ( (1<<18) -1 ) >> 1 ) */
  if (abs(offset) > MAXARG_sBx ) {
    luaX_syntaxerror(fs->ls, "control structure too long");
  }


  /*
  
    #define SETARG_Bx(i,b)	((i) = (((i)&MASK0(SIZE_Bx,POS_Bx)) | \
		    ((cast(Instruction, b)<<POS_Bx)&MASK1(SIZE_Bx,POS_Bx))))


	#define SIZE_A		8

	#define SIZE_OP		6

	#define POS_OP		0
	#define POS_A		(POS_OP + SIZE_OP) // 0 + 6 = 6
	#define POS_C		(POS_A + SIZE_A)   // 6 + 8 = 14
	#define POS_Bx		POS_C              // 14

	#define SETARG_sBx(i,b)	SETARG_Bx((i),cast(unsigned int, (b)+MAXARG_sBx))

	(*jmp) = (     ((*jmp)                                                                            & (  ~((~((~(Instruction)0)<<18))<<14) )  ) 
	                 | (( ( (Instruction)( (unsigned int)( (offset) + ( ( (1<<18) -1 ) >> 1 )) ) ) << 14 )  & (   ((~((~(Instruction)0)<<18))<<14) )  )
				   );

  SETARG_sBx(*jmp, offset);
  (((*jmp)) = ((((*jmp))&(~((~((~(Instruction)0)<<(9 + 9)))<<((0 + 6) + 8)))) | ((((Instruction)(((unsigned int)((offset)+(((1<<(9 + 9))-1)>>1)))))<<((0 + 6) + 8))&((~((~(Instruction)0)<<(9 + 9)))<<((0 + 6) + 8)))));
  */
  (*jmp) = (    ((*jmp)                                                                              & (  ~((~((~(Instruction)0)<<18))<<14) )  ) 
	          | (( ( (Instruction)( (unsigned int)( (offset) + ( ( (1<<18) -1 ) >> 1 )) ) ) << 14 )  & (   ((~((~(Instruction)0)<<18))<<14) )  )
		   );
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
/*
	#define SIZE_Bx		(SIZE_C + SIZE_B) // 9 + 9 = 18

	#define POS_Bx		POS_C              // 14

    #define MAXARG_sBx        (MAXARG_Bx>>1)         //   ( ( (1<<18) -1 ) >> 1 )

	#define GETARG_Bx(i)	(cast(int, ((i)>>POS_Bx) & MASK1(SIZE_Bx,0)))

	#define GETARG_sBx(i)	(GETARG_Bx(i)-MAXARG_sBx)

	(int)( ( (fs->f->code[pc]) >> 14 ) & ((~((~(Instruction)0)<<18))<<0) )  -  ( ( (1<<18) -1 ) >> 1 )


  int offset = GETARG_sBx(fs->f->code[pc]);
  int offset = ((((int)(((fs->f->code[pc])>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))-(((1<<(9 + 9))-1)>>1));
*/

  int offset = (int)( ( (fs->f->code[pc]) >> 14 ) & ((~((~(Instruction)0)<<18))<<0) )  -  ( ( (1<<18) -1 ) >> 1 );
  /*           NO_JUMP (-1) */
  if (offset == NO_JUMP ) { /* point to itself represents end of list */
    return NO_JUMP; /* end of list */
  }
  else {
    return (pc+1)+offset; /* turn offset into absolute position */
  }
}


static Instruction *getjumpcontrol (FuncState *fs, int pc) {
  Instruction *pi = &fs->f->code[pc];
  /*
  	  #define testTMode(m)    (luaP_opmodes[m] & (1 << 7))
  
  	  (   luaP_opmodes[ ( (OpCode)( ( (*(pi-1)) >> 0 )   & ((~((~(Instruction)0)<<6))<<0) ) ) ] 
  	    & (1<<7) 
  	  )
  */

  /*              testTMode(GET_OPCODE(*(pi-1)))   */
  if ( pc >= 1 &&
      (   luaP_opmodes[ ( (OpCode)( ( (*(pi-1)) >> 0 )   & ((~((~(Instruction)0)<<6))<<0) ) ) ] 
  	    & (1<<7) 
  	  )
  ) {
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
  /*             NO_JUMP (-1)                      */
  for (; list != NO_JUMP; list = getjump(fs, list)) {
    Instruction i = *getjumpcontrol(fs, list);

    /*
          typedef lu_int32 Instruction;
    
          lu_int32 left_Operand  = (i) >> 0;
          lu_int32 right_Operand = ((~((~(Instruction)0)<<6))<<0);
    	  (OpCode)( left_Operand &  right_Operand )
    
    */
	/*    GET_OPCODE(i)                                                  != OP_TESTSET ) */
    if ( ( (OpCode)( ( (i) >> 0 )   & ((~((~(Instruction)0)<<6))<<0) ) ) != OP_TESTSET ) {
      return 1;
    }
  }
  return 0; /* not found */
}


static int patchtestreg (FuncState *fs, int node, int reg) {
  Instruction *i = getjumpcontrol(fs, node);

  /*
        typedef lu_int32 Instruction;
  
        lu_int32 left_Operand  = (*i) >> 0;
        lu_int32 right_Operand = ((~((~(Instruction)0)<<6))<<0);
  	  (OpCode)( left_Operand &  right_Operand )
  */
  /*     GET_OPCODE(*i)                                                 != OP_TESTSET) */
  if ( ( (OpCode)( ( (*i) >> 0 )   & ((~((~(Instruction)0)<<6))<<0) ) ) != OP_TESTSET) {
    return 0; /* cannot patch other instructions */
  }

  /*
           GETARG_B(*i)
        => (int)(  ( (*i) >> 23)   & (  (~((~(Instruction)0)<< 9 ))<<0  ) )

		NO_REG       ( (1<<8) - 1 )
  */
  if (reg != NO_REG   && reg != ( (int)(  ( (*i) >> 23)   & (  (~((~(Instruction)0)<< 9 ))<<0  ) ) ) ) {

	/*

	#define SETARG_A(i,u)	((i) = (((i)&MASK0(SIZE_A,POS_A)) | \
			((cast(Instruction, u)<<POS_A)&MASK1(SIZE_A,POS_A))))

	*/

	/*


    SETARG_A(*i, reg);
			 (*i) = (    (*i) = ( (*i) & (~((~((~(Instruction)0)<<8))<<6)) ) 
							|  ( ((reg) << 6 )      &   ((~((~(Instruction)0)<<8))<<6)  )
			 );

      ((*i) = (((*i)&(~((~((~(Instruction)0)<<8))<<(0 + 6)))) | ((((Instruction)(reg))<<(0 + 6))&((~((~(Instruction)0)<<8))<<(0 + 6)))));
	*/
	(*i) =   (     (*i)               & (~((~((~(Instruction)0)<<8))<<6)) )
		       | ( ((reg) << 6 )      &   ((~((~(Instruction)0)<<8))<<6)  );
  }
  else { /* no register to put value or register already has the value */
    /*
    

       *i = CREATE_ABC(OP_TEST, GETARG_B(*i), 0, GETARG_C(*i));
       *i = ((((Instruction)(OP_TEST))<<0) | (((Instruction)((((int)(((*i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))))))<<(0 + 6)) | (((Instruction)(0))<<(((0 + 6) + 8) + 9)) | (((Instruction)((((int)(((*i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))))))<<((0 + 6) + 8)));
    */

	*i = (  
		     ( (Instruction)( (OP_TEST) << 0  ) )
	      |  ( (Instruction)( ( (int)(  ( (*i) >> 23)   & (  (~((~(Instruction)0)<< 9 ))<<0  ) ) ) << 6  ) )
	      |  ( (Instruction)( (0) << 23 ) )
	      |  ( (Instruction)( ( ( (int)( ((*i) >> 14) & ((~((~(Instruction)0)<<9))<<0) ) ) ) << 14 ) )
       );
  }
  return 1;
}


static void removevalues (FuncState *fs, int list) {
  /*             NO_JUMP  (-1)                    */
  for (; list != NO_JUMP; list = getjump(fs, list)) {
    /*                      NO_REG  ((1<<8)-1)  */
    patchtestreg(fs, list, ((1<<8)-1));
  }
}


static void patchlistaux (FuncState *fs, int list, int vtarget, int reg,
                          int dtarget) {
  /*            NO_JUMP  (-1)  */
  while (list != NO_JUMP ) {
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
  /*                                 NO_REG   ((1<<8)-1) */
  patchlistaux(fs, fs->jpc, fs->pc, ((1<<8)-1), fs->pc);
  fs->jpc = NO_JUMP;/* NO_JUMP (-1); */
}


void luaK_patchlist (FuncState *fs, int list, int target) {
  if (target == fs->pc) {
    luaK_patchtohere(fs, list);
  } else {
    ((void)0); /* lua_assert(target < fs->pc); */
	/*                             NO_REG   ((1<<8)-1) */
    patchlistaux(fs, list, target, ((1<<8)-1), target);
  }
}


void luaK_patchtohere (FuncState *fs, int list) {
  luaK_getlabel(fs);
  luaK_concat(fs, &fs->jpc, list);
}


void luaK_concat (FuncState *fs, int *l1, int l2) {
  if (l2 == NO_JUMP ) { /*  NO_JUMP (-1) */
    return;
  } else if (*l1 == NO_JUMP) { /*  NO_JUMP (-1) */
    *l1 = l2;
  } else {
    int list = *l1;
    int next;
	/*                                 NO_JUMP (-1)   */
    while ((next = getjump(fs, list)) != NO_JUMP ) { /* find last element */
      list = next;
    }
    fixjump(fs, list, l2);
  }
}


void luaK_checkstack (FuncState *fs, int n) {
  int newstack = fs->freereg + n;
  if (newstack > fs->f->maxstacksize) {
    if (newstack >= MAXSTACK ) { /* MAXSTACK 250 */
      luaX_syntaxerror(fs->ls, "function or expression too complex");
    }
    fs->f->maxstacksize = cast_byte(newstack); /* ((lu_byte)((newstack))); */
  }
}


void luaK_reserveregs (FuncState *fs, int n) {
  luaK_checkstack(fs, n);
  fs->freereg += n;
}


static void freereg (FuncState *fs, int reg) {
  /* 
       ISK(reg)
     ( (reg) & (1 << 8) )  
  */
  if (!( (reg) & (1 << 8) ) && reg >= fs->nactvar) {
    fs->freereg--;
    ((void)0); /* lua_assert(reg == fs->freereg); */
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
  if ( ttisnumber(idx) ) {
    ((void)0); /* lua_assert(luaO_rawequalObj(&fs->f->k[cast_int(nvalue(idx))], v)); */
    /* return ((int)((((idx)->value.n)))); */
    return cast_int( nvalue(idx) );
  }
  else { /* constant not found; create a new entry */
    /* { TValue *i_o=(idx); i_o->value.n=(((lua_Number)((fs->nk)))); i_o->tt=LUA_TNUMBER; }; */
    setnvalue(idx, cast_num(fs->nk));

	/*
    luaM_growvector(L, f->k, fs->nk, f->sizek, TValue,
                    MAXARG_Bx, "constant table overflow");
	*/
    if ( (fs->nk)+1 > (f->sizek)) { 
		/*                                                                     MAXARG_Bx ((1<<18)-1)                */
		(f->k) = (TValue *)luaM_growaux_(L,f->k,&(f->sizek),sizeof(TValue),((1<<18)-1),"constant table overflow");
	}
    while (oldsize < f->sizek) {
      /* ((&f->k[oldsize++])->tt=0); */
      setnilvalue(&f->k[oldsize++]);
    }

    /* { const TValue *o2=(v); TValue *o1=(&f->k[fs->nk]); o1->value = o2->value; o1->tt=o2->tt; ((void)0); }; */
    setobj(L, &f->k[fs->nk], v);

    /* luaC_barrier(L, f, v); */
    /* { if (((((v)->tt) >= LUA_TSTRING) && (((((v)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) && ((((((GCObject *)((f)))))->gch.marked) & ((1<<(2))))) luaC_barrierf(L,(((GCObject *)((f)))),((v)->value.gc)); }; */
    { 
		if ( ( ( v->tt >= LUA_TSTRING ) && ( ((( v->value.gc))->gch.marked ) &  ( ( 1<< WHITE0BIT ) | (1 << WHITE1BIT) ) ) ) && ( ( ( (GCObject *)(f) )->gch.marked ) & ( ( 1 << BLACKBIT ) ) ) ) { 
			luaC_barrierf(L, ( (GCObject *)(f) ),  v->value.gc  ); 
		} 
	};
    return fs->nk++;
  }
}


int luaK_stringK (FuncState *fs, TString *s) {
  TValue o;
  /* { TValue *i_o=(&o); i_o->value.gc=((GCObject *)((s))); i_o->tt = LUA_TSTRING; ((void)0); }; */
  setsvalue(fs->L, &o, s);
  return addk(fs, &o, &o);
}


int luaK_numberK (FuncState *fs, lua_Number r) {
  TValue o;
  /* { TValue *i_o=(&o); i_o->value.n=(r); i_o->tt = LUA_TNUMBER; }; */
  setnvalue(&o, r);
  return addk(fs, &o, &o);
}


static int boolK (FuncState *fs, int b) {
  TValue o;
  /* { TValue *i_o=(&o); i_o->value.b=(b); i_o->tt = LUA_TBOOLEAN; }; */
  setbvalue(&o, b);
  return addk(fs, &o, &o);
}


static int nilK (FuncState *fs) {
  TValue k, v;
  /* ((&v)->tt= LUA_TNIL); */
  setnilvalue(&v);
  /* cannot use nil as key; instead use table itself to represent nil */

  /* { TValue *i_o=(&k); i_o->value.gc=((GCObject *)((fs->h))); i_o->tt = LUA_TTABLE; ((void)0); }; */
  sethvalue(fs->L, &k, fs->h);
  return addk(fs, &k, &v);
}


void luaK_setreturns (FuncState *fs, expdesc *e, int nresults) {
  if (e->k == VCALL) { /* expression is an open function call? */
    /* 
	   SETARG_C(getcode(fs, e), nresults+1); 
	   ((((fs)->f->code[(e)->u.s.info])) = (((((fs)->f->code[(e)->u.s.info]))&(~((~((~(Instruction)0)<<9))<<((0 + 6) + 8)))) | ((((Instruction)(nresults+1))<<((0 + 6) + 8))&((~((~(Instruction)0)<<9))<<((0 + 6) + 8)))));
	*/
	 ( (fs)->f->code[ (e)->u.s.info] )  = (    ( ( (fs)->f->code[ (e)->u.s.info] )         & (~( (~((~(Instruction)0)<<9))<<14 )) )
	                                        |  ( ( ( (Instruction)( nresults+1 ) ) << 14 ) &   ( (~((~(Instruction)0)<<9))<<14 ) )
				 );
  }
  else if (e->k == VVARARG) {
    /* 
	   SETARG_B(getcode(fs, e), nresults+1); 
       ((((fs)->f->code[(e)->u.s.info])) = (((((fs)->f->code[(e)->u.s.info]))&(~((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))) | ((((Instruction)(nresults+1))<<(((0 + 6) + 8) + 9))&((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))));
	*/
    (fs)->f->code[ (e)->u.s.info]  = 
	   ( ( (  (fs)->f->code[ (e)->u.s.info] )      &  ( ~((~((~(Instruction)0)<<9))<<23) ) )
	|  ( ( ((Instruction)(nresults+1)) << 23 )   &  (  ((~((~(Instruction)0)<<9))<<23) ) ) );



    /* 
       SETARG_A(getcode(fs, e), fs->freereg);
	   ((((fs)->f->code[(e)->u.s.info])) = (((((fs)->f->code[(e)->u.s.info]))&(~((~((~(Instruction)0)<<8))<<(0 + 6)))) | ((((Instruction)(fs->freereg))<<(0 + 6))&((~((~(Instruction)0)<<8))<<(0 + 6))))); 
	*/

    (fs)->f->code[ (e)->u.s.info] =  
		( ( ( (fs)->f->code[ (e)->u.s.info] ) ) & (~((~((~(Instruction)0)<<8))<<6)) ) 
	  | ( ((fs->freereg) << 6 )                 &   ((~((~(Instruction)0)<<8))<<6)  );

    luaK_reserveregs(fs, 1);
  }
}


void luaK_setoneret (FuncState *fs, expdesc *e) {
  if (e->k == VCALL) { /* expression is an open function call? */
    e->k = VNONRELOC;

    /* 
      e->u.s.info = GETARG_A(getcode(fs, e));
	  e->u.s.info = (((int)(((((fs)->f->code[(e)->u.s.info]))>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0)))); 
	*/
    e->u.s.info = (int)(  (  ( (fs)->f->code[ (e)->u.s.info] ) >> 6)   &  (  (~((~(Instruction)0)<<8))<<0  ) );
  }
  else if (e->k == VVARARG) {
    /*
		SETARG_B(getcode(fs, e), 2);
		((((fs)->f->code[(e)->u.s.info])) = (((((fs)->f->code[(e)->u.s.info]))&(~((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))) | ((((Instruction)(2))<<(((0 + 6) + 8) + 9))&((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))));
	*/
    (fs)->f->code[ (e)->u.s.info] = 
	     ( ( ( (fs)->f->code[ (e)->u.s.info] ) )  &  ( ~((~((~(Instruction)0)<<9))<<23) ) )
      |  ( ( ((Instruction)(2)) << 23 )           &  (  ((~((~(Instruction)0)<<9))<<23) ) );
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
    default: {
	  break; /* there is one value available (somewhere) */
	}
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
      /*
      Instruction *pc = &getcode(fs, e);
      Instruction *pc = &((fs)->f->code[(e)->u.s.info]);

      SETARG_A(*pc, reg);
      ((*pc) = (((*pc)&(~((~((~(Instruction)0)<<8))<<(0 + 6)))) | ((((Instruction)(reg))<<(0 + 6))&((~((~(Instruction)0)<<8))<<(0 + 6)))));
      */
      Instruction *pc = &( (fs)->f->code[ (e)->u.s.info] );

      *pc =   ( (*pc) & (~((~((~(Instruction)0)<<8))<<6)) ) 
		       | ( ((reg) << 6 )      &   ((~((~(Instruction)0)<<8))<<6)  );
      break;
    }
    case VNONRELOC: {
      if (reg != e->u.s.info)
        luaK_codeABC(fs, OP_MOVE, reg, e->u.s.info, 0);
      break;
    }
    default: {
      ((void)0); /* lua_assert(e->k == VVOID || e->k == VJMP); */
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
  /*    hasjumps(e)       */
  if ( e->t != e->f ) {
    int final; /* position after whole expression */
    /*        NO_JUMP (-1) */
    int p_f = NO_JUMP; /* position of an eventual LOAD false */
    /*        NO_JUMP (-1) */
    int p_t = NO_JUMP; /* position of an eventual LOAD true */
    if (need_value(fs, e->t) || need_value(fs, e->f)) {

      /*                        NO_JUMP (-1)           */
      int fj = (e->k == VJMP) ? NO_JUMP : luaK_jump(fs);
      p_f = code_label(fs, reg, 0, 1);
      p_t = code_label(fs, reg, 1, 0);
      luaK_patchtohere(fs, fj);
    }
    final = luaK_getlabel(fs);
    patchlistaux(fs, e->f, final, reg, p_f);
    patchlistaux(fs, e->t, final, reg, p_t);
  }
  /*            NO_JUMP (-1)           */
  e->f = e->t = NO_JUMP;
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
    /*  !hasjumps(e)    */
    if (!(e->t != e->f)) {
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
  /*    hasjumps(e)      */
  if ( e->t != e->f ) {
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
      /*             MAXINDEXRK  ( (1 << 8) - 1) */
      if (fs->nk <= MAXINDEXRK ) { /* constant fit in RK operand? */
        e->u.s.info = (e->k == VNIL) ? nilK(fs) :
                      (e->k == VKNUM) ? luaK_numberK(fs, e->u.nval) :
                                        boolK(fs, (e->k == VTRUE));
        e->k = VK;
        /* return RKASK(e->u.s.info); */
        return ( (e->u.s.info) | (1 << 8) );
      }
      else {
        break;
      }
    }
    case VK: {
      /*                 MAXINDEXRK  ( (1 << 8) - 1) */
      if (e->u.s.info <= MAXINDEXRK ) { /* constant fit in argC? */
        /* return RKASK(e->u.s.info); */
        return ( (e->u.s.info) | (1 << 8) );
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
  switch ( var->k ) {
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
      ((void)0); /* lua_assert(0); */ /* invalid var kind to store */
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
  /*

  lua_assert(testTMode(GET_OPCODE(*pc)) && GET_OPCODE(*pc) != OP_TESTSET &&
                                           GET_OPCODE(*pc) != OP_TEST);
  */
  ((void)0);

  /*
  SETARG_A(*pc, !(GETARG_A(*pc)));
  ((*pc) = (((*pc)&(~((~((~(Instruction)0)<<8))<<(0 + 6)))) | ((((Instruction)(!((((int)(((*pc)>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0)))))))<<(0 + 6))&((~((~(Instruction)0)<<8))<<(0 + 6)))));
  */

  *pc =    ( (*pc) & (~((~((~(Instruction)0)<<8))<<6)) ) 
		 | ( ((!( (int)(  ( (*pc) >> 6)   &  (  (~((~(Instruction)0)<<8))<<0  ) ) )) << 6 )      &   ((~((~(Instruction)0)<<8))<<6)  );
}


static int jumponcond (FuncState *fs, expdesc *e, int cond) {
  if (e->k == VRELOCABLE) {
    /*                 getcode(fs, e)            */
    Instruction ie = ((fs)->f->code[(e)->u.s.info]);
    /*                                        GET_OPCODE(ie)              == OP_NOT */
    if ( ( (OpCode)( ( (ie) >> 0 )   & ((~((~(Instruction)0)<<6))<<0) ) ) == OP_NOT ) {
      fs->pc--; /* remove previous OP_NOT */
      /*                            GETARG_B(ie) */
      return condjump(fs, OP_TEST, ( (int)(  ( (ie) >> 23)   & (  (~((~(Instruction)0)<< 9 ))<<0  ) ) ), 0, !cond);
    }
    /* else go through */
  }
  discharge2anyreg(fs, e);
  freeexp(fs, e);
  /*                              NO_REG  ((1<<8)-1)        */
  return condjump(fs, OP_TESTSET, NO_REG, e->u.s.info, cond);
}


void luaK_goiftrue (FuncState *fs, expdesc *e) {
  int pc; /* pc of last jump */
  luaK_dischargevars(fs, e);
  switch (e->k) {
    case VK:
    case VKNUM:
    case VTRUE: {
      /*   NO_JUMP (-1) */
      pc = NO_JUMP; /* always true; do nothing */
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
  /*     NO_JUMP  (-1) */
  e->t = NO_JUMP;
}


static void luaK_goiffalse (FuncState *fs, expdesc *e) {
  int pc; /* pc of last jump */
  luaK_dischargevars(fs, e);
  switch (e->k) {
    case VNIL:
    case VFALSE: {
      /* NO_JUMP (-1) */
      pc = NO_JUMP; /* always false; do nothing */
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
  /*     NO_JUMP (-1)  */
  e->f = NO_JUMP;
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
      ((void)0); /* lua_assert(0); */ /* cannot happen */
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
      /*= luai_numadd(v1, v2); */
      r = ((v1)+(v2));
      break;
    }
    case OP_SUB: {
      /*= luai_numsub(v1, v2);  */
      r = ((v1)-(v2));
      break;
    }
    case OP_MUL: {
      /*=  luai_nummul(v1, v2);  */
      r = ((v1)*(v2));
      break;
    }
    case OP_DIV: {
      if (v2 == 0) {
        return 0; /* do not attempt to divide by 0 */
      }
      /*= luai_numdiv(v1, v2);  */
      r = ((v1)/(v2));
      break;
    }
    case OP_MOD: {
      if (v2 == 0) {
        return 0; /* do not attempt to divide by 0 */
      }
      /*= luai_nummod(v1, v2);  */
      r = ((v1) - floor((v1)/(v2))*(v2));
      break;
    }
    case OP_POW: {
      /*= luai_numpow(v1, v2);  */
      r = (pow(v1,v2));
      break;
    }
    case OP_UNM: {
      /*= luai_numunm(v1);  */
      r = (-(v1));
      break;
    }
    case OP_LEN: {
      return 0; /* no constant folding for 'len' */
    }
    default: {
      ((void)0); /* lua_assert(0); */
      r = 0;
      break;
    }
  }
  /*   luai_numisnan(r)   */
  if ( (!( r == r ) ) ) {
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
  /*            NO_JUMP (-1)                       */
  e2.t = e2.f = NO_JUMP; e2.k = VKNUM; e2.u.nval = 0;
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
      ((void)0); /* lua_assert(0); */
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
      ((void)0); /* lua_assert(e1->t == NO_JUMP); */ /* list must be closed */
      luaK_dischargevars(fs, e2);
      luaK_concat(fs, &e2->f, e1->f);
      *e1 = *e2;
      break;
    }
    case OPR_OR: {
      ((void)0); /* lua_assert(e1->f == NO_JUMP); */ /* list must be closed */
      luaK_dischargevars(fs, e2);
      luaK_concat(fs, &e2->t, e1->t);
      *e1 = *e2;
      break;
    }
    case OPR_CONCAT: {
      luaK_exp2val(fs, e2);

	  /*

          GET_OPCODE(getcode(fs, e2))
          (((OpCode)(((((fs)->f->code[(e2)->u.s.info]))>>0) & ((~((~(Instruction)0)<<6))<<0))))

          
          ( (OpCode)( ( ( ( (fs)->f->code[ (e2)->u.s.info] ) ) >> 0 )   & ((~((~(Instruction)0)<<6))<<0) ) )

	  */
      if (e2->k == VRELOCABLE && ( (OpCode)( ( ( ( (fs)->f->code[ (e2)->u.s.info] ) ) >> 0 )   & ((~((~(Instruction)0)<<6))<<0) ) ) == OP_CONCAT) {
        ((void)0); /* lua_assert(e1->u.s.info == GETARG_B(getcode(fs, e2))-1); */
        freeexp(fs, e1);
		/*
           SETARG_B(getcode(fs, e2), e1->u.s.info);
           ((((fs)->f->code[(e2)->u.s.info])) = (((((fs)->f->code[(e2)->u.s.info]))&(~((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))) | ((((Instruction)(e1->u.s.info))<<(((0 + 6) + 8) + 9))&((~((~(Instruction)0)<<9))<<(((0 + 6) + 8) + 9)))));
        */

	       (fs)->f->code[ (e2)->u.s.info]   = 
		   ( ( ( (fs)->f->code[ (e2)->u.s.info] ) )             &  ( ~((~((~(Instruction)0)<<9))<<23) ) )
	    |  ( ( ((Instruction)(e1->u.s.info)) << 23 )   &  (  ((~((~(Instruction)0)<<9))<<23) ) ) ;

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
      ((void)0);   /* lua_assert(0); */
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

  /*
  luaM_growvector(fs->L, f->code, fs->pc, f->sizecode, Instruction,
                  MAX_INT, "code size overflow");
  */
  if ( fs->pc + 1 > f->sizecode ) { 
      f->code = ( (Instruction *)(luaM_growaux_(fs->L, f->code, &(f->sizecode), sizeof(Instruction), MAX_INT,"code size overflow") ) );
  }

  f->code[fs->pc] = i;
  /* save corresponding line information */

  /*
  luaM_growvector(fs->L, f->lineinfo, fs->pc, f->sizelineinfo, int,
                  MAX_INT, "code size overflow");
  */
  if ( fs->pc + 1 > f->sizelineinfo ) { 
      f->lineinfo = ( (int *)(luaM_growaux_(fs->L,f->lineinfo,&(f->sizelineinfo),sizeof(int), MAX_INT,"code size overflow") ) );
  }

  f->lineinfo[fs->pc] = line;
  return fs->pc++;
}


int luaK_codeABC (FuncState *fs, OpCode o, int a, int b, int c) {
  ((void)0); /* lua_assert(getOpMode(o) == iABC); */
  ((void)0); /* lua_assert(getBMode(o) != OpArgN || b == 0); */
  ((void)0); /* lua_assert(getCMode(o) != OpArgN || c == 0); */
  /*
     CREATE_ABC(o, a, b, c)
     ((((Instruction)(o))<<0) | (((Instruction)(a))<<(0 + 6)) | (((Instruction)(b))<<(((0 + 6) + 8) + 9)) | (((Instruction)(c))<<((0 + 6) + 8)))

	   (  
		  ( (Instruction)( (o) << 0  ) )
	|     ( (Instruction)( (a) << 6  ) )
	|     ( (Instruction)( (b) << 23 ) )
	|     ( (Instruction)( (c) << 14 ) )
       )
  */

  return luaK_code(fs, (  
		  ( (Instruction)( (o) << 0  ) )
	|     ( (Instruction)( (a) << 6  ) )
	|     ( (Instruction)( (b) << 23 ) )
	|     ( (Instruction)( (c) << 14 ) )
       ), fs->ls->lastline);
}


int luaK_codeABx (FuncState *fs, OpCode o, int a, unsigned int bc) {
  ((void)0); /* lua_assert(getOpMode(o) == iABx || getOpMode(o) == iAsBx); */
  ((void)0); /* lua_assert(getCMode(o) == OpArgN); */

  /*
  CREATE_ABx(o, a, bc)
  ((((Instruction)(o))<<0) | (((Instruction)(a))<<(0 + 6)) | (((Instruction)(bc))<<((0 + 6) + 8)))

	   ( ((Instruction)(o)) << 0 ) 
	 | ( ((Instruction)(a)) << 6 ) 
	 | ( ((Instruction)(bc)) << 14 ) 
  */
  return luaK_code(fs, 
	 (
	     ( ((Instruction)(o)) << 0 ) 
	   | ( ((Instruction)(a)) << 6 ) 
	   | ( ((Instruction)(bc)) << 14 ) 
	),   
	fs->ls->lastline);

}


void luaK_setlist (FuncState *fs, int base, int nelems, int tostore) {
  /*            LFIELDS_PER_FLUSH 50 */
  int c = (nelems - 1)/LFIELDS_PER_FLUSH + 1;
  /*                   LUA_MULTRET (-1) */
  int b = ( tostore == LUA_MULTRET ) ? 0 : tostore;
  ((void)0); /* lua_assert(tostore != 0); */

  /*       MAXARG_C  ((1<<9)-1)  */
  if (c <= MAXARG_C ) {
    luaK_codeABC(fs, OP_SETLIST, base, b, c);
  } else {
    luaK_codeABC(fs, OP_SETLIST, base, b, 0);
    /*            ((Instruction)(c)) */
    luaK_code(fs, cast(Instruction, c), fs->ls->lastline);
  }
  fs->freereg = base + 1; /* free registers with list values */
}


