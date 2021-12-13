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
    NULL
};


#define save_and_next(ls) (save(ls, ls->current), next(ls))


static void save (LexState *ls, int c) {
  Mbuffer *b = ls->buff;
  if (b->n + 1 > b->buffsize) {
    size_t newsize;
    /*                 ((size_t)(~(size_t)0)-2)   MAX_SIZET */
    if (b->buffsize >= MAX_SIZET/2) {
      luaX_lexerror(ls, "lexical element too long", 0);
    }
    newsize = b->buffsize * 2;
    /* 
       #define luaZ_resizebuffer(L, buff, size) \
               (luaM_reallocvector(L, (buff)->buffer, (buff)->buffsize, size, char), \
               (buff)->buffsize = size)

       luaZ_resizebuffer(ls->L, b, newsize);
    */
      ( b->buffer = (char *)( (size_t)(newsize+1) <= MAX_SIZET/sizeof(char)
                             ? luaM_realloc_(ls->L, b->buffer, b->buffsize * sizeof(char), newsize * sizeof(char) ) 
                             : luaM_toobig(ls->L) ) ) 
        , b->buffsize = newsize;
  }

  b->buffer[b->n++] = cast(char, c);
}


void luaX_init (lua_State *L) {
  int i;
  /*          (((int)(TK_WHILE-257 +1)))  NUM_RESERVED */
  for (i=0; i<NUM_RESERVED; i++) {
    /*             luaS_new(L, luaX_tokens[i]); */
    TString *ts = luaS_newlstr(L, luaX_tokens[i], strlen(luaX_tokens[i]));

    /* luaS_fix(ts); */
    (((ts)->tsv.marked) |= ((1<<(5)))); /* reserved words are never collected */

    /* lua_assert(strlen(luaX_tokens[i])+1 <= TOKEN_LEN); */
    ((void)0);
    /*                 (lu_byte)(i+1);  cast_byte(i+1); */
    ts->tsv.reserved = cast_byte(i+1); /* reserved word */
  }
}


#define MAXSRC          80


const char *luaX_token2str (LexState *ls, int token) {
  /*          257    FIRST_RESERVED */
  if (token < FIRST_RESERVED) {
    /* lua_assert(token == cast(unsigned char, token)); */
    ((void)0);

    /*     (((*__ctype_b_loc ())[(int) ((token))] & (unsigned short int) _IScntrl))    iscntrl(token) */
    return iscntrl(token) ? luaO_pushfstring(ls->L, "char(%d)", token) :
                            luaO_pushfstring(ls->L, "%c", token);
  }
  else {
    /*                       257  FIRST_RESERVED */
    return luaX_tokens[token-FIRST_RESERVED];
  }
}


static const char *txtToken (LexState *ls, int token) {
  switch (token) {
    case TK_NAME:
    case TK_STRING:
    case TK_NUMBER: {
      save(ls, '\0');
      /*     luaZ_buffer(ls->buff); */
      return ls->buff->buffer;
    }
    default: {
      return luaX_token2str(ls, token);
    }
  }
}


void luaX_lexerror (LexState *ls, const char *msg, int token) {
  /*        80      MAXSRC  */
  char buff[MAXSRC];
  /*                 ((const char *)((ls->source) + 1))  getstr(ls->source)  */
  luaO_chunkid(buff, getstr(ls->source), MAXSRC);
  msg = luaO_pushfstring(ls->L, "%s:%d: %s", buff, ls->linenumber, msg);
  if (token) {
    luaO_pushfstring(ls->L, "%s near " "'" "%s" "'", msg, txtToken(ls, token));
  }
  /*                3  LUA_ERRSYNTAX  */
  luaD_throw(ls->L, LUA_ERRSYNTAX);
}


void luaX_syntaxerror (LexState *ls, const char *msg) {
  luaX_lexerror(ls, msg, ls->t.token);
}


TString *luaX_newstring (LexState *ls, const char *str, size_t l) {
  lua_State *L = ls->L;
  TString *ts = luaS_newlstr(L, str, l);
  TValue *o = luaH_setstr(L, ls->fs->h, ts); /* entry for `str' */
  /* (((o)->tt) == 0)  ttisnil(o) */
  if ( ttisnil(o) ) {
    /* setbvalue(o, 1); */
    { TValue *i_o=(o); i_o->value.b=(1); i_o->tt=1; }; /* make sure `str' will not be collected */

    /* luaC_checkGC(L); */
    { ((void)0); 
      if ( L->l_G->totalbytes >= L->l_G->GCthreshold ) { 
          luaC_step(L); 
      }
    };
  }
  return ts;
}


