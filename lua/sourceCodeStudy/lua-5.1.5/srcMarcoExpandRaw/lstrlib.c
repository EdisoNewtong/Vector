/*
** $Id: lstrlib.c,v 1.132.1.5 2010/05/14 15:34:19 roberto Exp $
** Standard library for string operations and pattern-matching
** See Copyright Notice in lua.h
*/


#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define lstrlib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


/* macro to `unsign' a character */
#define uchar(c)        ((unsigned char)(c))



static int str_len (lua_State *L) {
  size_t l;
  luaL_checklstring(L, 1, &l);
  lua_pushinteger(L, l);
  return 1;
}


static ptrdiff_t posrelat (ptrdiff_t pos, size_t len) {
  /* relative string position: negative means back from end */
  if (pos < 0) {
    pos += (ptrdiff_t)len + 1;
  }
  return (pos >= 0) ? pos : 0;
}


static int str_sub (lua_State *L) {
  size_t l;
  const char *s = luaL_checklstring(L, 1, &l);
  ptrdiff_t start = posrelat(luaL_checkinteger(L, 2), l);
  ptrdiff_t end = posrelat(luaL_optinteger(L, 3, -1), l);
  if (start < 1) {
    start = 1;
  }
  if (end > (ptrdiff_t)l) {
    end = (ptrdiff_t)l;
  }
  if (start <= end) {
    lua_pushlstring(L, s+start-1, end-start+1);
  } else {
    lua_pushlstring(L, "" "", (sizeof("")/sizeof(char))-1);
  }
  return 1;
}


static int str_reverse (lua_State *L) {
  size_t l;
  luaL_Buffer b;
  const char *s = luaL_checklstring(L, 1, &l);
  luaL_buffinit(L, &b);
  while (l--) {
    ((void)((&b)->p < ((&b)->buffer+8192) || luaL_prepbuffer(&b)), (*(&b)->p++ = (char)(s[l])));
  }
  luaL_pushresult(&b);
  return 1;
}


static int str_lower (lua_State *L) {
  size_t l;
  size_t i;
  luaL_Buffer b;
  const char *s = luaL_checklstring(L, 1, &l);
  luaL_buffinit(L, &b);
  for (i=0; i<l; i++) {
    ((void)((&b)->p < ((&b)->buffer+8192) || luaL_prepbuffer(&b)), (*(&b)->p++ = (char)(tolower(((unsigned char)(s[i]))))));
  }
  luaL_pushresult(&b);
  return 1;
}


static int str_upper (lua_State *L) {
  size_t l;
  size_t i;
  luaL_Buffer b;
  const char *s = luaL_checklstring(L, 1, &l);
  luaL_buffinit(L, &b);
  for (i=0; i<l; i++) {
    ((void)((&b)->p < ((&b)->buffer+8192) || luaL_prepbuffer(&b)), (*(&b)->p++ = (char)(toupper(((unsigned char)(s[i]))))));
  }
  luaL_pushresult(&b);
  return 1;
}


static int str_rep (lua_State *L) {
  size_t l;
  luaL_Buffer b;
  const char *s = luaL_checklstring(L, 1, &l);
  int n = ((int)luaL_checkinteger(L, (2)));
  luaL_buffinit(L, &b);
  while (n-- > 0) {
    luaL_addlstring(&b, s, l);
  }
  luaL_pushresult(&b);
  return 1;
}


static int str_byte (lua_State *L) {
  size_t l;
  const char *s = luaL_checklstring(L, 1, &l);
  ptrdiff_t posi = posrelat(luaL_optinteger(L, 2, 1), l);
  ptrdiff_t pose = posrelat(luaL_optinteger(L, 3, posi), l);
  int n, i;
  if (posi <= 0) {
    posi = 1;
  }
  if ((size_t)pose > l) {
    pose = l;
  }
  if (posi > pose) {
    return 0; /* empty interval; return no values */
  }
  n = (int)(pose - posi + 1);
  if (posi + n <= pose) { /* overflow? */
    luaL_error(L, "string slice too long");
  }
  luaL_checkstack(L, n, "string slice too long");
  for (i=0; i<n; i++) {
    lua_pushinteger(L, ((unsigned char)(s[posi+i-1])));
  }
  return n;
}


