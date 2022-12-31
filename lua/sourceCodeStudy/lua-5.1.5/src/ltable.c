/*
** $Id: ltable.c,v 2.32.1.2 2007/12/28 15:32:23 roberto Exp $
** Lua tables (hash)
** See Copyright Notice in lua.h
*/


/*
** Implementation of tables (aka arrays, objects, or hash tables).
** Tables keep its elements in two parts: an array part and a hash part.
** Non-negative integer keys are all candidates to be kept in the array
** part. The actual size of the array is the largest `n' such that at
** least half the slots between 0 and n are in use.
** Hash uses a mix of chained scatter table with Brent's variation.
** A main invariant of these tables is that, if an element is not
** in its main position (i.e. the `original' position that its hash gives
** to it), then the colliding element is in its own main position.
** Hence even when the load factor reaches 100%, performance remains good.
*/

#include <math.h>
#include <string.h>

#define ltable_c
#define LUA_CORE

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "ltable.h"


/*
** max size of array part is 2^MAXBITS
*/
#if LUAI_BITSINT > 26
    #define MAXBITS		26
#else
    #define MAXBITS		(LUAI_BITSINT-2)
#endif

#define MAXASIZE	(1 << MAXBITS)


#define hashpow2(t,n)      (gnode(t, lmod((n), sizenode(t))))
  
#define hashstr(t,str)  hashpow2(t, (str)->tsv.hash)
#define hashboolean(t,p)        hashpow2(t, p)


/*
** for some types, it is better to avoid modulus by power of 2, as
** they tend to have many 2 factors.
*/
#define hashmod(t,n)	(gnode(t, ((n) % ((sizenode(t)-1)|1))))


#define hashpointer(t,p)	hashmod(t, IntPoint(p))


/*
** number of ints inside a lua_Number
*/
#define numints		cast_int(sizeof(lua_Number)/sizeof(int))



#define dummynode		(&dummynode_)

static const Node dummynode_ = {
  {{NULL}, LUA_TNIL}, /* value */
  {{{NULL}, LUA_TNIL, NULL}} /* key */
};


/*
** hash for lua_Numbers
*/
static Node *hashnum (const Table *t, lua_Number n) {
  /*             (int)( sizeof(lua_Number)/sizeof(int) )  numints */
  unsigned int a[numints];
  int i;
  /*   luai_numeq(n, 0) */
  if ( n==0 ) { /* avoid problems with -0 */
    /*      gnode(t, 0) */
    return (&(t)->node[0]);
  }
  memcpy(a, &n, sizeof(a));
  /*             (int)( sizeof(lua_Number)/sizeof(int) )   numints */
  for (i = 1; i < numints; i++) {
    a[0] += a[i];
  }
  /*       hashmod(t, a[0]) */
  return ( &(t)->node[ a[0] % ( (( 1 << (t->lsizenode) ) -1 ) | 1) ] );
}



/*
** returns the `main' position of an element in a table (that is, the index
** of its hash value)
*/
static Node *mainposition (const Table *t, const TValue *key) {
  /*       ttype(key)  */
  switch ( key->tt ) {
    case LUA_TNUMBER: { /* 3 */ 
      /*                nvalue(key) */
      return hashnum(t, key->value.n );
    }
    case LUA_TSTRING: { /* 4 */
      /*      hashstr(t, rawtsvalue(key)); */
      return (&(t)->node[ (int)( ( (&(key)->value.gc->ts)->tsv.hash ) & ( (1 << (t->lsizenode))-1 ) ) ]);
    }
    case LUA_TBOOLEAN: { /* 1 */
      /*       hashboolean(t, bvalue(key)) */
      return (&(t)->node[ (int)( (key->value.b) & ( (1<< (t->lsizenode) )-1) ) ]);
    }
    case LUA_TLIGHTUSERDATA: { /* 2 */
      /*       hashpointer(t, pvalue(key)); */
      return (&(t)->node[ ((unsigned int)(lu_mem)(key->value.p)) % ( ( (1 << t->lsizenode) -1) | 1) ]);
    }
    default: {
      /*      hashpointer(t, gcvalue(key)); */
      return (&(t)->node[ ( (unsigned int)(lu_mem)(key->value.gc) ) % ( ( ( 1<< t->lsizenode ) - 1) | 1) ]);
    }
  }
}