static void inclinenumber (LexState *ls) {
  int old = ls->current;
  /* lua_assert(currIsNewline(ls)); */
  ((void)0);

  /* next(ls); */
  ls->current = ((ls->z)->n--)>0 ? (int)( (unsigned char)(*(ls->z)->p++) ) : luaZ_fill(ls->z); /* skip `\n' or `\r' */
  /*   (ls->current == '\n' || ls->current == '\r')   currIsNewline(ls)      */
  if ( (ls->current == '\n' || ls->current == '\r') && ls->current != old) {
    ls->current = ((ls->z)->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z); /* skip `\n\r' or `\r\n' */
  }
  /*                     (2147483647 -2)   MAX_INT */
  if (++ls->linenumber >= MAX_INT) {
    luaX_syntaxerror(ls, "chunk has too many lines");
  }
}


void luaX_setinput (lua_State *L, LexState *ls, ZIO *z, TString *source) {
  ls->decpoint = '.';
  ls->L = L;
  ls->lookahead.token = TK_EOS; /* no look-ahead token */
  ls->z = z;
  ls->fs = NULL;
  ls->linenumber = 1;
  ls->lastline = 1;
  ls->source = source;


    /* 
       #define luaZ_resizebuffer(L, buff, size) \
               (luaM_reallocvector(L, (buff)->buffer, (buff)->buffsize, size, char), \
               (buff)->buffsize = size)

       #define LUA_MINBUFFER   32

       luaZ_resizebuffer(ls->L, ls->buff, LUA_MINBUFFER);
    */
  ( ls->buff->buffer = ( 
                        (char *)( 
                                 (size_t)(LUA_MINBUFFER+1) <= MAX_SIZET/sizeof(char) 
                                 ? luaM_realloc_(ls->L, ls->buff->buffer, ls->buff->buffsize * sizeof(char), LUA_MINBUFFER* sizeof(char) ) 
                                 : luaM_toobig(ls->L) 
                                )  
                       ) 
  ) , ls->buff->buffsize = 32; /* initialize buffer */

  /* next(ls); */
  ls->current =  ls->z->n-- > 0 
                 ? (int)( (unsigned char)(*(ls->z)->p++) ) 
                 : luaZ_fill(ls->z); /* read first char */
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
  /* save_and_next(ls); */
  save(ls, ls->current)
  , (
      ls->current = ( ls->z->n-- > 0 
                     ? (int)( (unsigned char)(*(ls->z)->p++) ) 
                     : luaZ_fill(ls->z) ) 
    );
  return 1;
}


static void buffreplace (LexState *ls, char from, char to) {
  /*          luaZ_bufflen(ls->buff); */
  size_t n = ls->buff->n;
  /*        luaZ_buffer(ls->buff); */
  char *p = ls->buff->buffer;

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
  /* lua_assert(isdigit(ls->current)); */
  ((void)0);

  do {
      /* save_and_next(ls); */
      save(ls, ls->current) ,
      (
         ls->current = ( ls->z->n-- > 0 
                        ? (int)( (unsigned char)(*(ls->z)->p++) ) 
                        : luaZ_fill(ls->z) ) 
      );
  } while ( isdigit(ls->current)  || ls->current == '.');
  if (check_next(ls, "Ee")) { /* `E'? */
    check_next(ls, "+-"); /* optional exponent sign */
  }
  while ( isalnum(ls->current) || ls->current == '_') {
    /* save_and_next(ls); */
    save(ls, ls->current)
    , (
        ls->current = ( ls->z->n-- > 0 
                       ? (int)( (unsigned char)(*(ls->z)->p++) ) 
                       : luaZ_fill(ls->z) ) 
      );
  }
  save(ls, '\0');
  buffreplace(ls, '.', ls->decpoint); /* follow locale for decimal point */

  /*               luaZ_buffer(ls->buff) */
  if (!luaO_str2d( ls->buff->buffer, &seminfo->r)) { /* format error? */
    trydecpoint(ls, seminfo); /* try to update decimal point separator */
  }
}


static int skip_sep (LexState *ls) {
  int count = 0;
  int s = ls->current;

  /* lua_assert(s == '[' || s == ']'); */
  ((void)0);


  /* save_and_next(ls); */
  save(ls, ls->current) ,
  (
    ls->current = ls->z->n-- > 0 
				    ? (int)( (unsigned char)(*(ls->z)->p++) ) 
				   : luaZ_fill(ls->z)
  );

  while (ls->current == '=') {
    /* save_and_next(ls); */
    save(ls, ls->current) ,
    (
      ls->current = ls->z->n-- > 0 
      			    ? (int)( (unsigned char)(*(ls->z)->p++) ) 
      			   : luaZ_fill(ls->z)
    );

    count++;
  }
  return (ls->current == s) ? count : (-count) - 1;
}


