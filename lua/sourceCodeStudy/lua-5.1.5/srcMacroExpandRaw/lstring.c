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



void luaS_resize (lua_State *L, int newsize) {
  GCObject **newhash;
  stringtable *tb;
  int i;
  if ((L->l_G)->gcstate == 2) {
    return; /* cannot resize during GC traverse */
  }
  newhash = ((GCObject * *)(((((size_t)((newsize)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(GCObject *))) ? luaM_realloc_(L, (((void *)0)), (0)*(sizeof(GCObject *)), (newsize)*(sizeof(GCObject *))) : luaM_toobig(L))));
  tb = &(L->l_G)->strt;
  for (i=0; i<newsize; i++) {
    newhash[i] = ((void *)0);
  }
  /* rehash */
  for (i=0; i<tb->size; i++) {
    GCObject *p = tb->hash[i];
    while (p) { /* for each node in the list */
      GCObject *next = p->gch.next; /* save next */
      unsigned int h = (&(&((p)->ts))->tsv)->hash;
      int h1 = (((((int)((h) & ((newsize)-1)))))); /* new position */
      ((void)0);
      p->gch.next = newhash[h1]; /* chain it */
      newhash[h1] = p;
      p = next;
    }
  }
  ((((size_t)((0)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(TString *))) ? luaM_realloc_(L, ((tb->hash)), (tb->size)*(sizeof(TString *)), (0)*(sizeof(TString *))) : luaM_toobig(L));
  tb->size = newsize;
  tb->hash = newhash;
}


static TString *newlstr (lua_State *L, const char *str, size_t l,
                                       unsigned int h) {
  TString *ts;
  stringtable *tb;
  if (l+1 > (((size_t)(~(size_t)0)-2) - sizeof(TString))/sizeof(char)) {
    luaM_toobig(L);
  }
  ts = ((TString *)(luaM_realloc_(L, ((void *)0), 0, ((l+1)*sizeof(char)+sizeof(TString)))));
  ts->tsv.len = l;
  ts->tsv.hash = h;
  ts->tsv.marked = ((lu_byte)(((L->l_G))->currentwhite & ((1<<(0)) | (1<<(1)))));
  ts->tsv.tt = 4;
  ts->tsv.reserved = 0;
  memcpy(ts+1, str, l*sizeof(char));
  ((char *)(ts+1))[l] = '\0'; /* ending 0 */
  tb = &(L->l_G)->strt;
  h = (((((int)((h) & ((tb->size)-1))))));
  ts->tsv.next = tb->hash[h]; /* chain new entry */
  tb->hash[h] = (((GCObject *)((ts))));
  tb->nuse++;
  if (tb->nuse > ((lu_int32)(tb->size)) && tb->size <= (2147483647 -2)/2) {
    luaS_resize(L, tb->size*2); /* too crowded */
  }
  return ts;
}


TString *luaS_newlstr (lua_State *L, const char *str, size_t l) {
  GCObject *o;
  unsigned int h = ((unsigned int)(l)); /* seed */
  size_t step = (l>>5)+1; /* if string is too long, don't hash all its chars */
  size_t l1;
  for (l1=l; l1>=step; l1-=step) { /* compute hash */
    h = h ^ ((h<<5)+(h>>2)+((unsigned char)(str[l1-1])));
  }
  for (o = (L->l_G)->strt.hash[(((((int)((h) & (((L->l_G)->strt.size)-1))))))];
       o != ((void *)0);
       o = o->gch.next) {
    TString *ts = (&((o)->ts));
    if (ts->tsv.len == l && (memcmp(str, ((const char *)((ts) + 1)), l) == 0)) {
      /* string may be dead */
      if (((o)->gch.marked & ((L->l_G)->currentwhite ^ ((1<<(0)) | (1<<(1)))) & ((1<<(0)) | (1<<(1))))) {
        ((o)->gch.marked ^= ((1<<(0)) | (1<<(1))));
      }
      return ts;
    }
  }
  return newlstr(L, str, l, h); /* not found */
}


Udata *luaS_newudata (lua_State *L, size_t s, Table *e) {
  Udata *u;
  if (s > ((size_t)(~(size_t)0)-2) - sizeof(Udata)) {
    luaM_toobig(L);
  }
  u = ((Udata *)(luaM_realloc_(L, ((void *)0), 0, (s + sizeof(Udata)))));
  u->uv.marked = ((lu_byte)(((L->l_G))->currentwhite & ((1<<(0)) | (1<<(1))))); /* is not finalized */
  u->uv.tt = 7;
  u->uv.len = s;
  u->uv.metatable = ((void *)0);
  u->uv.env = e;
  /* chain it on udata list (after main thread) */
  u->uv.next = (L->l_G)->mainthread->next;
  (L->l_G)->mainthread->next = (((GCObject *)((u))));
  return u;
}


