/*
** $Id: lzio.c,v 1.31.1.1 2007/12/27 13:02:25 roberto Exp $
** a generic input stream interface
** See Copyright Notice in lua.h
*/


#include <string.h>

#define lzio_c
#define LUA_CORE

#include "lua.h"

#include "llimits.h"
#include "lmem.h"
#include "lstate.h"
#include "lzio.h"


int luaZ_fill (ZIO *z) {
  size_t size;
  lua_State *L = z->L;
  const char *buff;
  /* lua_unlock(L); */
  ((void) 0);
  buff = z->reader(L, z->data, &size);
  /* lua_lock(L); */
  ((void) 0);
  if (buff == NULL || size == 0) {
    /*     (-1) EOZ */
    return EOZ;
  }
  z->n = size - 1;
  z->p = buff;
  /*      char2int(*(z->p++)) */
  return (int)( (unsigned char)(*(z->p++)) );
}


int luaZ_lookahead (ZIO *z) {
  if (z->n == 0) {
    /*                  (-1)  EOZ */
    if ( luaZ_fill(z) == EOZ ) {
      /*     (-1) EOZ  */
      return EOZ;
    } else {
      z->n++; /* luaZ_fill removed first byte; put back it */
      z->p--;
    }
  }
  /*      char2int(*z->p); */
  return (int)( (unsigned char)(*z->p) );
}


void luaZ_init (lua_State *L, ZIO *z, lua_Reader reader, void *data) {
  z->L = L;
  z->reader = reader;
  z->data = data;
  z->n = 0;
  z->p = NULL;
}


/* --------------------------------------------------------------- read --- */
size_t luaZ_read (ZIO *z, void *b, size_t n) {
  while (n) {
    size_t m;
    /*                       (-1)  EOZ */
    if (luaZ_lookahead(z) == EOZ ) {
      return n; /* return number of missing bytes */
    }
    m = (n <= z->n) ? n : z->n; /* min. between n and z->n */
    memcpy(b, z->p, m);
    z->n -= m;
    z->p += m;
    b = (char *)b + m;
    n -= m;
  }
  return 0;
}


/* ------------------------------------------------------------------------ */
char *luaZ_openspace (lua_State *L, Mbuffer *buff, size_t n) {
  if (n > buff->buffsize) {
    /*              32    LUA_MINBUFFER */
    if (n < LUA_MINBUFFER) {
      /*            32   LUA_MINBUFFER */
      n = LUA_MINBUFFER;
    }
    /* luaZ_resizebuffer(L, buff, n); */
    (  (buff->buffer = (char *)(( (size_t)(n+1) <= MAX_SIZET/sizeof(char) ) 
                                ? luaM_realloc_(L, buff->buffer, buff->buffsize * sizeof(char), n * sizeof(char) ) 
                                : luaM_toobig(L) )
       )
	   , buff->buffsize = n
	);
  }
  return buff->buffer;
}