static void read_long_string (LexState *ls, SemInfo *seminfo, int sep) {
  int cont = 0;
  (void)(cont); /* avoid warnings when `cont' is not used */
  /* save_and_next(ls); */
  save(ls, ls->current) ,
  (
    ls->current = ls->z->n-- > 0 
    			    ? (int)( (unsigned char)(*(ls->z)->p++) ) 
    			   : luaZ_fill(ls->z)
  );

  /*   currIsNewline(ls) */
  if ( ls->current == '\n' || ls->current == '\r' ) { /* string starts with a newline? */
    inclinenumber(ls); /* skip it */
  }
  for (;;) {
    switch (ls->current) {
      /* EOZ  (-1) */
      case EOZ: {
        luaX_lexerror(ls, seminfo ? "unfinished long string" :
                                   "unfinished long comment", TK_EOS);
        break; /* to avoid warnings */
      }
#if defined(LUA_COMPAT_LSTR)
      case '[': {
        if (skip_sep(ls) == sep) {
          /* save_and_next(ls); */   /* skip 2nd `[' */
          save(ls, ls->current) ,
          (
            ls->current = ls->z->n-- > 0 
            			    ? (int)( (unsigned char)(*(ls->z)->p++) ) 
            			   : luaZ_fill(ls->z)
          );
          cont++;

          #if LUA_COMPAT_LSTR == 1
                    if (sep == 0) {
                      luaX_lexerror(ls, "nesting of [[...]] is deprecated", '[');
                    }
          #endif

        }
        break;
      }
#endif

      case ']': {
        if (skip_sep(ls) == sep) {
          /* save_and_next(ls); */  /* skip 2nd `]' */
          save(ls, ls->current) ,
          (
            ls->current = ls->z->n-- > 0 
            			    ? (int)( (unsigned char)(*(ls->z)->p++) ) 
            			   : luaZ_fill(ls->z)
          );

          /*
          #if defined(LUA_COMPAT_LSTR) && LUA_COMPAT_LSTR == 2
                    cont--;
                    if (sep == 0 && cont >= 0) { 
                      break;
                    }
          #endif
          */
          goto endloop;
        }
        break;
      }
      case '\n':
      case '\r': {
        save(ls, '\n');
        inclinenumber(ls);
        if (!seminfo) {
          /* luaZ_resetbuffer(ls->buff); */
          ls->buff->n = 0; /* avoid wasting space */
        }
        break;
      }
      default: {
        if (seminfo) {
          /* save_and_next(ls); */
          save(ls, ls->current) ,
          (
            ls->current = ls->z->n-- > 0 
            			    ? (int)( (unsigned char)(*(ls->z)->p++) ) 
            			   : luaZ_fill(ls->z)
          );
        } else {
          /* next(ls); */
          ls->current =    ls->z->n-->0 
			             ? (int)( (unsigned char)(*(ls->z)->p++) ) 
						 : luaZ_fill(ls->z);
        }
      }
    }
  } 
endloop:
  if (seminfo) {
    /*                               luaZ_buffer(ls->buff) */
    seminfo->ts = luaX_newstring(ls, ls->buff->buffer + (2 + sep),
    /*                               luaZ_bufflen(ls->buff) */
                                     ls->buff->n - 2*(2 + sep));
  }
}