/*
** returns the index for `key' if `key' is an appropriate key to live in
** the array part of the table, -1 otherwise.
*/
static int arrayindex (const TValue *key) {
  /*   ttisnumber(key) */
  if ( (key->tt) == LUA_TNUMBER ) {
    /*             nvalue(key); */
    lua_Number n = key->value.n;
    int k;
    /* lua_number2int(k, n); */
    k= (int)n;
    /*   luai_numeq(cast_num(k), n) */
    if ( ((lua_Number)(k)) == n  ) {
      return k;
    }
  }
  return -1; /* `key' did not match some condition */
}


/*
** returns the index of a `key' for table traversals. First goes all
** elements in the array part, then elements in the hash part. The
** beginning of a traversal is signalled by -1.
*/
static int findindex (lua_State *L, Table *t, StkId key) {
  int i;
  /*    ttisnil(key) */
  if ( key->tt == LUA_TNIL ) {
    return -1; /* first iteration */
  }
  i = arrayindex(key);
  if (0 < i && i <= t->sizearray) { /* is `key' inside array part? */
    return i-1; /* yes; that's the index (corrected to C) */
  } else {
    Node *n = mainposition(t, key);
    do { /* check whether `key' is somewhere in the chain */
      /* key may be dead already, but it is ok to use it in `next' */
      /*                   key2tval(n)             */
      if (luaO_rawequalObj( &(n)->i_key.tvk, key) ||
            /*        ttype(gkey(n))  == LUA_TDEADKEY    #define  LUA_TDEADKEY (LAST_TAG+3)  */
            /*                                       iscollectable(key) */
            ((((&(n)->i_key.nk))->tt) == (8 +3) && (((key)->tt) >= LUA_TSTRING) &&
             /* gcvalue(gkey(n))            ==  gcvalue(key)            */
             (((&(n)->i_key.nk))->value.gc) == key->value.gc))
      {
        /*               gnode(t, 0) */
        i = (int)(n - (&(t)->node[0])); /* key index in hash table */
        /* hash elements are numbered after array ones */
        return i + t->sizearray;
      }
      else {
        /*   gnext(n); */
        n = (n->i_key.nk.next);
      }
    } while (n);
    /*                              LUA_QL("next") */
    luaG_runerror(L, "invalid key to " "'" "next" "'"); /* key not found */
    return 0; /* to avoid warnings */
  }
}


int luaH_next (lua_State *L, Table *t, StkId key) {
  int i = findindex(L, t, key); /* find original element */
  for (i++; i < t->sizearray; i++) { /* try first array part */
    /*   ttisnil(&t->array[i]) */
    if (!((&t->array[i])->tt == LUA_TNIL ) ) { /* a non-nil value? */
      /* setnvalue(key, cast_num(i+1)); */
      { 
          TValue *i_o = key; 
          i_o->value.n = (lua_Number)(i+1); 
          i_o->tt = LUA_TNUMBER; 
      };

      /* setobj2s(L, key+1, &t->array[i]); */
      { 
          const TValue *o2 = &t->array[i]; 
          TValue *o1 = key+1; 
          o1->value = o2->value; 
          o1->tt = o2->tt; 
          ((void)0); 
      };
      return 1;
    }
  }
  /*                            sizenode(t) */
  for (i -= t->sizearray; i < (1 << (t->lsizenode)); i++) { /* then hash part */
    /*    ttisnil(gval(gnode(t, i))) */
    if (!(((&((&(t)->node[i]))->i_val)->tt) == LUA_TNIL) ) { /* a non-nil value? */
      /* setobj2s(L, key, key2tval(gnode(t, i))); */
      { 
          const TValue *o2 = (&((&(t)->node[i]))->i_key.tvk); 
          TValue *o1 = key; 
          o1->value = o2->value; 
          o1->tt = o2->tt; 
          ((void)0); 
      };
      /* setobj2s(L, key+1, gval(gnode(t, i))); */
      { 
          const TValue *o2 = &((&(t)->node[i]))->i_val; 
          TValue *o1 = key+1; 
          o1->value = o2->value; 
          o1->tt = o2->tt; 
          ((void)0); 
      };
      return 1;
    }
  }
  return 0; /* no more elements */
}


