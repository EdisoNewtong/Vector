/*
** $Id: lobject.c,v 2.22.1.1 2007/12/27 13:02:25 roberto Exp $
** Some generic functions over Lua objects
** See Copyright Notice in lua.h
*/

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define lobject_c
#define LUA_CORE

#include "lua.h"

#include "ldo.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "lvm.h"



const TValue luaO_nilobject_ = {{NULL}, LUA_TNIL};


/*
** converts an integer to a "floating point byte", represented as
** (eeeeexxx), where the real value is (1xxx) * 2^(eeeee - 1) if
** eeeee != 0 and (xxx) otherwise.
*/
int luaO_int2fb (unsigned int x) {
  int e = 0; /* expoent */
  while (x >= 16) {
    x = (x+1) >> 1;
    e++;
  }
  if (x < 8) {
    return x;
  } else {
    return ((e+1) << 3) | (((int)((x))) - 8);
  }
}


/* converts back */
int luaO_fb2int (int x) {
  int e = (x >> 3) & 31;
  if (e == 0) {
    return x;
  }
  else {
    return ((x & 7)+8) << (e - 1);
  }
}


int luaO_log2 (unsigned int x) {
  static const lu_byte log_2[256] = {
    0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
  };
  int l = -1;
  while (x >= 256) {
      l += 8;
      x >>= 8;
  }
  return l + log_2[x];
}


int luaO_rawequalObj (const TValue *t1, const TValue *t2) {
  if (((t1)->tt) != ((t2)->tt)) {
      return 0;
  } else {
    switch (((t1)->tt))
    {
      case 0: {
        return 1;
      }
      case 3: {
        return ((((t1)->value.n))==(((t2)->value.n)));
      }
      case 1: {
        return ((t1)->value.b) == ((t2)->value.b); /* boolean true must be 1 !! */
      }
      case 2: {
        return ((t1)->value.p) == ((t2)->value.p);
      }
      default: {
        ((void)0);
        return ((t1)->value.gc) == ((t2)->value.gc);
      }
    }
  }
}


int luaO_str2d (const char *s, lua_Number *result) {
  char *endptr;
  *result = strtod((s), (&endptr));
  if (endptr == s) {
    return 0; /* conversion failed */
  }
  if (*endptr == 'x' || *endptr == 'X') { /* maybe an hexadecimal constant? */
    *result = ((lua_Number)((strtoul(s, &endptr, 16))));
  }
  if (*endptr == '\0') {
    return 1; /* most common case */
  }
  while (((*__ctype_b_loc ())[(int) ((((unsigned char)(*endptr))))] & (unsigned short int) _ISspace)) {
    endptr++;
  }
  if (*endptr != '\0') {
    return 0; /* invalid trailing characters? */
  }
  return 1;
}


static void pushstr (lua_State *L, const char *str) {
  { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)(((luaS_newlstr(L, str, strlen(str)))))); i_o->tt=4; ((void)0); };
  {if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) luaD_growstack(L, 1); else ((void)0);; L->top++;};
}


/* this function handles only `%d', `%c', %f, %p, and `%s' formats */
const char *luaO_pushvfstring (lua_State *L, const char *fmt, va_list argp) {
  int n = 1;
  pushstr(L, "");
  for (;;) {
    const char *e = strchr(fmt, '%');
    if (e == ((void *)0)) {
      break;
    }
    { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((luaS_newlstr(L, fmt, e-fmt)))); i_o->tt=4; ((void)0); };
    {if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) luaD_growstack(L, 1); else ((void)0);; L->top++;};
    switch (*(e+1)) {
      case 's': {
        const char *s = __builtin_va_arg(argp,char *);
        if (s == ((void *)0)) {
          s = "(null)";
        }
        pushstr(L, s);
        break;
      }
      case 'c': {
        char buff[2];
        buff[0] = ((char)(__builtin_va_arg(argp,int)));
        buff[1] = '\0';
        pushstr(L, buff);
        break;
      }
      case 'd': {
        { TValue *i_o=(L->top); i_o->value.n=(((lua_Number)((__builtin_va_arg(argp,int))))); i_o->tt=3; };
        {if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) luaD_growstack(L, 1); else ((void)0);; L->top++;};
        break;
      }
      case 'f': {
        { TValue *i_o=(L->top); i_o->value.n=(((lua_Number)((__builtin_va_arg(argp,l_uacNumber))))); i_o->tt=3; };
        {if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) luaD_growstack(L, 1); else ((void)0);; L->top++;};
        break;
      }
      case 'p': {
        char buff[4*sizeof(void *) + 8]; /* should be enough space for a `%p' */
        sprintf(buff, "%p", __builtin_va_arg(argp,void *));
        pushstr(L, buff);
        break;
      }
      case '%': {
        pushstr(L, "%");
        break;
      }
      default: {
        char buff[3];
        buff[0] = '%';
        buff[1] = *(e+1);
        buff[2] = '\0';
        pushstr(L, buff);
        break;
      }
    }
    n += 2;
    fmt = e+2;
  }
  pushstr(L, fmt);
  luaV_concat(L, n+1, ((int)((L->top - L->base))) - 1);
  L->top -= n;
  return ((const char *)(((&(L->top - 1)->value.gc->ts)) + 1));
}


const char *luaO_pushfstring (lua_State *L, const char *fmt, ...) {
  const char *msg;
  va_list argp;
  __builtin_va_start(argp,fmt);
  msg = luaO_pushvfstring(L, fmt, argp);
  __builtin_va_end(argp);
  return msg;
}


void luaO_chunkid (char *out, const char *source, size_t bufflen) {
  if (*source == '=') {
    strncpy(out, source+1, bufflen); /* remove first char */
    out[bufflen-1] = '\0'; /* ensures null termination */
  }
  else { /* out = "source", or "...source" */
    if (*source == '@') {
      size_t l;
      source++; /* skip the `@' */
      bufflen -= sizeof(" '...' ");
      l = strlen(source);
      strcpy(out, "");
      if (l > bufflen) {
        source += (l-bufflen); /* get last part of file name */
        strcat(out, "...");
      }
      strcat(out, source);
    }
    else { /* out = [string "string"] */
      size_t len = strcspn(source, "\n\r"); /* stop at first newline */
      bufflen -= sizeof(" [string \"...\"] ");
      if (len > bufflen) {
        len = bufflen;
      }
      strcpy(out, "[string \"");
      if (source[len] != '\0') { /* must truncate? */
        strncat(out, source, len);
        strcat(out, "...");
      }
      else {
        strcat(out, source);
      }
      strcat(out, "\"]");
    }
  }
}