static void read_string (LexState *ls, int del, SemInfo *seminfo) {
  /* 
   *
		#define save_and_next(ls) (save(ls, ls->current), next(ls))                       // llex.c:48
		#define next(ls) (ls->current = zgetc(ls->z))                                     // llex.c:28
		#define zgetc(z)  (((z)->n--)>0 ?  char2int(*(z)->p++) : luaZ_fill(z))            // lzio.h:22
		#define char2int(c)	cast(int, cast(unsigned char, (c)))                           // lzio.h:20

         save_and_next(ls);
  */
  (   save(ls, ls->current), 
      ( ls->current = (  (ls->z->n--)>0 ? ( (int)( (unsigned char)(*(ls->z)->p++) )) : luaZ_fill(ls->z) ) )
  );
  while (ls->current != del) {
    switch (ls->current) {
      /* #define EOZ	(-1) 		/ * end of stream     */
      case EOZ: {
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

		/* 
		 *
		  	#define next(ls) (ls->current = zgetc(ls->z))                                     // llex.c:28
		  	#define zgetc(z)  (((z)->n--)>0 ?  char2int(*(z)->p++) : luaZ_fill(z))            // lzio.h:22
		  	#define char2int(c)	cast(int, cast(unsigned char, (c)))                           // lzio.h:20

            next(ls);  / *   do not save the `\'    * /
		*/
        ls->current = (ls->z->n--)>0 ? ((int)((unsigned char)(*(ls->z)->p++))) : luaZ_fill(ls->z); /* do not save the `\' */
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
			 /* 
			  *
			 	#define next(ls) (ls->current = zgetc(ls->z))                                     // llex.c:28
			 	#define zgetc(z)  (((z)->n--)>0 ?  char2int(*(z)->p++) : luaZ_fill(z))            // lzio.h:22
			 	#define char2int(c)	cast(int, cast(unsigned char, (c)))                           // lzio.h:20

			 	next(ls);  / *   do not save the `\'         *  /
			 */
             save(ls, '\n');
             inclinenumber(ls);
             continue;
          }
          /* #define EOZ	(-1) 		/ * end of stream     */
          case EOZ: {
             continue; /* will raise an error next loop */
          }
          default: {
            if ( !isdigit(ls->current) ) {
              /* 
               *
                	#define save_and_next(ls) (save(ls, ls->current), next(ls))                       // llex.c:48
                	#define next(ls) (ls->current = zgetc(ls->z))                                     // llex.c:28
                	#define zgetc(z)  (((z)->n--)>0 ?  char2int(*(z)->p++) : luaZ_fill(z))            // lzio.h:22
                	#define char2int(c)	cast(int, cast(unsigned char, (c)))                           // lzio.h:20


                    save_and_next(ls);
              */
              (save(ls, ls->current), 
			   ( ls->current = (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z) ) ); /* handles \\, \", \', and \? */
            } else { /* \xxx */
              int i = 0;
              c = 0;
              do {
                c = 10*c + (ls->current-'0');
                /* next(ls); */
                ls->current = ( (ls->z->n--)>0 ? (int)((unsigned char)((*(ls->z)->p++))) : luaZ_fill(ls->z) );
              } while ( ++i<3 && isdigit(ls->current) );
              /*      UCHAR_MAX  (127 * 2 + 1)  */
              if (c > UCHAR_MAX ) {
                luaX_lexerror(ls, "escape sequence too large", TK_STRING);
              }
              save(ls, c);
            }
            continue;
          }
        }
        save(ls, c);
		/* next(ls); */
        ls->current = (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z);
        continue;
      }
      default: {
        /* 
         *
          	#define save_and_next(ls) (save(ls, ls->current), next(ls))                       // llex.c:48
          	#define next(ls) (ls->current = zgetc(ls->z))                                     // llex.c:28
          	#define zgetc(z)  (((z)->n--)>0 ?  char2int(*(z)->p++) : luaZ_fill(z))            // lzio.h:22
          	#define char2int(c)	cast(int, cast(unsigned char, (c)))                           // lzio.h:20


             save_and_next(ls);
        */
        ( save(ls, ls->current), 
		  (ls->current = ( (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z) )) 
        );
      }
    }
  }

  /* 
   *
  	#define save_and_next(ls) (save(ls, ls->current), next(ls))                       // llex.c:48
  	#define next(ls) (ls->current = zgetc(ls->z))                                     // llex.c:28
  	#define zgetc(z)  (((z)->n--)>0 ?  char2int(*(z)->p++) : luaZ_fill(z))            // lzio.h:22
  	#define char2int(c)	cast(int, cast(unsigned char, (c)))                           // lzio.h:20
  
  
     save_and_next(ls);
  */
  ( save(ls, ls->current), 
    (ls->current = (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z))
  ); /* skip delimiter */

  /*                                luaZ_buffer(ls->buff) */
  seminfo->ts = luaX_newstring(ls, ((ls->buff)->buffer) + 1,
                                   ((ls->buff)->n) - 2);
}