static int str_char (lua_State *L) {
  int n = lua_gettop(L); /* number of arguments */
  int i;
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  for (i=1; i<=n; i++) {
    int c = ((int)luaL_checkinteger(L, (i)));
    ((void)((((unsigned char)(c)) == c) || luaL_argerror(L, (i), ("invalid value"))));
    ((void)((&b)->p < ((&b)->buffer+8192) || luaL_prepbuffer(&b)), (*(&b)->p++ = (char)(((unsigned char)(c)))));
  }
  luaL_pushresult(&b);
  return 1;
}


static int writer (lua_State *L, const void* b, size_t size, void* B) {
  (void)L;
  luaL_addlstring((luaL_Buffer*) B, (const char *)b, size);
  return 0;
}

static int str_dump (lua_State *L) {
  luaL_Buffer b;
  luaL_checktype(L, 1, 6);
  lua_settop(L, 1);
  luaL_buffinit(L,&b);
  if (lua_dump(L, writer, &b) != 0) {
    luaL_error(L, "unable to dump given function");
  }
  luaL_pushresult(&b);
  return 1;
}


/*
** {======================================================
** PATTERN MATCHING
** =======================================================
*/
typedef struct MatchState {
  const char *src_init; /* init of source string */
  const char *src_end; /* end (`\0') of source string */
  lua_State *L;
  int level; /* total number of captures (finished or unfinished) */
  struct {
    const char *init;
    ptrdiff_t len;
  } capture[32];
} MatchState;


static int check_capture (MatchState *ms, int l) {
  l -= '1';
  if (l < 0 || l >= ms->level || ms->capture[l].len == (-1)) {
    return luaL_error(ms->L, "invalid capture index");
  }
  return l;
}


static int capture_to_close (MatchState *ms) {
  int level = ms->level;
  for (level--; level>=0; level--) {
    if (ms->capture[level].len == (-1)) {
      return level;
    }
  }
  return luaL_error(ms->L, "invalid pattern capture");
}


static const char *classend (MatchState *ms, const char *p) {
  switch (*p++) {
    case '%': {
      if (*p == '\0') {
        luaL_error(ms->L, "malformed pattern (ends with " "'" "%%" "'" ")");
      }
      return p+1;
    }
    case '[': {
      if (*p == '^') {
        p++;
      }
      do { /* look for a `]' */
        if (*p == '\0') {
          luaL_error(ms->L, "malformed pattern (missing " "'" "]" "'" ")");
        }
        if (*(p++) == '%' && *p != '\0') {
          p++; /* skip escapes (e.g. `%]') */
        }
      } while (*p != ']');
      return p+1;
    }
    default: {
      return p;
    }
  }
}


static int match_class (int c, int cl) {
  int res;
  switch (tolower(cl)) {
    case 'a' : {
      res = ((*__ctype_b_loc ())[(int) ((c))] & (unsigned short int) _ISalpha);
      break;
    }
    case 'c' : {
      res = ((*__ctype_b_loc ())[(int) ((c))] & (unsigned short int) _IScntrl);
      break;
    }
    case 'd' : {
      res = ((*__ctype_b_loc ())[(int) ((c))] & (unsigned short int) _ISdigit);
      break;
    }
    case 'l' : {
      res = ((*__ctype_b_loc ())[(int) ((c))] & (unsigned short int) _ISlower);
      break;
    }
    case 'p' : {
      res = ((*__ctype_b_loc ())[(int) ((c))] & (unsigned short int) _ISpunct);
      break;
    }
    case 's' : {
      res = ((*__ctype_b_loc ())[(int) ((c))] & (unsigned short int) _ISspace);
      break;
    }
    case 'u' : {
      res = ((*__ctype_b_loc ())[(int) ((c))] & (unsigned short int) _ISupper);
      break;
    }
    case 'w' : {
      res = ((*__ctype_b_loc ())[(int) ((c))] & (unsigned short int) _ISalnum);
      break;
    }
    case 'x' : {
      res = ((*__ctype_b_loc ())[(int) ((c))] & (unsigned short int) _ISxdigit);
      break;
    }
    case 'z' : {
      res = (c == 0);
      break;
    }
    default: {
      return (cl == c);
    }
  }
  return (((*__ctype_b_loc ())[(int) ((cl))] & (unsigned short int) _ISlower) ? res : !res);
}


