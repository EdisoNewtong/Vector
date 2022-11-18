/*
** $Id: lstring.c,v 2.8.1.1 2007/12/27 13:02:25 roberto Exp $
** String table (keeps all strings handled by Lua)
** See Copyright Notice in lua.h
*/


#include <string.h>

#define lstring_c
#define LUA_CORE

#include "lua.h"

#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"


/*
    resize 
*/
void luaS_resize (lua_State *L, int newsize) {
  GCObject **newhash;
  stringtable *tb;
  int i;
  /*      G(L)                    2          GCSsweepstring */
  if ((L->l_G)->gcstate == GCSsweepstring) {
    return; /* cannot resize during GC traverse */
  }

  // alloc a new hash table with `newsize` elements
  /*         luaM_newvector(L, newsize, GCObject *); */
  newhash = (GCObject * *)(  (size_t)(newsize+1) <= MAX_SIZET / sizeof(GCObject *)  
			               ? luaM_realloc_(L, NULL, 0 * sizeof(GCObject *), newsize * sizeof(GCObject *) ) 
						   : luaM_toobig(L) );

  // set a reference pointer from   `&G(L)->strt`   <==   Core Core Core
  /*tb=&G(L)->strt; */
  tb = &(L->l_G)->strt;

  for (i=0; i<newsize; i++) {
    newhash[i] = NULL; // init each elements inside hash table as   NULL
  }

  /**********************************************************************
        
       Re-Order Hash Table :

Assume Situation-1 :
    old bucket count : 2  
    new bucket count : 4


--------------------------------------------------
    Old Summary :
--------------------------------------------------
       a.hash = 4    a.slotIdx = 4   % 2 = 0
       b.hash = 8    b.slotIdx = 8   % 2 = 0
       c.hash = 12   c.slotIdx = 12  % 2 = 0

       d.hash = 5    d.slotIdx = 5   % 2 = 1
       e.hash = 9    e.slotIdx = 9   % 2 = 1
       f.hash = 13   f.slotIdx = 13  % 2 = 1

--------------------------------------------------
    New Summary :
--------------------------------------------------
       a.hash = 4    a.slotIdx = 4   % 4 = 0
       b.hash = 8    b.slotIdx = 8   % 4 = 0
       c.hash = 12   c.slotIdx = 12  % 4 = 0

       d.hash = 5    d.slotIdx = 5   % 4 = 1
       e.hash = 9    e.slotIdx = 9   % 4 = 1
       f.hash = 13   f.slotIdx = 13  % 4 = 1

 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

           Old[0] : a -> b -> c
           New[0] : c -> b -> a

           Old[1] : d -> e -> f
           New[1] : f -> e -> d


####################################################################################################
####################################################################################################
####################################################################################################
####################################################################################################




Assume Situation-2 :
    old bucket count : 4  
    new bucket count : 8


--------------------------------------------------
    Old Summary :
--------------------------------------------------
       a.hash = 4    a.slotIdx = 4   % 4 = 0
       b.hash = 8    b.slotIdx = 8   % 4 = 0
       c.hash = 12   c.slotIdx = 12  % 4 = 0

       d.hash = 5    d.slotIdx = 5   % 4 = 1
       e.hash = 9    e.slotIdx = 9   % 4 = 1
       f.hash = 13   f.slotIdx = 13  % 4 = 1

--------------------------------------------------
    New Summary :  Core Core Core 
--------------------------------------------------
       a.hash = 4    a.slotIdx = 4   % 8 = 4
       b.hash = 8    b.slotIdx = 8   % 8 = 0
       c.hash = 12   c.slotIdx = 12  % 8 = 4

       d.hash = 5    d.slotIdx = 5   % 8 = 5
       e.hash = 9    e.slotIdx = 9   % 8 = 1
       f.hash = 13   f.slotIdx = 13  % 8 = 5


           Old[0] : a -> b -> c
           Old[1] : d -> e -> f

           New[0] : b
           New[1] : e
           New[2] : <NULL>
           New[3] : <NULL>
           New[4] : c -> a
           New[5] : f -> d
           New[6] : <NULL>
           New[7] : <NULL>





  **********************************************************************/
  /* rehash */
  for (i=0; i<tb->size; i++) {
    GCObject *p = tb->hash[i];
    while (p) { /* for each node in the list */
      GCObject *next = p->gch.next; /* save next */
      /*                          gco2ts(p)->hash */
      unsigned int h = (&(&((p)->ts))->tsv)->hash;
      /*        lmod(h, newsize); */
      int h1 = (int)( h & (newsize-1) ); /* new position */
      /* lua_assert(cast_int(h%newsize) == lmod(h, newsize)); */
      ((void)0);
      p->gch.next = newhash[h1]; /* chain it */
      newhash[h1] = p;
      p = next;
    }
  }
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  //
  // free old string table's hash table
  //
  /* luaM_freearray(L, tb->hash, tb->size, TString *); */
  (   (size_t)(0+1) <= MAX_SIZET/ sizeof(TString *) 
    ? luaM_realloc_(L, tb->hash, tb->size * sizeof(TString *), 0 * sizeof(TString *) ) 
	: luaM_toobig(L) );

  // set new size and new alloced hash for the global string's hash table
  tb->size = newsize;
  tb->hash = newhash;
}