/*
** {=============================================================
** Rehash
** ==============================================================
*/


static int computesizes (int nums[], int *narray) {
  int i;
  int twotoi; /* 2^i */
  int a = 0; /* number of elements smaller than 2^i */
  int na = 0; /* number of elements to go to array part */
  int n = 0; /* optimal size for array part */
  for (i = 0, twotoi = 1; twotoi/2 < *narray; i++, twotoi *= 2) {
    if (nums[i] > 0) {
      a += nums[i];
      if (a > twotoi/2) { /* more than half elements present? */
        n = twotoi; /* optimal size (till now) */
        na = a; /* all elements smaller than n will go to array part */
      }
    }
    if (a == *narray) {
      break; /* all elements already counted */
    }
  }
  *narray = n;
  /* lua_assert(*narray/2 <= na && na <= *narray); */
  ((void)0);
  return na;
}


static int countint (const TValue *key, int *nums) {
  int k = arrayindex(key);
  /*                (1 << 26)   MAXASIZE */
  if (0 < k && k <= MAXASIZE ) { /* is `key' an appropriate array index? */
    /*    ceillog2(k)         */
    nums[ luaO_log2(k-1) + 1]++; /* count as such */
    return 1;
  }
  else {
    return 0;
  }
}


static int numusearray (const Table *t, int *nums) {
  int lg;
  int ttlg; /* 2^lg */
  int ause = 0; /* summation of `nums' */
  int i = 1; /* count to traverse all array keys */
  /*                        26  MAXBITS */
  for (lg=0, ttlg=1; lg<=MAXBITS; lg++, ttlg*=2) { /* for each slice */
    int lc = 0; /* counter */
    int lim = ttlg;
    if (lim > t->sizearray) {
      lim = t->sizearray; /* adjust upper limit */
      if (i > lim) {
        break; /* no more elements to count */
      }
    }
    /* count elements in range (2^(lg-1), 2^lg] */
    for (; i <= lim; i++) {
      /*    ttisnil(&t->array[i-1]) */
      if ( !((&t->array[i-1])->tt == LUA_TNIL) ) {
        lc++;
      }
    }
    nums[lg] += lc;
    ause += lc;
  }
  return ause;
}


static int numusehash (const Table *t, int *nums, int *pnasize) {
  int totaluse = 0; /* total number of elements */
  int ause = 0; /* summation of `nums' */
  /*       sizenode(t); */
  int i = 1 << (t->lsizenode);
  while (i--) {
    Node *n = &t->node[i];
    /*   ttisnil(gval(n)) */
    if (!( (&(n)->i_val)->tt == LUA_TNIL) ) {
      /*                 key2tval(n) */
      ause += countint( &(n)->i_key.tvk, nums);
      totaluse++;
    }
  }
  *pnasize += ause;
  return totaluse;
}


static void setarrayvector (lua_State *L, Table *t, int size) {
  int i;
  /* luaM_reallocvector(L, t->array, t->sizearray, size, TValue); */
  t->array = (TValue *)(  ((size_t)(size+1)) <= MAX_SIZET/sizeof(TValue)
                        ? luaM_realloc_(L, t->array, t->sizearray * sizeof(TValue), size*sizeof(TValue) ) 
                        : luaM_toobig(L) );
  for (i=t->sizearray; i<size; i++) {
     /* setnilvalue(&t->array[i]); */
     (&t->array[i])->tt = LUA_TNIL;
  }
  t->sizearray = size;
}