static int matchbracketclass (int c, const char *p, const char *ec) {
  int sig = 1;
  if (*(p+1) == '^') {
    sig = 0;
    p++; /* skip the `^' */
  }
  while (++p < ec) {
    if (*p == '%') {
      p++;
      if (match_class(c, ((unsigned char)(*p)))) {
        return sig;
      }
    }
    else if ((*(p+1) == '-') && (p+2 < ec)) {
      p+=2;
      if (((unsigned char)(*(p-2))) <= c && c <= ((unsigned char)(*p))) {
        return sig;
      }
    }
    else if (((unsigned char)(*p)) == c) {
      return sig;
    }
  }
  return !sig;
}


static int singlematch (int c, const char *p, const char *ep) {
  switch (*p) {
    case '.': {
      return 1; /* matches any char */
    }
    case '%': {
      return match_class(c, ((unsigned char)(*(p+1))));
    }
    case '[': {
      return matchbracketclass(c, p, ep-1);
    }
    default: {
      return (((unsigned char)(*p)) == c);
    }
  }
}


static const char *match (MatchState *ms, const char *s, const char *p);


static const char *matchbalance (MatchState *ms, const char *s,
                                   const char *p) {
  if (*p == 0 || *(p+1) == 0) {
    luaL_error(ms->L, "unbalanced pattern");
  }
  if (*s != *p) {
    return ((void *)0);
  }
  else {
    int b = *p;
    int e = *(p+1);
    int cont = 1;
    while (++s < ms->src_end) {
      if (*s == e) {
        if (--cont == 0) {
          return s+1;
        }
      }
      else if (*s == b) {
        cont++;
      }
    }
  }
  return ((void *)0); /* string ends out of balance */
}


static const char *max_expand (MatchState *ms, const char *s,
                                 const char *p, const char *ep) {
  ptrdiff_t i = 0; /* counts maximum expand for item */
  while ((s+i)<ms->src_end && singlematch(((unsigned char)(*(s+i))), p, ep)) {
    i++;
  }
  /* keeps trying to match with the maximum repetitions */
  while (i>=0) {
    const char *res = match(ms, (s+i), ep+1);
    if (res) {
      return res;
    }
    i--; /* else didn't match; reduce 1 repetition to try again */
  }
  return ((void *)0);
}


static const char *min_expand (MatchState *ms, const char *s,
                                 const char *p, const char *ep) {
  for (;;) {
    const char *res = match(ms, s, ep+1);
    if (res != ((void *)0)) {
      return res;
    } else if (s<ms->src_end && singlematch(((unsigned char)(*s)), p, ep)) {
      s++; /* try with one more repetition */
    }
    else {
      return ((void *)0);
    }
  }
}


static const char *start_capture (MatchState *ms, const char *s,
                                    const char *p, int what) {
  const char *res;
  int level = ms->level;
  if (level >= 32) {
    luaL_error(ms->L, "too many captures");
  }
  ms->capture[level].init = s;
  ms->capture[level].len = what;
  ms->level = level+1;
  if ((res=match(ms, s, p)) == ((void *)0)) { /* match failed? */
    ms->level--; /* undo capture */
  }
  return res;
}