static int llex (LexState *ls, SemInfo *seminfo) {
  /* luaZ_resetbuffer(ls->buff); */
  ls->buff->n = 0;
  for (;;) {
    switch (ls->current) {
      case '\n':
      case '\r': {
        inclinenumber(ls);
        continue;
      }
      case '-': {
        /* next(ls); */
        ls->current = ( (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z) );
        if (ls->current != '-') {
          return '-';
        }
        /* else is a comment */
        /* next(ls); */
        ls->current = ( (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z) );
        if (ls->current == '[') {
          int sep = skip_sep(ls);
		  /* luaZ_resetbuffer(ls->buff); */
          ls->buff->n = 0; /* `skip_sep' may dirty the buffer */
          if (sep >= 0) {
            read_long_string(ls, NULL, sep); /* long comment */
            /* luaZ_resetbuffer(ls->buff); */
            ls->buff->n = 0;
            continue;
          }
        }
        /* else short comment */
        /*     !currIsNewline(ls)                            && ls->current != EOZ */
        while (!(ls->current == '\n' || ls->current == '\r') && ls->current != (-1) ) {
          /* next(ls); */
          ls->current = (ls->z->n--)>0 ? (int)( (unsigned char)(*(ls->z)->p++) ) : luaZ_fill(ls->z);
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
		/* next(ls); */
        ls->current = (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z);
        if (ls->current != '=') {
          return '=';
        } else {
          /* next(ls); */
          ls->current = (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z);
          return TK_EQ;
        }
      }
      case '<': {
        /* next(ls); */
        ls->current = (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z);
        if (ls->current != '=') {
          return '<';
        } else {
          /* next(ls); */
          ls->current = (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z);
          return TK_LE;
        }
      }
      case '>': {
        /* next(ls); */
        ls->current = (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z);
        if (ls->current != '=') {
          return '>';
        } else {
          /* next(ls);  */
          ls->current = (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z);
          return TK_GE;
        }
      }
      case '~': {
        /* next(ls); */
        ls->current = (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z);
        if (ls->current != '=') {
          return '~';
        } else {
          /* next(ls);  */
          ls->current = (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z);
          return TK_NE;
        }
      }
      case '"':
      case '\'': {
        read_string(ls, ls->current, seminfo);
        return TK_STRING;
      }
      case '.': {
        /* 
         *
          	#define save_and_next(ls) (save(ls, ls->current), next(ls))                       // llex.c:48
          	#define next(ls) (ls->current = zgetc(ls->z))                                     // llex.c:28
          	#define zgetc(z)  (((z)->n--)>0 ?  char2int(*(z)->p++) : luaZ_fill(z))            // lzio.h:22
          	#define char2int(c)	cast(int, cast(unsigned char, (c)))                           // lzio.h:20

               save_and_next(ls);
        */
        ( save(ls, ls->current), 
		  (ls->current =  (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z))
        );
        if (check_next(ls, ".")) {
          if (check_next(ls, ".")) {
            return TK_DOTS; /* ... */
          } else {
            return TK_CONCAT; /* .. */
          }
        }
        else if ( !isdigit(ls->current) ) {
          return '.';
        }
        else {
          read_numeral(ls, seminfo);
          return TK_NUMBER;
        }
      }
      /* #define EOZ	(-1) 		/ * end of stream     */
      case EOZ: {
        return TK_EOS;
      }
      default: {
        if ( isspace(ls->current) ) {
          /* lua_assert(!currIsNewline(ls)); */
          ((void)0);
          /* next(ls); */
          ls->current = (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z);
          continue;
        }
        else if ( isdigit(ls->current) ) {
          read_numeral(ls, seminfo);
          return TK_NUMBER;
        }
        else if ( isalpha(ls->current) || ls->current == '_' ) {
          /* identifier or reserved word */
          TString *ts;
          do {

              /* 
               *
                	#define save_and_next(ls) (save(ls, ls->current), next(ls))                       // llex.c:48
                	#define next(ls) (ls->current = zgetc(ls->z))                                     // llex.c:28
                	#define zgetc(z)  (((z)->n--)>0 ?  char2int(*(z)->p++) : luaZ_fill(z))            // lzio.h:22
                	#define char2int(c)	cast(int, cast(unsigned char, (c)))                           // lzio.h:20

                     save_and_next(ls);
              */
              ( save(ls, ls->current), 
		        (ls->current =  (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z))
              );
          } while ( isalnum(ls->current) || ls->current == '_' );

          /*                      luaZ_buffer(ls->buff)  */
          ts = luaX_newstring(ls, ((ls->buff)->buffer),
          /*                      luaZ_bufflen(ls->buff) */
                                  ((ls->buff)->n));
          if (ts->tsv.reserved > 0) { /* reserved word? */
            /*                            257             FIRST_RESERVED */
            return ts->tsv.reserved - 1 + FIRST_RESERVED;
          } else {
            seminfo->ts = ts;
            return TK_NAME;
          }
        }
        else {
          int c = ls->current;
          /* next(ls); */
          ls->current = (ls->z->n--)>0 ? (int)((unsigned char)(*(ls->z)->p++)) : luaZ_fill(ls->z);
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
  /* lua_assert(ls->lookahead.token == TK_EOS); */
  ((void)0);
  ls->lookahead.token = llex(ls, &ls->lookahead.seminfo);
}