static void setnodevector (lua_State *L, Table *t, int size) {
  int lsize;
  if (size == 0) { /* no elements to hash part? */
    /*          cast(Node *, dummynode); */
    t->node = (Node *)(&dummynode_); /* use common `dummynode' */
    lsize = 0;
  }
  else {
    int i;
    /*      ceillog2(size); */
    lsize = luaO_log2(size-1) + 1;
    /*             26      MAXBITS */
    if (lsize > MAXBITS) {
      luaG_runerror(L, "table overflow");
    }
    /*     twoto(lsize); */
    size = 1<< lsize;
    /*         luaM_newvector(L, size, Node); */
    t->node = (Node *)(   ((size_t)(size+1)) <= MAX_SIZET/sizeof(Node)
                        ? luaM_realloc_(L, NULL,  0 * sizeof(Node), size * sizeof(Node) ) 
                        : luaM_toobig(L) );
    for (i=0; i<size; i++) {
      /*         gnode(t, i); */
      Node *n = (&(t)->node[i]);
      /* gnext(n) */
      (n->i_key.nk.next) = NULL;
      /* setnilvalue(gkey(n)); */
      (&(n)->i_key.nk)->tt = LUA_TNIL;
      /* setnilvalue(gval(n)); */
      (&(n)->i_val)->tt = LUA_TNIL;
    }
  }
  /*              cast_byte(lsize); */
  t->lsizenode = (lu_byte)(lsize);
  /*             gnode(t, size); */
  t->lastfree = &(t)->node[size]; /* all positions are free */
}


static void resize (lua_State *L, Table *t, int nasize, int nhsize) {
  int i;
  int oldasize = t->sizearray;
  int oldhsize = t->lsizenode;
  Node *nold = t->node; /* save old hash ... */
  if (nasize > oldasize) { /* array part must grow? */
    setarrayvector(L, t, nasize);
  }
  /* create new hash part with appropriate size */
  setnodevector(L, t, nhsize);
  if (nasize < oldasize) { /* array part must shrink? */
    t->sizearray = nasize;
    /* re-insert elements from vanishing slice */
    for (i=nasize; i<oldasize; i++) {
      /*    ttisnil(&t->array[i]) */
      if ( !((&t->array[i])->tt == LUA_TNIL) ) {
        /* setobjt2t(L, luaH_setnum(L, t, i+1), &t->array[i]); */
        { 
            const TValue *o2 = (&t->array[i]); 
            TValue *o1 = luaH_setnum(L, t, i+1); 
            o1->value = o2->value; 
            o1->tt = o2->tt; 
            ((void)0); 
        };
      }
    }
    /* shrink array */
    /* luaM_reallocvector(L, t->array, oldasize, nasize, TValue); */
    t->array = (TValue *)(   ( (size_t)(nasize+1) <= MAX_SIZET/sizeof(TValue) ) 
                           ? luaM_realloc_(L, t->array, oldasize * sizeof(TValue), nasize * sizeof(TValue) ) 
                           : luaM_toobig(L) );
  }
  /* re-insert elements from hash part */
  /*        twoto(oldhsize) */
  for (i = (1 << oldhsize) - 1; i >= 0; i--) {
    Node *old = nold+i;
    /*    ttisnil(gval(old)) */
    if (!( (&(old)->i_val)->tt == LUA_TNIL) ) {
      /* setobjt2t(L, luaH_set(L, t, key2tval(old)), gval(old)); */
      { 
          const TValue *o2 = (&(old)->i_val); 
          TValue *o1 = luaH_set(L, t, (&(old)->i_key.tvk) ); 
          o1->value = o2->value; 
          o1->tt = o2->tt; 
          ((void)0); 
      };
    }
  }
  /*           dummynode   */
  if (nold != (&dummynode_)) {
    /* luaM_freearray(L, nold, twoto(oldhsize), Node); */
    (     ( (size_t)(0+1) <= MAX_SIZET/sizeof(Node) ) 
        ? luaM_realloc_(L, nold, (1<<oldhsize) * sizeof(Node), 0*sizeof(Node) ) 
        : luaM_toobig(L) 
    ); /* free old array */
  }
}


void luaH_resizearray (lua_State *L, Table *t, int nasize) {
  /*                       dummynode              sizenode(t)*/
  int nsize = t->node == (&dummynode_) ? 0 : (1<<(t->lsizenode) );
  resize(L, t, nasize, nsize);
}


static void rehash (lua_State *L, Table *t, const TValue *ek) {
  int nasize, na;
  /*           26 MAXBITS */
  int nums[MAXBITS + 1]; /* nums[i] = number of keys between 2^(i-1) and 2^i */
  int i;
  int totaluse;
  /*              26    MAXBITS */
  for (i=0; i<=MAXBITS; i++) {
    nums[i] = 0; /* reset counts */
  }
  nasize = numusearray(t, nums); /* count keys in array part */
  totaluse = nasize; /* all those keys are integer keys */
  totaluse += numusehash(t, nums, &nasize); /* count keys in hash part */
  /* count extra key */
  nasize += countint(ek, nums);
  totaluse++;
  /* compute new size for array part */
  na = computesizes(nums, &nasize);
  /* resize the table to new computed sizes */
  resize(L, t, nasize, totaluse - na);
}