static const char *end_capture (MatchState *ms, const char *s,
                                  const char *p) {
  int l = capture_to_close(ms);
  const char *res;
  ms->capture[l].len = s - ms->capture[l].init; /* close capture */
  if ((res = match(ms, s, p)) == ((void *)0)) { /* match failed? */
    ms->capture[l].len = (-1); /* undo capture */
  }
  return res;
}


static const char *match_capture (MatchState *ms, const char *s, int l) {
  size_t len;
  l = check_capture(ms, l);
  len = ms->capture[l].len;
  if ((size_t)(ms->src_end-s) >= len &&
      memcmp(ms->capture[l].init, s, len) == 0)
  {
    return s+len;
  }
  else {
    return ((void *)0);
  }
}


static const char *match (MatchState *ms, const char *s, const char *p) {
  init: /* using goto's to optimize tail recursion */
  switch (*p) {
    case '(': { /* start capture */
      if (*(p+1) == ')'){ /* position capture? */
        return start_capture(ms, s, p+2, (-2));
      } else {
        return start_capture(ms, s, p+1, (-1));
      }
    }
    case ')': { /* end capture */
      return end_capture(ms, s, p+1);
    }
    case '%': {
      switch (*(p+1)) {
        case 'b': { /* balanced string? */
          s = matchbalance(ms, s, p+2);
          if (s == ((void *)0)) {
            return ((void *)0);
          }
          p+=4;
          goto init; /* else return match(ms, s, p+4); */
        }
        case 'f': { /* frontier? */
          const char *ep; char previous;
          p += 2;
          if (*p != '[') {
            luaL_error(ms->L, "missing " "'" "[" "'" " after "
                               "'" "%%f" "'" " in pattern");
          }
          ep = classend(ms, p); /* points to what is next */
          previous = (s == ms->src_init) ? '\0' : *(s-1);
          if (matchbracketclass(((unsigned char)(previous)), p, ep-1) ||
             !matchbracketclass(((unsigned char)(*s)), p, ep-1))
          {
            return ((void *)0);
          }
          p=ep;
          goto init; /* else return match(ms, s, ep); */
        }
        default: {
          if (((*__ctype_b_loc ())[(int) ((((unsigned char)(*(p+1)))))] & (unsigned short int) _ISdigit)) { /* capture results (%0-%9)? */
            s = match_capture(ms, s, ((unsigned char)(*(p+1))));
            if (s == ((void *)0)) {
              return ((void *)0);
            }
            p+=2;
            goto init; /* else return match(ms, s, p+2) */
          }
          goto dflt; /* case default */
        }
      }
    }
    case '\0': { /* end of pattern */
      return s; /* match succeeded */
    }
    case '$': {
      if (*(p+1) == '\0') { /* is the `$' the last char in pattern? */
        return (s == ms->src_end) ? s : ((void *)0); /* check end of string */
      }
      else {
        goto dflt;
      }
    }
    default: dflt: { /* it is a pattern item */
      const char *ep = classend(ms, p); /* points to what is next */
      int m = s<ms->src_end && singlematch(((unsigned char)(*s)), p, ep);
      switch (*ep) {
        case '?': { /* optional */
          const char *res;
          if (m && ((res=match(ms, s+1, ep+1)) != ((void *)0))) {
            return res;
          }
          p=ep+1;
          goto init; /* else return match(ms, s, ep+1); */
        }
        case '*': { /* 0 or more repetitions */
          return max_expand(ms, s, p, ep);
        }
        case '+': { /* 1 or more repetitions */
          return (m ? max_expand(ms, s+1, p, ep) : ((void *)0));
        }
        case '-': { /* 0 or more repetitions (minimum) */
          return min_expand(ms, s, p, ep);
        }
        default: {
          if (!m) {
            return ((void *)0);
          }
          s++;
          p=ep;
          goto init; /* else return match(ms, s+1, ep); */
        }
      }
    }
  }
}


