/*
** $Id: llex.c,v 2.20.1.2 2009/11/23 14:58:22 roberto Exp $
** Lexical Analyzer
** See Copyright Notice in lua.h
*/


#include <ctype.h>
#include <locale.h>
#include <string.h>

#define llex_c
#define LUA_CORE

#include "lua.h"

#include "ldo.h"
#include "llex.h"
#include "lobject.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lzio.h"



#define next(ls) (ls->current = zgetc(ls->z))




#define currIsNewline(ls)	(ls->current == '\n' || ls->current == '\r')


/* ORDER RESERVED */
const char *const luaX_tokens [] = {
    "and", "break", "do", "else", "elseif",
    "end", "false", "for", "function", "if",
    "in", "local", "nil", "not", "or", "repeat",
    "return", "then", "true", "until", "while",
    "..", "...", "==", ">=", "<=", "~=",
    "<number>", "<name>", "<string>", "<eof>",
    ((void *)0)
};


static void save (LexState *ls, int c) {
  Mbuffer *b = ls->buff;
  if (b->n + 1 > b->buffsize) {
    size_t newsize;
    if (b->buffsize >= ((size_t)(~(size_t)0)-2)/2) {
      luaX_lexerror(ls, "lexical element too long", 0);
    }
    newsize = b->buffsize * 2;
    ((((b)->buffer)=((char *)(((((size_t)((newsize)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(char))) ? luaM_realloc_(ls->L, ((b)->buffer), ((b)->buffsize)*(sizeof(char)), (newsize)*(sizeof(char))) : luaM_toobig(ls->L))))), (b)->buffsize = newsize);
  }
  b->buffer[b->n++] = ((char)(c));
}


void luaX_init (lua_State *L) {
  int i;
  for (i=0; i<(((int)(TK_WHILE-257 +1))); i++) {
    TString *ts = (luaS_newlstr(L, luaX_tokens[i], strlen(luaX_tokens[i])));
    (((ts)->tsv.marked) |= ((1<<(5)))); /* reserved words are never collected */
    ((void)0);
    ts->tsv.reserved = ((lu_byte)((i+1))); /* reserved word */
  }
}


const char *luaX_token2str (LexState *ls, int token) {
  if (token < 257) {
    ((void)0);
    return (((*__ctype_b_loc ())[(int) ((token))] & (unsigned short int) _IScntrl)) ? luaO_pushfstring(ls->L, "char(%d)", token) :
                              luaO_pushfstring(ls->L, "%c", token);
  }
  else {
    return luaX_tokens[token-257];
  }
}


static const char *txtToken (LexState *ls, int token) {
  switch (token) {
    case TK_NAME:
    case TK_STRING:
    case TK_NUMBER: {
      save(ls, '\0');
      return ((ls->buff)->buffer);
    }
    default: {
      return luaX_token2str(ls, token);
    }
  }
}


void luaX_lexerror (LexState *ls, const char *msg, int token) {
  char buff[80];
  luaO_chunkid(buff, ((const char *)((ls->source) + 1)), 80);
  msg = luaO_pushfstring(ls->L, "%s:%d: %s", buff, ls->linenumber, msg);
  if (token) {
    luaO_pushfstring(ls->L, "%s near " "'" "%s" "'", msg, txtToken(ls, token));
  }
  luaD_throw(ls->L, 3);
}


void luaX_syntaxerror (LexState *ls, const char *msg) {
  luaX_lexerror(ls, msg, ls->t.token);
}


TString *luaX_newstring (LexState *ls, const char *str, size_t l) {
  lua_State *L = ls->L;
  TString *ts = luaS_newlstr(L, str, l);
  TValue *o = luaH_setstr(L, ls->fs->h, ts); /* entry for `str' */
  if ((((o)->tt) == 0)) {
    { TValue *i_o=(o); i_o->value.b=(1); i_o->tt=1; }; /* make sure `str' will not be collected */
    { ((void)0); if ((L->l_G)->totalbytes >= (L->l_G)->GCthreshold) luaC_step(L); };
  }
  return ts;
}


static void inclinenumber (LexState *ls) {
  int old = ls->current;
  ((void)0);
  (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z))); /* skip `\n' or `\r' */
  if ((ls->current == '\n' || ls->current == '\r') && ls->current != old) {
    (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z))); /* skip `\n\r' or `\r\n' */
  }
  if (++ls->linenumber >= (2147483647 -2)) {
    luaX_syntaxerror(ls, "chunk has too many lines");
  }
}


void luaX_setinput (lua_State *L, LexState *ls, ZIO *z, TString *source) {
  ls->decpoint = '.';
  ls->L = L;
  ls->lookahead.token = TK_EOS; /* no look-ahead token */
  ls->z = z;
  ls->fs = ((void *)0);
  ls->linenumber = 1;
  ls->lastline = 1;
  ls->source = source;
  ((((ls->buff)->buffer)=((char *)(((((size_t)((32)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(char))) ? luaM_realloc_(ls->L, ((ls->buff)->buffer), ((ls->buff)->buffsize)*(sizeof(char)), (32)*(sizeof(char))) : luaM_toobig(ls->L))))), (ls->buff)->buffsize = 32); /* initialize buffer */
  (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z))); /* read first char */
}