/*
** }=============================================================
*/
Table *luaH_new (lua_State *L, int narray, int nhash) {
  /*          luaM_new(L, Table); */
  Table *t = (Table *)( luaM_realloc_(L, NULL, 0, sizeof(Table) ) );
  /*              obj2gco(t)           5  LUA_TTABLE */
  luaC_link(L, (GCObject *)(t), LUA_TTABLE);
  t->metatable = NULL;
  /*          cast_byte(~0) */
  t->flags = (lu_byte)(~0);
  /* temporary values (kept only if some malloc fails) */
  t->array = NULL;
  t->sizearray = 0;
  t->lsizenode = 0;
  /*          cast(Node *, dummynode); */
  t->node = (Node *)(&dummynode_);
  setarrayvector(L, t, narray);
  setnodevector(L, t, nhash);
  return t;
}


void luaH_free (lua_State *L, Table *t) {
  /*              dummynode */
  if (t->node != (&dummynode_)) {
    /*  luaM_freearray(L, t->node, sizenode(t), Node); */
    (   (size_t)(0+1) <= MAX_SIZET/sizeof(Node) 
      ? luaM_realloc_(L, t->node, (1 << (t->lsizenode)) * sizeof(Node), 0 * sizeof(Node) ) 
      : luaM_toobig(L)
    );
  }
  /* luaM_freearray(L, t->array, t->sizearray, TValue); */
  (
    (   (size_t)(0+1) <= MAX_SIZET/sizeof(TValue) ) 
      ? luaM_realloc_(L, t->array, t->sizearray * sizeof(TValue), 0 * sizeof(TValue) ) 
      : luaM_toobig(L)
  );
  /* luaM_free(L, t); */
  luaM_realloc_(L, t, sizeof(*t), 0);
}


static Node *getfreepos (Table *t) {
  while (t->lastfree-- > t->node) {
    /*    ttisnil(gkey(t->lastfree) */
    if ( (&(t->lastfree)->i_key.nk)->tt == LUA_TNIL ) {
      return t->lastfree;
    }
  }
  return NULL; /* could not find a free place */
}


/*
** inserts a new key into a hash table; first, check whether key's main 
** position is free. If not, check whether colliding node is in its main 
** position or not: if it is not, move colliding node to an empty place and 
** put new key in its main position; otherwise (colliding node is in its main 
** position), new key goes to an empty position. 
*/
static TValue *newkey (lua_State *L, Table *t, const TValue *key) {
  Node *mp = mainposition(t, key);
  /*   ttisnil(gval(mp))                             dummynode   */
  if (!( ((&(mp)->i_val)->tt) == LUA_TNIL) || mp == (&dummynode_)) {
    Node *othern;
    Node *n = getfreepos(t); /* get a free place */
    if (n == NULL) { /* cannot find a free place? */
      rehash(L, t, key); /* grow table */
      return luaH_set(L, t, key); /* re-insert key into grown table */
    }
    /* lua_assert(n != dummynode); */
    ((void)0);
    /*                        key2tval(mp) */
    othern = mainposition(t, (&(mp)->i_key.tvk));
    if (othern != mp) { /* is colliding node out of its main position? */
      /* yes; move colliding node into free position */
      /*       gnext(othern) */
      while ( othern->i_key.nk.next != mp) {
        /*        gnext(othern) */
        othern = othern->i_key.nk.next; /* find previous */
      }
      /* gnext(othern) */
      othern->i_key.nk.next = n; /* redo the chain with `n' in place of `mp' */
      *n = *mp; /* copy colliding node into free pos. (mp->next also goes) */
      /* gnext(mp) */
      mp->i_key.nk.next = NULL; /* now `mp' is free */
      /* setnilvalue(gval(mp)); */
      (&(mp)->i_val)->tt = LUA_TNIL;
    }
    else { /* colliding node is in its own main position */
      /* new node will go into free position */
      /* gnext(n)      = gnext(mp); */
      n->i_key.nk.next = mp->i_key.nk.next; /* chain new position */
      /* gnext(mp) */
      mp->i_key.nk.next = n;
      mp = n;
    }
  }
  /* gkey(mp)->value       = key->value;          gkey(mp)->tt = key->tt; */
  (&(mp)->i_key.nk)->value = key->value; (&(mp)->i_key.nk)->tt = key->tt;

  /* luaC_barriert(L, t, key); */
  { 
    if (
            ( (key->tt >= LUA_TSTRING) && ( (key->value.gc->gch.marked) & ( (1<<0) | (1<<1) ) ) ) 
         && ( ((GCObject *)(t))->gch.marked & (1<<2) )
       ) 
    { 
        luaC_barrierback(L,t); 
    }
  };
  /* lua_assert(ttisnil(gval(mp))); */
  ((void)0);
  /*     gval(mp); */
  return &(mp)->i_val;
}


