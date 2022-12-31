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
  /*             luaM_newvector(L, BASIC_CI_SIZE, CallInfo); */
  /*                               BASIC_CI_SIZE      8  */
  L1->base_ci = (CallInfo *)(  ((size_t)(BASIC_CI_SIZE+1)) <=  MAX_SIZET / sizeof(CallInfo) 
		                     ? luaM_realloc_(L, NULL, 0 * sizeof(CallInfo), BASIC_CI_SIZE * sizeof(CallInfo) ) 
							 : luaM_toobig(L) );
  L1->ci = L1->base_ci;
  /*                 8        BASIC_CI_SIZE */
  L1->size_ci = BASIC_CI_SIZE;
  L1->end_ci = L1->base_ci + L1->size_ci - 1; // the address of the last element in the CallInfo's array

  /* initialize stack array */
  /*          luaM_newvector(L, BASIC_STACK_SIZE + EXTRA_STACK, TValue); */
  /*                                     BASIC_STACK_SIZE  (2*20)            #define LUA_MINSTACK	20  */
  /*                                     EXTRA_STACK         5           */
  L1->stack = (TValue *)(    ((size_t)( (BASIC_STACK_SIZE + EXTRA_STACK)+1)) <= MAX_SIZET/sizeof(TValue) 
				           ? luaM_realloc_(L, NULL, 0 * sizeof(TValue), (BASIC_STACK_SIZE + EXTRA_STACK) * sizeof(TValue) ) 
				           : luaM_toobig(L) 
						);

  /*              (2*20)           +         5; */
  L1->stacksize = BASIC_STACK_SIZE +  EXTRA_STACK;
  L1->top = L1->stack;  // top ->    the address of the 1st element in the stack array

  // array.size = 45 , stack_last ->  the 39th(40-1) element
  /*                                               5  EXTRA_STACK */
  L1->stack_last = L1->stack+(L1->stacksize - EXTRA_STACK) - 1;
  /* initialize first ci */
  L1->ci->func = L1->top;
  /* setnilvalue(L1->top++); */
  (L1->top++)->tt = LUA_TNIL; /* `function' entry for this `ci' */
  L1->base = L1->ci->base = L1->top;

  //  L1->ci->top   ===>  [21] / [45]     Tvalue
  /*                      20           LUA_MINSTACK */
  L1->ci->top = L1->top + LUA_MINSTACK;
}


static void freestack (lua_State *L, lua_State *L1) {
  /* luaM_freearray(L, L1->base_ci, L1->size_ci, CallInfo); */
  ( ( (size_t)(0+1) <= MAX_SIZET/sizeof(CallInfo) ) 
     ? luaM_realloc_(L, L1->base_ci, L1->size_ci * sizeof(CallInfo), 0 * sizeof(CallInfo) ) 
	 : luaM_toobig(L) );

  /* luaM_freearray(L, L1->stack, L1->stacksize, TValue); */
  ( ( (size_t)(0+1) <= MAX_SIZET/sizeof(TValue) ) 
     ? luaM_realloc_(L, L1->stack, L1->stacksize * sizeof(TValue), 0 * sizeof(TValue) ) 
	 : luaM_toobig(L));
}


/*
** open parts that may cause memory-allocation errors
*/
static void f_luaopen (lua_State *L, void *ud) {
  /*                 G(L) */
  global_State *g = (L->l_G);
  /* UNUSED(ud); */
  ((void)(ud));
  stack_init(L, L); /* init stack */

  /* sethvalue(L, gt(L), luaH_new(L, 0, 2)); */
  { 
      /*             gt(L)  */
      TValue *i_o = (&L->l_gt); 
      //                                   with 0 elements in array part , 
      //                                      with 2 key-value pairs in hash part
      i_o->value.gc = (GCObject *)( luaH_new(L, 0, 2) ); 
      i_o->tt = LUA_TTABLE;
      ((void)0); 
  }; /* table of globals */

  /* sethvalue(L, registry(L), luaH_new(L, 0, 2)); */
  { 
      /*            registry(L) */
      TValue *i_o = &(L->l_G)->l_registry;
      i_o->value.gc = (GCObject *)( luaH_new(L, 0, 2) ); 
      i_o->tt = LUA_TTABLE; 
      ((void)0); 
  }; /* registry */

  // alloc a string table with 32 TString elements
  /*             32            MINSTRTABSIZE */
  luaS_resize(L, MINSTRTABSIZE); /* initial size of string table */
  luaT_init(L); // ltm.c:38   alloc tag-method's reservered keywords TString and set flags :  e.g.  __index, __newindex , __eq , ...
  luaX_init(L); // llex.c:77  alloc lua keyword TStrings and set flags : e.g.  "local" / "and" / "or" / "nil" / "then" / "repeat" / "util" / ...

  /*
      Notes:

      ----------
      |a|b|c|\0|   =>  sizeof("abc") yield  4 rather than 3
      ----------

         sizeof("abc") turns out 4 ( not 3 , because it take '\0' into account ) , so you should minus 1 if you want to get the real length of "abc"
         
  */
  // push a new TString "not enough memory" into global stringtable 
  /* luaS_fix(luaS_newliteral(L, MEMERRMSG)); */
  /*  MEMERRMSG    "not enough memory"         */
  ((luaS_newlstr(L, MEMERRMSG, (sizeof(MEMERRMSG)/sizeof(char))-1)))->tsv.marked  |=  (1<<5);

  // set the threshold as 4 times of the current alloced memory size
  g->GCthreshold = 4*g->totalbytes;
}