/*
** =======================================================
** LEXICAL ANALYZER
** =======================================================
*/
static int check_next (LexState *ls, const char *set) {
  if (!strchr(set, ls->current)) {
    return 0;
  }
  (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z))));
  return 1;
}


static void buffreplace (LexState *ls, char from, char to) {
  size_t n = ((ls->buff)->n);
  char *p = ((ls->buff)->buffer);
  while (n--) {
    if (p[n] == from) {
      p[n] = to;
    }
  }
}


static void trydecpoint (LexState *ls, SemInfo *seminfo) {
  /* format error: try to update decimal point separator */
  struct lconv *cv = localeconv();
  char old = ls->decpoint;
  ls->decpoint = (cv ? cv->decimal_point[0] : '.');
  buffreplace(ls, old, ls->decpoint); /* try updated decimal separator */
  if (!luaO_str2d(((ls->buff)->buffer), &seminfo->r)) {
    /* format error with correct decimal point: no more options */
    buffreplace(ls, ls->decpoint, '.'); /* undo change (for error message) */
    luaX_lexerror(ls, "malformed number", TK_NUMBER);
  }
}


/* LUA_NUMBER */
static void read_numeral (LexState *ls, SemInfo *seminfo) {
  ((void)0);
  do {
    (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z))));
  } while (((*__ctype_b_loc ())[(int) ((ls->current))] & (unsigned short int) _ISdigit) || ls->current == '.');
  if (check_next(ls, "Ee")) { /* `E'? */
    check_next(ls, "+-"); /* optional exponent sign */
  }
  while (((*__ctype_b_loc ())[(int) ((ls->current))] & (unsigned short int) _ISalnum) || ls->current == '_') {
    (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z))));
  }
  save(ls, '\0');
  buffreplace(ls, '.', ls->decpoint); /* follow locale for decimal point */
  if (!luaO_str2d(((ls->buff)->buffer), &seminfo->r)) { /* format error? */
    trydecpoint(ls, seminfo); /* try to update decimal point separator */
  }
}


static int skip_sep (LexState *ls) {
  int count = 0;
  int s = ls->current;
  ((void)0);
  (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z))));
  while (ls->current == '=') {
    (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z))));
    count++;
  }
  return (ls->current == s) ? count : (-count) - 1;
}


static void read_long_string (LexState *ls, SemInfo *seminfo, int sep) {
  int cont = 0;
  (void)(cont); /* avoid warnings when `cont' is not used */
  (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)))); /* skip 2nd `[' */
  if ((ls->current == '\n' || ls->current == '\r')) { /* string starts with a newline? */
    inclinenumber(ls); /* skip it */
  }
  for (;;) {
    switch (ls->current) {
      case (-1): {
        luaX_lexerror(ls, (seminfo) ? "unfinished long string" :
                                   "unfinished long comment", TK_EOS);
        break; /* to avoid warnings */
      }
      case '[': {
        if (skip_sep(ls) == sep) {
          (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)))); /* skip 2nd `[' */
          cont++;
          if (sep == 0) {
            luaX_lexerror(ls, "nesting of [[...]] is deprecated", '[');
          }
        }
        break;
      }
      case ']': {
        if (skip_sep(ls) == sep) {
          (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)))); /* skip 2nd `]' */
          goto endloop;
        }
        break;
      }
      case '\n':
      case '\r': {
        save(ls, '\n');
        inclinenumber(ls);
        if (!seminfo) {
          ((ls->buff)->n = 0); /* avoid wasting space */
        }
        break;
      }
      default: {
        if (seminfo) {
          (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z))));
        } else {
          (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
        }
      }
    }
  } endloop:
  if (seminfo) {
    seminfo->ts = luaX_newstring(ls, ((ls->buff)->buffer) + (2 + sep),
                                     ((ls->buff)->n) - 2*(2 + sep));
  }
}


static void read_string (LexState *ls, int del, SemInfo *seminfo) {
  (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z))));
  while (ls->current != del) {
    switch (ls->current) {
      case (-1): {
        luaX_lexerror(ls, "unfinished string", TK_EOS);
        continue; /* to avoid warnings */
      }
      case '\n':
      case '\r': {
        luaX_lexerror(ls, "unfinished string", TK_STRING);
        continue; /* to avoid warnings */
      }
      case '\\': {
        int c;
        (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z))); /* do not save the `\' */
        switch (ls->current) {
          case 'a': {
            c = '\a';
            break;
          }
          case 'b': {
            c = '\b';
            break;
          }
          case 'f': {
            c = '\f';
            break;
          }
          case 'n': {
            c = '\n';
            break;
          }
          case 'r': {
            c = '\r';
            break;
          }
          case 't': {
            c = '\t';
            break;
          }
          case 'v': {
            c = '\v';
            break;
          }
          case '\n': /* go through */
          case '\r': {
             save(ls, '\n');
             inclinenumber(ls);
             continue;
          }
          case (-1): {
             continue; /* will raise an error next loop */
          }
          default: {
            if (!((*__ctype_b_loc ())[(int) ((ls->current))] & (unsigned short int) _ISdigit)) {
              (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)))); /* handles \\, \", \', and \? */
            } else { /* \xxx */
              int i = 0;
              c = 0;
              do {
                c = 10*c + (ls->current-'0');
                (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
              } while (++i<3 && ((*__ctype_b_loc ())[(int) ((ls->current))] & (unsigned short int) _ISdigit));
              if (c > (127 * 2 + 1)) {
                luaX_lexerror(ls, "escape sequence too large", TK_STRING);
              }
              save(ls, c);
            }
            continue;
          }
        }
        save(ls, c);
        (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
        continue;
      }
      default: {
        (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z))));
      }
    }
  }
  (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)))); /* skip delimiter */
  seminfo->ts = luaX_newstring(ls, ((ls->buff)->buffer) + 1,
                                   ((ls->buff)->n) - 2);
}