/*
** search function for integers
*/
const TValue *luaH_getnum (Table *t, int key) {
  /* (1 <= key && key <= t->sizearray) */
  /*   cast(unsigned int, key-1)  cast(unsigned int, t->sizearray)*/
  if ( (unsigned int)(key-1) < (unsigned int)(t->sizearray) ) {
    return &t->array[key-1];
  } else {
    /*               cast_num(key); */
    lua_Number nk = (lua_Number)(key);
    Node *n = hashnum(t, nk);
    do { /* check whether `key' is somewhere in the chain */
      /*   ttisnumber(gkey(n))                       luai_numeq(nvalue(gkey(n)), nk)*/
      if ( ( (&(n)->i_key.nk)->tt == LUA_TNUMBER) && (&(n)->i_key.nk)->value.n == nk ) {
        /*      gval(n); */
        return (&(n)->i_val); /* that's it */
      }
      else {
        /*  gnext(n); */
        n = n->i_key.nk.next;
      }
    } while (n);
    /*      luaO_nilobject */
    return (&luaO_nilobject_);
  }
}


/*
** search function for strings
*/
const TValue *luaH_getstr (Table *t, TString *key) {
  /*        hashstr(t, key); */
  Node *n = &(t)->node[ (int)( key->tsv.hash & ((1 << (t->lsizenode))-1) ) ];
  do { /* check whether `key' is somewhere in the chain */
    /*     ttisstring(gkey(n))                   rawtsvalue(gkey(n))*/
    if (  (&(n)->i_key.nk)->tt == LUA_TSTRING && (&(&(n)->i_key.nk)->value.gc->ts) == key) {
      /*     gval(n) */
      return &(n)->i_val; /* that's it */
    }
    else {
      /*  gnext(n); */
      n = n->i_key.nk.next;
    }
  } while (n);
  /*      luaO_nilobject */
  return (&luaO_nilobject_);
}


/*
** main search function
*/
const TValue *luaH_get (Table *t, const TValue *key) {
  /*      ttype(key) */
  switch ( key->tt ) {
    case LUA_TNIL: { /* 0 */
        /*      luaO_nilobject */
        return (&luaO_nilobject_);
    }
    case LUA_TSTRING: { /* 4 */
        /*                    rawtsvalue(key) */
        return luaH_getstr(t, &(key)->value.gc->ts );
    }
    case LUA_TNUMBER: { /* 3 */
      int k;
      /*             nvalue(key) */
      lua_Number n = key->value.n;
      /* lua_number2int(k, n); */
      k = (int)(n);
      /*     luai_numeq(cast_num(k), nvalue(key)) */
      if ( ( ((lua_Number)(k)) == (key->value.n) ) ) { /* index is int? */
        return luaH_getnum(t, k); /* use specialized version */
      }
      /* else go through */
    }
    default: {
      Node *n = mainposition(t, key);
      do { /* check whether `key' is somewhere in the chain */
        /*                    key2tval(n) */
        if (luaO_rawequalObj( &(n)->i_key.tvk, key)) {
          /*      gval(n) */
          return &(n)->i_val; /* that's it */
        }
        else {
          /*  gnext(n) */
          n = n->i_key.nk.next;
        }
      } while (n);
      /*      luaO_nilobject */
      return (&luaO_nilobject_);
    }
  }
}