// 初始化 lua_State 结构体 中的成员
static void preinit_state (lua_State *L, global_State *g) {
  /* G(L)    */
  (L->l_G) = g;
  L->stack = NULL;
  L->stacksize = 0;
  L->errorJmp = NULL;
  L->hook = NULL;
  L->hookmask = 0;
  L->basehookcount = 0;
  L->allowhook = 1;
  /* resethookcount(L); */
  L->hookcount = L->basehookcount;
  L->openupval = NULL;
  L->size_ci = 0;
  L->nCcalls = L->baseCcalls = 0;
  L->status = 0;
  L->base_ci = L->ci = NULL;
  L->savedpc = NULL;
  L->errfunc = 0;
  /* setnilvalue(gt(L)); */
  /* gt(L)  */
  // TValue l_gt;  /* table of globals */
  (&L->l_gt)->tt = LUA_TNIL;
}


static void close_state (lua_State *L) {
  /*                 G(L) */
  global_State *g = (L->l_G);
  luaF_close(L, L->stack); /* close all upvalues for this thread */
  luaC_freeall(L); /* collect all objects */

  /* lua_assert(g->rootgc == obj2gco(L)); */
  ((void)0);
  /* lua_assert(g->strt.nuse == 0); */
  ((void)0);

  /* luaM_freearray(L, G(L)->strt.hash, G(L)->strt.size, TString *); */
  (     (size_t)(0+1) <= MAX_SIZET/sizeof(TString *) ) 
      ? luaM_realloc_(L, (L->l_G)->strt.hash, L->l_G->strt.size * sizeof(TString *), 0 * sizeof(TString *) ) 
	  : luaM_toobig(L);

  /* luaZ_freebuffer(L, &g->buff); */
  ( 
    (&g->buff)->buffer = ( (char *)(   (size_t)(0+1) <= MAX_SIZET / sizeof(char) 
                           ? luaM_realloc_(L, (&g->buff)->buffer, (&g->buff)->buffsize * sizeof(char), 0 * sizeof(char) ) 
                           : luaM_toobig(L) ) ) 
  ), 
  (&g->buff)->buffsize = 0;

  freestack(L, L);
  /* lua_assert(g->totalbytes == sizeof(LG)); */
  ((void)0);
  /*                     fromstate(L),         state_size(LG)    */
  (*g->frealloc)(g->ud, ((lu_byte *)(L) - 0), (sizeof(LG) + 0), 0);
}


lua_State *luaE_newthread (lua_State *L) {
  /*              tostate(luaM_malloc(L, state_size(lua_State))); */
  lua_State *L1 = (lua_State *)(  (lu_byte *)(luaM_realloc_(L, NULL, 0, (sizeof(lua_State) + 0)) ) + 0 );
  /*             obj2gco(L1)           8  LUA_TTHREAD */
  luaC_link(L, (GCObject *)(L1), LUA_TTHREAD);
  /*                   G(L) */
  preinit_state(L1, (L->l_G));
  stack_init(L1, L); /* init stack */

  /* setobj2n(L, gt(L1), gt(L)); */
  { 
      /*                  gt(L) */
      const TValue *o2 = (&L->l_gt); 
      /*            gt(L1) */
      TValue *o1 = (&L1->l_gt); 
      o1->value = o2->value; 
      o1->tt = o2->tt; 
      ((void)0); 
  }; /* share table of globals */

  L1->hookmask = L->hookmask;
  L1->basehookcount = L->basehookcount;
  L1->hook = L->hook;

  /* resethookcount(L1); */
  L1->hookcount = L1->basehookcount;

  /* lua_assert(iswhite(obj2gco(L1))); */
  ((void)0);
  return L1;
}