static const char *lmemfind (const char *s1, size_t l1,
                               const char *s2, size_t l2) {
  if (l2 == 0) {
    return s1; /* empty strings are everywhere */
  } else if (l2 > l1) {
     return ((void *)0); /* avoids a negative `l1' */
  } else {
    const char *init; /* to search for a `*s2' inside `s1' */
    l2--; /* 1st char will be checked by `memchr' */
    l1 = l1-l2; /* `s2' cannot be found after that */
    while (l1 > 0 && (init = (const char *)memchr(s1, *s2, l1)) != ((void *)0)) {
      init++; /* 1st char is already checked */
      if (memcmp(init, s2+1, l2) == 0) {
        return init-1;
      } else { /* correct `l1' and `s1' to try again */
        l1 -= init-s1;
        s1 = init;
      }
    }
    return ((void *)0); /* not found */
  }
}


static void push_onecapture (MatchState *ms, int i, const char *s,
                                                    const char *e) {
  if (i >= ms->level) {
    if (i == 0) { /* ms->level == 0, too */
      lua_pushlstring(ms->L, s, e - s); /* add whole match */
    } else {
      luaL_error(ms->L, "invalid capture index");
    }
  }
  else {
    ptrdiff_t l = ms->capture[i].len;
    if (l == (-1)) {
      luaL_error(ms->L, "unfinished capture");
    }
    if (l == (-2)) {
      lua_pushinteger(ms->L, ms->capture[i].init - ms->src_init + 1);
    } else {
      lua_pushlstring(ms->L, ms->capture[i].init, l);
    }
  }
}


static int push_captures (MatchState *ms, const char *s, const char *e) {
  int i;
  int nlevels = (ms->level == 0 && s) ? 1 : ms->level;
  luaL_checkstack(ms->L, nlevels, "too many captures");
  for (i = 0; i < nlevels; i++) {
    push_onecapture(ms, i, s, e);
  }
  return nlevels; /* number of strings pushed */
}


static int str_find_aux (lua_State *L, int find) {
  size_t l1, l2;
  const char *s = luaL_checklstring(L, 1, &l1);
  const char *p = luaL_checklstring(L, 2, &l2);
  ptrdiff_t init = posrelat(luaL_optinteger(L, 3, 1), l1) - 1;
  if (init < 0) {
    init = 0;
  }
  else if ((size_t)(init) > l1) {
    init = (ptrdiff_t)l1;
  }
  if (find && (lua_toboolean(L, 4) || /* explicit request? */
      strpbrk(p, "^$*+?.([%-") == ((void *)0))) { /* or no special characters? */
    /* do a plain search */
    const char *s2 = lmemfind(s+init, l1-init, p, l2);
    if (s2) {
      lua_pushinteger(L, s2-s+1);
      lua_pushinteger(L, s2-s+l2);
      return 2;
    }
  }
  else {
    MatchState ms;
    int anchor = (*p == '^') ? (p++, 1) : 0;
    const char *s1=s+init;
    ms.L = L;
    ms.src_init = s;
    ms.src_end = s+l1;
    do {
      const char *res;
      ms.level = 0;
      if ((res=match(&ms, s1, p)) != ((void *)0)) {
        if (find) {
          lua_pushinteger(L, s1-s+1); /* start */
          lua_pushinteger(L, res-s); /* end */
          return push_captures(&ms, ((void *)0), 0) + 2;
        }
        else {
          return push_captures(&ms, s1, res);
        }
      }
    } while (s1++ < ms.src_end && !anchor);
  }
  lua_pushnil(L); /* not found */
  return 1;
}


static int str_find (lua_State *L) {
  return str_find_aux(L, 1);
}


static int str_match (lua_State *L) {
  return str_find_aux(L, 0);
}