TValue *luaH_set (lua_State *L, Table *t, const TValue *key) {
  const TValue *p = luaH_get(t, key);
  t->flags = 0;
  /*        luaO_nilobject */
  if (p != (&luaO_nilobject_)) {
    return ((TValue *)(p));
  } else {
    /*   ttisnil(key) */
    if ( key->tt == LUA_TNIL ) {
      luaG_runerror(L, "table index is nil");
      /*         ttisnumber(key)          luai_numisnan(nvalue(key))                */
    } else if ( key->tt == LUA_TNUMBER && (!( (key->value.n) == (key->value.n) ))  ) {
      luaG_runerror(L, "table index is NaN");
    }
    return newkey(L, t, key);
  }
}


TValue *luaH_setnum (lua_State *L, Table *t, int key) {
  const TValue *p = luaH_getnum(t, key);
  /*        luaO_nilobject */
  if (p != (&luaO_nilobject_)) {
    /*      cast(TValue *, p); */
    return (TValue *)(p);
  } else {
    TValue k;
    /* setnvalue(&k, cast_num(key)); */
    { 
        TValue *i_o = &k; 
        i_o->value.n = (lua_Number)(key); 
        i_o->tt = LUA_TNUMBER; 
    };
    return newkey(L, t, &k);
  }
}


TValue *luaH_setstr (lua_State *L, Table *t, TString *key) {
  const TValue *p = luaH_getstr(t, key);
  /*        luaO_nilobject */
  if (p != (&luaO_nilobject_)) {
    /*      cast(TValue *, p); */
    return (TValue *)(p);
  } else {
    TValue k;
    /* setsvalue(L, &k, key); */
    { 
        TValue *i_o = &k; 
        i_o->value.gc = (GCObject *)(key); 
        i_o->tt = LUA_TSTRING; 
        ((void)0); 
    };
    return newkey(L, t, &k);
  }
}


static int unbound_search (Table *t, unsigned int j) {
  unsigned int i = j; /* i is zero or a present index */
  j++;
  /* find `i' and `j' such that i is present and j is not */
  /*      ttisnil(luaH_getnum(t, j)) */
  while (!( luaH_getnum(t, j)->tt == LUA_TNIL) ) {
    i = j;
    j *= 2;
    /*  cast(unsigned int, MAX_INT)      */
    if (j > (unsigned int)(2147483647 -2) ) { /* overflow? */
      /* table was built with bad purposes: resort to linear search */
      i = 1;
      /*      ttisnil(luaH_getnum(t, i)) */
      while (!( luaH_getnum(t, i)->tt == LUA_TNIL) ) {
        i++;
      }
      return i - 1;
    }
  }
  /* now do a binary search between them */
  while (j - i > 1) {
    unsigned int m = (i+j)/2;
    /*  ttisnil(luaH_getnum(t, m)) */
    if ( luaH_getnum(t, m)->tt == LUA_TNIL ) {
      j = m;
    } else {
      i = m;
    }
  }
  return i;
}


/*
** Try to find a boundary in table `t'. A `boundary' is an integer index
** such that t[i] is non-nil and t[i+1] is nil (and 0 if t[1] is nil).
*/
int luaH_getn (Table *t) {
  unsigned int j = t->sizearray;
  /*            ttisnil(&t->array[j - 1]) */
  if (j > 0 && ((&t->array[j - 1])->tt == LUA_TNIL)) {
    /* there is a boundary in the array part: (binary) search for it */
    unsigned int i = 0;
    while (j - i > 1) {
      unsigned int m = (i+j)/2;
      /*   ttisnil(&t->array[m - 1]) */
      if ( (&t->array[m - 1])->tt == LUA_TNIL ) {
        j = m;
      } else {
        i = m;
      }
    }
    return i;
  }
  /* else must find a boundary in hash part */
  /*                   dummynode */
  else if (t->node == (&dummynode_)) { /* hash part is empty? */
    return j; /* that is easy... */
  }
  else {
    return unbound_search(t, j);
  }
}



#if defined(LUA_DEBUG)

Node *luaH_mainposition (const Table *t, const TValue *key) {
  return mainposition(t, key);
}

int luaH_isdummy (Node *n) { 
    return n == dummynode; 
}

#endif