/*
    alloc a TString with 

        str |  2nd arg : str's content     
        l   |  3rd arg : str's length      
        h   |  4th arg : str's hash code   

*/
static TString *newlstr (lua_State *L, const char *str, size_t l,
                                       unsigned int h) {
  TString *ts;
  stringtable *tb;
  
  // l is the length of the content of "str" ,    (l+1) means "str" + '\0'
  if ( l+1 > (MAX_SIZET - sizeof(TString)) / sizeof(char) ) {
    luaM_toobig(L);
  }


  /////////////////////////////////////////////////////////////////
  //
  // ----------------------------------------------------------------
  // ||     Part1               ||    Part 2                       ||
  // ----------------------------------------------------------------
  // ||  sizeof(struct TString) ||   |h|e|l|l|o| |w|o|r|l|d|\0|    ||  here "hello world" is a dummy example only
  // ----------------------------------------------------------------
  /*               luaM_malloc(L, (l+1)*sizeof(char)+sizeof(TString)) */
  /*                                +1 : is take '\0' into account    */
  ts = (TString *)luaM_realloc_(L, NULL, 0,  (l+1) * sizeof(char) + sizeof(TString)  );
  ts->tsv.len = l;
  ts->tsv.hash = h;

  //      marked = 0x21 & (0x3) = 0x1
  /*                luaC_white(G(L)); */
  ts->tsv.marked = (lu_byte)( (L->l_G)->currentwhite & ((1<<0) | (1<<1)) );
  /*                 4      LUA_TSTRING */
  ts->tsv.tt = LUA_TSTRING;
  ts->tsv.reserved = 0; // init as Un-Reserved word

  
  // copy the real str content into the 2nd part with '\0'
  memcpy(ts+1, str, l*sizeof(char));
  ((char *)(ts+1))[l] = '\0'; /* ending 0 */

  /*     G(L) */
  tb = &(L->l_G)->strt;

  //
  // there are  <tb->size> slots in the global string table
  //
  // use modulo value as the slot index to be placed ( for performance aspect use  & op rather than  % )
  //  but    tb->size must be a number whose value is power(2,n) , otherwise can't use op & instead of op %
  //
  // if      the result of log2(n) is an interger number
  // then     (a % n) == ( a & (n-1) )
  // the exp ( a & (n-1) )  is  more efficient than  (a % n)
  //
  // Notes : 
  //        tb->size is container's capacity rather than the element's count
  //
  /*    lmod(h, tb->size); */
  h = (int)( h & (tb->size-1) );

  /*
    Core Core Core:
                        <Install> chain

        另外 ： 根据如下算法，很容易制造出2个字符串内容不同，但是Hash值相同的2个字符串

    // get the char every 2 step
     2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2        
    12345678901234567890123456789012		// 32位，从最后一位开始计算，每隔2位计算1次 
    a2a4a6a8a0a2a4a6a8a0a2a4a6a8a0a2		// 32位，从最后一位开始计算，每隔2位计算1次

    
    hash1 == hash2   =>   true

    // insert str1 : "12345678901234567890123456789012"
    newlstr(L, 
                   "12345678901234567890123456789012", 
            strlen("12345678901234567890123456789012"), 
            hash1 );
    // tb->hash[hash1] -> str1 -> NULL

    // insert str2 : "a2a4a6a8a0a2a4a6a8a0a2a4a6a8a0a2"
    newlstr(L, 
                   "a2a4a6a8a0a2a4a6a8a0a2a4a6a8a0a2", 
            strlen("a2a4a6a8a0a2a4a6a8a0a2a4a6a8a0a2"), 
            hash2 );

       // tb->hash[hash2] -> str2 -> str1 -> NULL

  */
  ts->tsv.next = tb->hash[h]; /* chain new entry */

  // place the alloced ts object into the property slot 
  /*             obj2gco(ts); */
  tb->hash[h] = (GCObject *)(ts);
  tb->nuse++;

  /*               cast(lu_int32, tb->size)            MAX_INT     (INT_MAX-2)  (2147483647 -2)*/
  if (tb->nuse > ((lu_int32)(tb->size)) && tb->size <= MAX_INT/2) {
    luaS_resize(L, tb->size*2); /* too crowded */
  }
  return ts;
}