static int gmatch_aux (lua_State *L) {
  MatchState ms;
  size_t ls;
  const char *s = lua_tolstring(L, ((-10002)-(1)), &ls);
  const char *p = lua_tolstring(L, (((-10002)-(2))), ((void *)0));
  const char *src;
  ms.L = L;
  ms.src_init = s;
  ms.src_end = s+ls;
  for (src = s + (size_t)lua_tointeger(L, ((-10002)-(3)));
       src <= ms.src_end;
       src++) {
    const char *e;
    ms.level = 0;
    if ((e = match(&ms, src, p)) != ((void *)0)) {
      lua_Integer newstart = e-s;
      if (e == src) {
        newstart++; /* empty match? go at least one position */
      }
      lua_pushinteger(L, newstart);
      lua_replace(L, ((-10002)-(3)));
      return push_captures(&ms, src, e);
    }
  }
  return 0; /* not found */
}


static int gmatch (lua_State *L) {
  (luaL_checklstring(L, (1), ((void *)0)));
  (luaL_checklstring(L, (2), ((void *)0)));
  lua_settop(L, 2);
  lua_pushinteger(L, 0);
  lua_pushcclosure(L, gmatch_aux, 3);
  return 1;
}


static int gfind_nodef (lua_State *L) {
  return luaL_error(L, "'" "string.gfind" "'" " was renamed to "
                       "'" "string.gmatch" "'");
}


static void add_s (MatchState *ms, luaL_Buffer *b, const char *s,
                                                   const char *e) {
  size_t l, i;
  const char *news = lua_tolstring(ms->L, 3, &l);
  for (i = 0; i < l; i++) {
    if (news[i] != '%') {
      ((void)((b)->p < ((b)->buffer+8192) || luaL_prepbuffer(b)), (*(b)->p++ = (char)(news[i])));
    } else {
      i++; /* skip ESC */
      if (!((*__ctype_b_loc ())[(int) ((((unsigned char)(news[i]))))] & (unsigned short int) _ISdigit)) {
        ((void)((b)->p < ((b)->buffer+8192) || luaL_prepbuffer(b)), (*(b)->p++ = (char)(news[i])));
      } else if (news[i] == '0') {
          luaL_addlstring(b, s, e - s);
      } else {
        push_onecapture(ms, news[i] - '1', s, e);
        luaL_addvalue(b); /* add capture to accumulated result */
      }
    }
  }
}


static void add_value (MatchState *ms, luaL_Buffer *b, const char *s,
                                                       const char *e) {
  lua_State *L = ms->L;
  switch (lua_type(L, 3)) {
    case 3:
    case 4: {
      add_s(ms, b, s, e);
      return;
    }
    case 6: {
      int n;
      lua_pushvalue(L, 3);
      n = push_captures(ms, s, e);
      lua_call(L, n, 1);
      break;
    }
    case 5: {
      push_onecapture(ms, 0, s, e);
      lua_gettable(L, 3);
      break;
    }
  }
  if (!lua_toboolean(L, -1)) { /* nil or false? */
    lua_settop(L, -(1)-1);
    lua_pushlstring(L, s, e - s); /* keep original text */
  }
  else if (!lua_isstring(L, -1)) {
    luaL_error(L, "invalid replacement value (a %s)", lua_typename(L, lua_type(L,(-1))));
  }
  luaL_addvalue(b); /* add result to accumulator */
}


static int str_gsub (lua_State *L) {
  size_t srcl;
  const char *src = luaL_checklstring(L, 1, &srcl);
  const char *p = (luaL_checklstring(L, (2), ((void *)0)));
  int tr = lua_type(L, 3);
  int max_s = ((int)luaL_optinteger(L, (4), (srcl+1)));
  int anchor = (*p == '^') ? (p++, 1) : 0;
  int n = 0;
  MatchState ms;
  luaL_Buffer b;
  ((void)((tr == 3 || tr == 4 || tr == 6 || tr == 5) || luaL_argerror(L, (3), ("string/function/table expected"))));
  luaL_buffinit(L, &b);
  ms.L = L;
  ms.src_init = src;
  ms.src_end = src+srcl;
  while (n < max_s) {
    const char *e;
    ms.level = 0;
    e = match(&ms, src, p);
    if (e) {
      n++;
      add_value(&ms, &b, src, e);
    }
    if (e && e>src) { /* non empty match? */
      src = e; /* skip it */
    } else if (src < ms.src_end) {
      ((void)((&b)->p < ((&b)->buffer+8192) || luaL_prepbuffer(&b)), (*(&b)->p++ = (char)(*src++)));
    } else {
      break;
    }
    if (anchor) {
      break;
    }
  }
  luaL_addlstring(&b, src, ms.src_end-src);
  luaL_pushresult(&b);
  lua_pushinteger(L, n); /* number of substitutions */
  return 2;
}


