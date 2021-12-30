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
  ((void) 0);
  buff = z->reader(L, z->data, &size);
  ((void) 0);
  if (buff == ((void *)0) || size == 0) {
    return (-1);
  }
  z->n = size - 1;
  z->p = buff;
  return ((int)(((unsigned char)((*(z->p++))))));
}


int luaZ_lookahead (ZIO *z) {
  if (z->n == 0) {
    if (luaZ_fill(z) == (-1)) {
      return (-1);
    } else {
      z->n++; /* luaZ_fill removed first byte; put back it */
      z->p--;
    }
  }
  return ((int)(((unsigned char)((*z->p)))));
}


void luaZ_init (lua_State *L, ZIO *z, lua_Reader reader, void *data) {
  z->L = L;
  z->reader = reader;
  z->data = data;
  z->n = 0;
  z->p = ((void *)0);
}


/* --------------------------------------------------------------- read --- */
size_t luaZ_read (ZIO *z, void *b, size_t n) {
  while (n) {
    size_t m;
    if (luaZ_lookahead(z) == (-1)) {
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
    if (n < 32) {
      n = 32;
    }
    ((((buff)->buffer)=((char *)(((((size_t)((n)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(char))) ? luaM_realloc_(L, ((buff)->buffer), ((buff)->buffsize)*(sizeof(char)), (n)*(sizeof(char))) : luaM_toobig(L))))), (buff)->buffsize = n);
  }
  return buff->buffer;
}