void luaE_freethread (lua_State *L, lua_State *L1) {
  luaF_close(L1, L1->stack); /* close all upvalues for this thread */

  /* lua_assert(L1->openupval == NULL); */
  ((void)0);
  /* luai_userstatefree(L1); */
  ((void)L1);

  freestack(L, L1);
  /* luaM_freemem(L,        fromstate(L1), state_size(lua_State)); */
  luaM_realloc_(L, ( (lu_byte *)(L1) - 0), (sizeof(lua_State) + 0), 0);
}

/* 
                            //  lauxlib.c:764
   lua_Alloc f =  static void *l_alloc (void *ud, void *ptr, size_t osize, size_t nsize); 
   void    *ud =  NULL
*/
/* 
extern LUA_API   */
LUA_API  lua_State *lua_newstate (lua_Alloc f, void *ud) {
  int i;
  lua_State *L;
  global_State *g;

  // struct LG { lua_State l; global_State g; };  // lstate.c:35
  // memory alloc a sturct LG's size
  /*                           state_size(LG) */
  /************************************************************
     Core Core Core:

     Why ?
         注意: 这里分配新的内存块时，为什么不用封装好的 luaM_realloc_(...) 函数 , 而用原始的 l_alloc(...) 进行分配呢 ? 同时，可以看到在此 l_alloc 执行完以后 ，立即通过代码判定指针 [是否] 为空的逻辑判定语句 , 而没有使用 luaD_throw(...)
         因为:  
                luaM_realloc_(...) 是被封装在 luaD_rawrunprotected(...) 中执行的

        luaD_rawrunprotected() 封装了C语言的中断函数:
           类似C++ 的 try { ... } catch { ... } 语句

        在 任何<子>函数中发生问题的时候, 立即 中断( 而不使用常规做法， 在每个函数发生错误时，使用 return xxx 语句来中止当前函数的执行

        因为这种代码的书写方式，在编码时会有一些[不]美观
e.g.
        
         auto          succ = func_1();
         if ( succ ) { succ = func_2(); }
         if ( succ ) { succ = func_3(); }
         if ( succ ) { succ = func_4(); }
        
or   

        if (     func_1() && func_2() && func_3() && func_4() ) {
            ...
        } else {
           // 逻辑控制，并不知道上面的 if 语句中，到底在哪个函数中发生了错误
           //        func_1()   
           //   or   func_2()    
           //   or   func_3()    
           //   or   func_4() 
           //  都有可能
        }




e.g.
        static jmp_buf buf;

        int main()

            if ( setjmp(buf) == 0 ) {
                func_1();
                func_2();
                func_3();
                func_4();
            }

            nextLogic(...);
        }


        void func_2()
        {

            ... 

            if ( meet_error ) {
                // longjmp(buf,1);
                // --> 1 != 0 , if 块的后续函数就没有机会执行了，
                //
                // 直接跳出func_2的  if 块 , 
                //    restCode() 也[没有]执行了
                // 
                // 与此同时， 跳出 if(  setjmp(b) == 0 ) { ... } , 因为立即无条件跳转，瞬间被侦测到了，条件不满足
                //     func_3();  func_4(); 都[没有]机会执行了。
                // 
                // 于是执行main() 函数的 nextLogic(...)




                longjmp(buf, 1);   
            }

            restCode();
        }



  *************************************************************/
  void *l = (*f)(ud, NULL, 0, (sizeof(LG) + 0));
  if (l == NULL) {
    return NULL;
  }

  /* 
     --------------------------------------------------------------------
        LG :    | LUAI_EXTRASPACE ||       lua_State | global_State ||
     --------------------------------------------------------------------

        if LUAI_EXTRASPACE is not 0
        the memory address of lua_State is   ( ( (unsigned char*)l)  + LUAI_EXTRASPACE )
  
  */

  /*    tostate(l); */
  L = (lua_State *)( (lu_byte *)(l)     + 0);
  // global_State gtmp = ((LG *)L)->g;
  // g = &gtmp;
  // g = &(    ((LG *)L)->g   );
  g = &((LG *)L)->g;

  /***************************************************

    初始化 L 结构体中的一些成员值

  **************************************************/
  L->next = NULL;
  /*            8 LUA_TTHREAD */
  L->tt = LUA_TTHREAD;
  //                      FIXEDBIT    WHITE0BIT
  // lu_byte currentwhite = | 0010 0001 | (B) = 0x21
  /*          bit2mask(WHITE0BIT, FIXEDBIT); */
  g->currentwhite = ( (1<<0)    | (1<<5) );
  // lu_byte marked = | 0010 0001 |(B)   
  //               &  | 0000 0011 |(B)
  // --------------------------------------------------
  //                  | 0000 0001 |(B) =  0x1
  /*           luaC_white(g); */
  L->marked = (lu_byte)( g->currentwhite & ((1<<0) | (1<<1)) );
  //        SFIXEDBIT FIXEDBIT
  // L->marked = | 0110 0001 |(B)
  /* set2bits(L->marked, FIXEDBIT, SFIXEDBIT); */
  L->marked |= ( (1<<5) | (1<<6) ); // L->marked = 0x61 = | 0110 0001 |(B)

  // lstate.h:128 , 初始化结构体中的成员 , 置为 0 或 1
  preinit_state(L, g);

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // 初始化 global_State 结构体中的一些成员值
  //
  g->frealloc = f; // set alloc/free callback function
  g->ud = ud;     // user-data field is set as  NULL passed by  'ud'
  g->mainthread = L;
  g->uvhead.u.l.prev = &g->uvhead;
  g->uvhead.u.l.next = &g->uvhead;
  g->GCthreshold = 0; /* mark it as unfinished state */
  g->strt.size = 0;
  g->strt.nuse = 0;
  g->strt.hash = NULL;

  /* setnilvalue(registry(L)); */
  /*           registry(L)     */
  ((&(L->l_G)->l_registry))->tt = LUA_TNIL;


  // typedef struct Mbuffer { ... }    // lzio.h:24
  /* luaZ_initbuffer(L, &g->buff); */
  ( (&g->buff)->buffer = NULL, 
    (&g->buff)->buffsize = 0
  );
  g->panic = NULL;
  /*               0    GCSpause */
  g->gcstate = GCSpause;
  /*            obj2gco(L); */
  g->rootgc = (GCObject *)(L);
  g->sweepstrgc = 0;
  g->sweepgc = &g->rootgc;
  g->gray = NULL;
  g->grayagain = NULL;
  g->weak = NULL;
  g->tmudata = NULL;
  g->totalbytes = sizeof(LG);
  /*             200     LUAI_GCPAUSE */
  g->gcpause = LUAI_GCPAUSE;
  /*             200   LUAI_GCMUL */
  g->gcstepmul = LUAI_GCMUL;
  g->gcdept = 0;
  /*           (8+1)  NUM_TAGS */
  for (i=0; i < NUM_TAGS; i++) {
    // struct Table *mt[NUM_TAGS];  /* metatables for basic types */
    g->mt[i] = NULL;
  }
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  // luaD_rawrunprotected(...)      ldo.c:123  
  // f_luaopen                      lstate.c:92
  if (luaD_rawrunprotected(L, f_luaopen, NULL) != 0) {
    // result != 0  =>
    // !!! Failed !!!
    /* memory allocation error: free partial state */
    close_state(L);
    L = NULL;
  }
  else {
    // !!! Success !!! 
    //  execute function  :
    //       luaD_rawrunprotected(L, f_luaopen, NULL)   -> return 0
    /* luai_userstateopen(L); */
    ((void)L);
  }
  return L;
}


static void callallgcTM (lua_State *L, void *ud) {
  /* UNUSED(ud); */
  ((void)(ud));

  luaC_callGCTM(L); /* call GC metamethods for all udata */
}

/*
LUA_API extern */
LUA_API void lua_close (lua_State *L) {
  /*      G(L)->mainthread;                                       */
  L = (L->l_G)->mainthread; /* only the main thread can be closed */
  /* lua_lock(L); */
  ((void) 0);

  luaF_close(L, L->stack); /* close all upvalues for this thread */
  luaC_separateudata(L, 1); /* separate udata that have GC metamethods */
  L->errfunc = 0; /* no error function during GC metamethods */
  do { /* repeat until no more errors */
    L->ci = L->base_ci;
    L->base = L->top = L->ci->base;
    L->nCcalls = L->baseCcalls = 0;
  } while (luaD_rawrunprotected(L, callallgcTM, NULL) != 0);

  /* lua_assert(G(L)->tmudata == NULL); */
  ((void)0);

  /* luai_userstateclose(L); */
  ((void)L);

  close_state(L);
}