static int llex (LexState *ls, SemInfo *seminfo) {
  ((ls->buff)->n = 0);
  for (;;) {
    switch (ls->current) {
      case '\n':
      case '\r': {
        inclinenumber(ls);
        continue;
      }
      case '-': {
        (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
        if (ls->current != '-') {
          return '-';
        }
        /* else is a comment */
        (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
        if (ls->current == '[') {
          int sep = skip_sep(ls);
          ((ls->buff)->n = 0); /* `skip_sep' may dirty the buffer */
          if (sep >= 0) {
            read_long_string(ls, ((void *)0), sep); /* long comment */
            ((ls->buff)->n = 0);
            continue;
          }
        }
        /* else short comment */
        while (!(ls->current == '\n' || ls->current == '\r') && ls->current != (-1)) {
          (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
        }
        continue;
      }
      case '[': {
        int sep = skip_sep(ls);
        if (sep >= 0) {
          read_long_string(ls, seminfo, sep);
          return TK_STRING;
        }
        else if (sep == -1) {
          return '[';
        }
        else {
          luaX_lexerror(ls, "invalid long string delimiter", TK_STRING);
        }
      }
      case '=': {
        (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
        if (ls->current != '=') {
          return '=';
        } else {
          (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
          return TK_EQ;
        }
      }
      case '<': {
        (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
        if (ls->current != '=') {
          return '<';
        } else {
          (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
          return TK_LE;
        }
      }
      case '>': {
        (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
        if (ls->current != '=') {
          return '>';
        } else {
          (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
          return TK_GE;
        }
      }
      case '~': {
        (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
        if (ls->current != '=') {
          return '~';
        } else {
          (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
          return TK_NE;
        }
      }
      case '"':
      case '\'': {
        read_string(ls, ls->current, seminfo);
        return TK_STRING;
      }
      case '.': {
        (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z))));
        if (check_next(ls, ".")) {
          if (check_next(ls, ".")) {
            return TK_DOTS; /* ... */
          } else {
            return TK_CONCAT; /* .. */
          }
        }
        else if (!((*__ctype_b_loc ())[(int) ((ls->current))] & (unsigned short int) _ISdigit)) {
          return '.';
        }
        else {
          read_numeral(ls, seminfo);
          return TK_NUMBER;
        }
      }
      case (-1): {
        return TK_EOS;
      }
      default: {
        if (((*__ctype_b_loc ())[(int) ((ls->current))] & (unsigned short int) _ISspace)) {
          ((void)0);
          (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
          continue;
        }
        else if (((*__ctype_b_loc ())[(int) ((ls->current))] & (unsigned short int) _ISdigit)) {
          read_numeral(ls, seminfo);
          return TK_NUMBER;
        }
        else if (((*__ctype_b_loc ())[(int) ((ls->current))] & (unsigned short int) _ISalpha) || ls->current == '_') {
          /* identifier or reserved word */
          TString *ts;
          do {
            (save(ls, ls->current), (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z))));
          } while (((*__ctype_b_loc ())[(int) ((ls->current))] & (unsigned short int) _ISalnum) || ls->current == '_');
          ts = luaX_newstring(ls, ((ls->buff)->buffer),
                                  ((ls->buff)->n));
          if (ts->tsv.reserved > 0) { /* reserved word? */
            return ts->tsv.reserved - 1 + 257;
          } else {
            seminfo->ts = ts;
            return TK_NAME;
          }
        }
        else {
          int c = ls->current;
          (ls->current = (((ls->z)->n--)>0 ? ((int)(((unsigned char)((*(ls->z)->p++))))) : luaZ_fill(ls->z)));
          return c; /* single-char tokens (+ - / ...) */
        }
      }
    }
  }
}


void luaX_next (LexState *ls) {
  ls->lastline = ls->linenumber;
  if (ls->lookahead.token != TK_EOS) { /* is there a look-ahead token? */
    ls->t = ls->lookahead; /* use this one */
    ls->lookahead.token = TK_EOS; /* and discharge it */
  }
  else {
    ls->t.token = llex(ls, &ls->t.seminfo); /* read next token */
  }
}


void luaX_lookahead (LexState *ls) {
  ((void)0);
  ls->lookahead.token = llex(ls, &ls->lookahead.seminfo);
}