/* }====================================================== */
/* maximum size of each formatted item (> len(format('%99.99f', -1e308))) */
/* valid flags in a format specification */
/*
** maximum size of each format specification (such as '%-099.99d')
** (+10 accounts for %99.99x plus margin of error)
*/
static void addquoted (lua_State *L, luaL_Buffer *b, int arg) {
  size_t l;
  const char *s = luaL_checklstring(L, arg, &l);
  ((void)((b)->p < ((b)->buffer+8192) || luaL_prepbuffer(b)), (*(b)->p++ = (char)('"')));
  while (l--) {
    switch (*s) {
      case '"':
      case '\\':
      case '\n': {
        ((void)((b)->p < ((b)->buffer+8192) || luaL_prepbuffer(b)), (*(b)->p++ = (char)('\\')));
        ((void)((b)->p < ((b)->buffer+8192) || luaL_prepbuffer(b)), (*(b)->p++ = (char)(*s)));
        break;
      }
      case '\r': {
        luaL_addlstring(b, "\\r", 2);
        break;
      }
      case '\0': {
        luaL_addlstring(b, "\\000", 4);
        break;
      }
      default: {
        ((void)((b)->p < ((b)->buffer+8192) || luaL_prepbuffer(b)), (*(b)->p++ = (char)(*s)));
        break;
      }
    }
    s++;
  }
  ((void)((b)->p < ((b)->buffer+8192) || luaL_prepbuffer(b)), (*(b)->p++ = (char)('"')));
}


static const char *scanformat (lua_State *L, const char *strfrmt, char *form) {
  const char *p = strfrmt;
  while (*p != '\0' && strchr("-+ #0", *p) != ((void *)0)) {
    p++; /* skip flags */
  }
  if ((size_t)(p - strfrmt) >= sizeof("-+ #0")) {
    luaL_error(L, "invalid format (repeated flags)");
  }
  if (((*__ctype_b_loc ())[(int) ((((unsigned char)(*p))))] & (unsigned short int) _ISdigit)) {
    p++; /* skip width */
  }
  if (((*__ctype_b_loc ())[(int) ((((unsigned char)(*p))))] & (unsigned short int) _ISdigit)) {
    p++; /* (2 digits at most) */
  }
  if (*p == '.') {
    p++;
    if (((*__ctype_b_loc ())[(int) ((((unsigned char)(*p))))] & (unsigned short int) _ISdigit)) {
      p++; /* skip precision */
    }
    if (((*__ctype_b_loc ())[(int) ((((unsigned char)(*p))))] & (unsigned short int) _ISdigit)) {
      p++; /* (2 digits at most) */
    }
  }
  if (((*__ctype_b_loc ())[(int) ((((unsigned char)(*p))))] & (unsigned short int) _ISdigit)) {
    luaL_error(L, "invalid format (width or precision too long)");
  }
  *(form++) = '%';
  strncpy(form, strfrmt, p - strfrmt + 1);
  form += p - strfrmt + 1;
  *form = '\0';
  return p;
}


static void addintlen (char *form) {
  size_t l = strlen(form);
  char spec = form[l - 1];
  strcpy(form + l - 1, "l");
  form[l + sizeof("l") - 2] = spec;
  form[l + sizeof("l") - 1] = '\0';
}