TString *luaS_newlstr (lua_State *L, const char *str, size_t l) {
  GCObject *o;
  unsigned int h = ((unsigned int)(l)); /* seed */

  /*************************************************** 

     Core Core Core : calculate string's Hash Code

  ****************************************************/

  // if str.length >=32 , partially hash the character step by step ( depand on the value of varible step )
  // if len == 32  , every 2 (1+1) step
  // if len == 64  , every 3 (2+1) step
  // if len == 128 , every 4 (3+1) step
  size_t step = (l>>5)+1; /* if string is too long, don't hash all its chars */
  size_t l1;
  for (l1=l; l1>=step; l1-=step) { /* compute hash */
    /*                           cast(unsigned char, str[l1-1]) */
    h = h ^ ( (h<<5) + (h>>2) + (unsigned char)(str[l1-1]) );
  }


  /*        G(L)                 lmod(h,  G(L)->strt.size)*/
  for (o = (L->l_G)->strt.hash[ (int)(h & (L->l_G->strt.size-1)) ];
       o != NULL;
       o = o->gch.next) {
    /*            rawgco2ts(o); */
    TString *ts = (&((o)->ts));

    //                              compare passed "str" with existed string's content in the string table
    /*                                     getstr(ts) */
    if (ts->tsv.len == l && (memcmp(str, (const char *)((ts) + 1), l) == 0) ) {
      /* string may be dead */
      /*   isdead(G(L), o)  */
      if (     o->gch.marked 
            /*    G(L)      */
			& ( (L->l_G)->currentwhite ^ ( (1<<0) | (1<<1) ) ) // otherwhite(g)
			& ( (1<<0) | (1<<1) )  // WHITEBITS   =>    bit2mask(WHITE0BIT, WHITE1BIT)
		) 
	  {
        /* changewhite(o); */
        o->gch.marked ^= ( (1<<0) | (1<<1) );
      }
      return ts;
    }
  }
  return newlstr(L, str, l, h); /* not found */
}


Udata *luaS_newudata (lua_State *L, size_t s, Table *e) {
  Udata *u;
  if (s > MAX_SIZET - sizeof(Udata)) {
    luaM_toobig(L);
  }
  /*               luaM_malloc(L, s + sizeof(Udata)) */
  u = ((Udata *)(luaM_realloc_(L, NULL, 0, (s + sizeof(Udata)))));
  /*                luaC_white(G(L)); */
  u->uv.marked = (lu_byte)( (L->l_G)->currentwhite & ( (1<<0) | (1<<1) ) ); /* is not finalized */
  /*               7 LUA_TUSERDATA */
  u->uv.tt = LUA_TUSERDATA;
  u->uv.len = s;
  u->uv.metatable = NULL;
  u->uv.env = e;
  /* chain it on udata list (after main thread) */
  /*               G(L) */
  u->uv.next = (L->l_G)->mainthread->next;
  /* G(L)                       obj2gco(u)     */
  (L->l_G)->mainthread->next = (GCObject *)(u);
  return u;
}