static int str_format (lua_State *L) {
  int top = lua_gettop(L);
  int arg = 1;
  size_t sfl;
  const char *strfrmt = luaL_checklstring(L, arg, &sfl);
  const char *strfrmt_end = strfrmt+sfl;
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  while (strfrmt < strfrmt_end) {
    if (*strfrmt != '%') {
      ((void)((&b)->p < ((&b)->buffer+8192) || luaL_prepbuffer(&b)), (*(&b)->p++ = (char)(*strfrmt++)));
    } else if (*++strfrmt == '%') {
      ((void)((&b)->p < ((&b)->buffer+8192) || luaL_prepbuffer(&b)), (*(&b)->p++ = (char)(*strfrmt++))); /* %% */
    } else { /* format item */
      char form[(sizeof("-+ #0") + sizeof("l") + 10)]; /* to store the format (`%...') */
      char buff[512]; /* to store the formatted item */
      if (++arg > top) {
        luaL_argerror(L, arg, "no value");
      }
      strfrmt = scanformat(L, strfrmt, form);
      switch (*strfrmt++) {
        case 'c': {
          sprintf(buff, form, (int)luaL_checknumber(L, arg));
          break;
        }
        case 'd':
        case 'i': {
          addintlen(form);
          sprintf(buff, form, (long)luaL_checknumber(L, arg));
          break;
        }
        case 'o':
        case 'u':
        case 'x':
        case 'X': {
          addintlen(form);
          sprintf(buff, form, (unsigned long)luaL_checknumber(L, arg));
          break;
        }
        case 'e':
        case 'E':
        case 'f':
        case 'g':
        case 'G': {
          sprintf(buff, form, (double)luaL_checknumber(L, arg));
          break;
        }
        case 'q': {
          addquoted(L, &b, arg);
          continue; /* skip the 'addsize' at the end */
        }
        case 's': {
          size_t l;
          const char *s = luaL_checklstring(L, arg, &l);
          if (!strchr(form, '.') && l >= 100) {
            /* no precision and string is too long to be formatted;
               keep original string */
            lua_pushvalue(L, arg);
            luaL_addvalue(&b);
            continue; /* skip the `addsize' at the end */
          }
          else {
            sprintf(buff, form, s);
            break;
          }
        }
        default: { /* also treat cases `pnLlh' */
          return luaL_error(L, "invalid option " "'" "%%%c" "'" " to "
                               "'" "format" "'", *(strfrmt - 1));
        }
      }
      luaL_addlstring(&b, buff, strlen(buff));
    }
  }
  luaL_pushresult(&b);
  return 1;
}


static const luaL_Reg strlib[] = {
  {"byte", str_byte},
  {"char", str_char},
  {"dump", str_dump},
  {"find", str_find},
  {"format", str_format},
  {"gfind", gfind_nodef},
  {"gmatch", gmatch},
  {"gsub", str_gsub},
  {"len", str_len},
  {"lower", str_lower},
  {"match", str_match},
  {"rep", str_rep},
  {"reverse", str_reverse},
  {"sub", str_sub},
  {"upper", str_upper},
  {((void *)0), ((void *)0)}
};


static void createmetatable (lua_State *L) {
  lua_createtable(L, 0, 1); /* create metatable for strings */
  lua_pushlstring(L, "" "", (sizeof("")/sizeof(char))-1); /* dummy string */
  lua_pushvalue(L, -2);
  lua_setmetatable(L, -2); /* set string metatable */
  lua_settop(L, -(1)-1); /* pop dummy string */
  lua_pushvalue(L, -2); /* string library... */
  lua_setfield(L, -2, "__index"); /* ...is the __index metamethod */
  lua_settop(L, -(1)-1); /* pop metatable */
}


/*
** Open string library
*/
extern int luaopen_string (lua_State *L) {
  luaL_register(L, "string", strlib);
  lua_getfield(L, -1, "gmatch");
  lua_setfield(L, -2, "gfind");
  createmetatable(L);
  return 1;
}

