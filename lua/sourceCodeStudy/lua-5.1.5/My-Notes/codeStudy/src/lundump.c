/*
** $Id: lundump.c,v 2.7.1.4 2008/04/04 19:51:41 roberto Exp $
** load precompiled Lua chunks
** See Copyright Notice in lua.h
*/

#include <string.h>

#define lundump_c
#define LUA_CORE

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstring.h"
#include "lundump.h"
#include "lzio.h"

typedef struct {
  lua_State* L;
  ZIO* Z;
  Mbuffer* b;
  const char* name;
} LoadState;

#ifdef LUAC_TRUST_BINARIES
    #define IF(c,s)
    #define error(S,s)
#else
    #define IF(c,s)		if (c) error(S,s)

    static void error(LoadState* S, const char* why)
    {
      luaO_pushfstring(S->L,"%s: %s in precompiled chunk",S->name,why);
      luaD_throw(S->L,LUA_ERRSYNTAX);
    }
#endif

#define LoadMem(S,b,n,size)	LoadBlock(S,b,(n)*(size))
#define	LoadByte(S)		(lu_byte)LoadChar(S)
#define LoadVar(S,x)		LoadMem(S,&x,1,sizeof(x))
#define LoadVector(S,b,n,size)	LoadMem(S,b,n,size)

static void LoadBlock(LoadState* S, void* b, size_t size)
{
  size_t r = luaZ_read(S->Z,b,size);
  /* IF (r!=0, "unexpected end"); */
  if (r!=0) { 
    error(S,"unexpected end");
  }
}


static int LoadChar(LoadState* S)
{
  char x;
  /* LoadVar(S,x); */
  LoadBlock(S, &x, 1 * sizeof(x) );
  return x;
}


static int LoadInt(LoadState* S)
{
  int x;
  /* LoadVar(S,x); */
  LoadBlock(S, &x, 1 * sizeof(x) );
  /* IF (x<0, "bad integer"); */
  if (x<0) { 
    error(S,"bad integer");
  }
  return x;
}


static lua_Number LoadNumber(LoadState* S)
{
  lua_Number x;
  /* LoadVar(S,x); */
  LoadBlock(S, &x, 1 *sizeof(x) );
  return x;
}


static TString* LoadString(LoadState* S)
{
  size_t size;
  /* LoadVar(S,size); */
  LoadBlock(S, &size, 1 * sizeof(size) );
  if (size==0) {
    return NULL;
  } else {
    char* s=luaZ_openspace(S->L,S->b,size);
    LoadBlock(S,s,size);
    return luaS_newlstr(S->L,s,size-1); /* remove trailing '\0' */
  }
}


static void LoadCode(LoadState* S, Proto* f)
{
  int n=LoadInt(S);
  /*        luaM_newvector(S->L,n,Instruction); */
  f->code = (Instruction *)(  ( (size_t)(n+1) <= MAX_SIZET/sizeof(Instruction) ) 
                              ? luaM_realloc_(S->L, NULL, 0 * sizeof(Instruction), n * sizeof(Instruction) ) 
                              : luaM_toobig(S->L)
                           );
  f->sizecode=n;
  /* LoadVector(S,f->code,n,sizeof(Instruction)); */
  LoadBlock(S,f->code, n * sizeof(Instruction) );
}


static Proto* LoadFunction(LoadState* S, TString* p);

static void LoadConstants(LoadState* S, Proto* f)
{
  int i,n;
  n = LoadInt(S);
  /*      luaM_newvector(S->L,n,TValue); */
  f->k = (TValue *)(    ( (size_t)(n+1) <= MAX_SIZET/sizeof(TValue) ) 
                      ? luaM_realloc_(S->L, NULL, 0 * sizeof(TValue), n * sizeof(TValue) ) 
                      : luaM_toobig(S->L)
                    );
  f->sizek = n;
  for (i=0; i<n; i++) {
    /* setnilvalue(&f->k[i]); */
    (&f->k[i])->tt = LUA_TNIL;
  }

  for (i=0; i<n; i++)
  {
    TValue* o=&f->k[i];
    int t=LoadChar(S);
    switch (t)
    {
      case LUA_TNIL: { /* 0 */
        /* setnilvalue(o); */
        o->tt = LUA_TNIL;
        break;
      }
      case LUA_TBOOLEAN: { /* 1 */
        /* setbvalue(o,LoadChar(S)!=0); */
        { 
            TValue *i_o = o; 
            i_o->value.b = (LoadChar(S)!=0); 
            i_o->tt = LUA_TBOOLEAN; 
        };
        break;
      }
      case LUA_TNUMBER: { /* 3 */
        { 
            TValue *i_o = o; 
            i_o->value.n = LoadNumber(S); 
            i_o->tt = LUA_TNUMBER; 
        };
        break;
      }
      case LUA_TSTRING: { /* 4 */
        /* setsvalue2n(S->L,o,LoadString(S)); */
        { 
            TValue *i_o = o; 
            i_o->value.gc = (GCObject *)(LoadString(S)); 
            i_o->tt = LUA_TSTRING; 
            ((void)0); 
        };
        break;
      }
      default: {
       error(S,"bad constant");
       break;
      }
    }
  }
  n = LoadInt(S);
  /*     luaM_newvector(S->L,n,Proto*); */
  f->p = (Proto* *)(    ( (size_t)(n+1) <= MAX_SIZET/sizeof(Proto*) ) 
                       ? luaM_realloc_(S->L, NULL, 0 * sizeof(Proto*), n * sizeof(Proto*) ) 
                       : luaM_toobig(S->L)
                   );
  f->sizep = n;
  for (i=0; i<n; i++) {
    f->p[i] = NULL;
  }
  for (i=0; i<n; i++) {
    f->p[i]= LoadFunction(S,f->source);
  }
}


static void LoadDebug(LoadState* S, Proto* f)
{
  int i,n;
  n = LoadInt(S);
  /*             luaM_newvector(S->L,n,int); */
  f->lineinfo = (int *)(  ( (size_t)(n+1) <= MAX_SIZET/sizeof(int) ) 
                          ? luaM_realloc_(S->L, NULL, 0 * sizeof(int), n * sizeof(int) ) 
                          : luaM_toobig(S->L)
                       );
  f->sizelineinfo = n;
  /* LoadVector(S,f->lineinfo,n,sizeof(int)); */
  LoadBlock(S,f->lineinfo,n * sizeof(int) );
  n = LoadInt(S);
  /*            luaM_newvector(S->L,n,LocVar); */
  f->locvars = (LocVar *)(  ( (size_t)(n+1) <= MAX_SIZET/sizeof(LocVar) ) 
                            ? luaM_realloc_(S->L, NULL, 0 * sizeof(LocVar), n * sizeof(LocVar) ) 
                            : luaM_toobig(S->L)
                         );
  f->sizelocvars = n;
  for (i=0; i<n; i++) {
    f->locvars[i].varname=NULL;
  }
  for (i=0; i<n; i++)
  {
    f->locvars[i].varname = LoadString(S);
    f->locvars[i].startpc = LoadInt(S);
    f->locvars[i].endpc = LoadInt(S);
  }
  n = LoadInt(S);
  /*             luaM_newvector(S->L,n,TString*); */
  f->upvalues = (TString* *)(   ((size_t)(n+1) <= MAX_SIZET/sizeof(TString*) ) 
                              ? luaM_realloc_(S->L, NULL, 0 * sizeof(TString*), n * sizeof(TString*) ) 
                              : luaM_toobig(S->L)
                            );
  f->sizeupvalues = n;
  for (i=0; i<n; i++) {
    f->upvalues[i] = NULL;
  }
  for (i=0; i<n; i++) {
    f->upvalues[i] = LoadString(S);
  }
}


static Proto* LoadFunction(LoadState* S, TString* p)
{
  Proto* f;
  /*                             200   LUAI_MAXCCALLS */
  if (++S->L->nCcalls > LUAI_MAXCCALLS) {
    error(S,"code too deep");
  }
  f = luaF_newproto(S->L);
  /* setptvalue2s(S->L,S->L->top,f); incr_top(S->L); */
  { 
      TValue *i_o = S->L->top; 
      i_o->value.gc = (GCObject *)(f); 
      /*         (8       + 1) */
      i_o->tt = (LAST_TAG + 1); 
      ((void)0); 
  }; 
  {
      if ((char *)S->L->stack_last - (char *)S->L->top <= 1 * (int)sizeof(TValue) ) { 
          luaD_growstack(S->L, 1); 
      } else { 
          ((void)0);; 
      }
      S->L->top++;
  };
  f->source = LoadString(S);
  if (f->source == NULL) {
    f->source = p;
  }
  f->linedefined = LoadInt(S);
  f->lastlinedefined = LoadInt(S);
  /*         LoadByte(S); */
  f->nups = (lu_byte)LoadChar(S);
  /*             LoadByte(S); */
  f->numparams = (lu_byte)LoadChar(S);
  /*             LoadByte(S); */
  f->is_vararg = (lu_byte)LoadChar(S);
  /*                LoadByte(S); */
  f->maxstacksize = (lu_byte)LoadChar(S);
  LoadCode(S,f);
  LoadConstants(S,f);
  LoadDebug(S,f);
  /* IF (!luaG_checkcode(f), "bad code"); */
  if ( !luaG_checkcode(f) ) {
      error(S,"bad code");
  }
  S->L->top--;
  S->L->nCcalls--;
  return f;
}


static void LoadHeader(LoadState* S)
{
  /*              12     LUAC_HEADERSIZE */
  char h[LUAC_HEADERSIZE];
  /*              12     LUAC_HEADERSIZE */
  char s[LUAC_HEADERSIZE];
  luaU_header(h);
  /*                    12     LUAC_HEADERSIZE */
  LoadBlock(S,s,LUAC_HEADERSIZE);
  /* IF (memcmp(h,s,LUAC_HEADERSIZE) != 0, "bad header"); */
  /*                    12     LUAC_HEADERSIZE */
  if ( memcmp(h,s,LUAC_HEADERSIZE) != 0) { 
      error(S,"bad header");
  }
}


/*
** load precompiled chunk
*/
Proto* luaU_undump (lua_State* L, ZIO* Z, Mbuffer* buff, const char* name)
{
  LoadState S;
  if (*name=='@' || *name=='=') {
    S.name = name+1;
  /*                  "\033Lua"    LUA_SIGNATURE */
  } else if (*name == LUA_SIGNATURE[0]) {
    S.name = "binary string";
  } else {
    S.name = name;
  }
  S.L = L;
  S.Z = Z;
  S.b = buff;
  LoadHeader(&S);
  /*                      luaS_newliteral(L,"=?") */
  return LoadFunction(&S,(luaS_newlstr(L, "" "=?", (sizeof("=?")/sizeof(char))-1)));
}


/*
* make header
*/
void luaU_header (char* h)
{
  int x = 1;
  /*           "\033Lua"  LUA_SIGNATURE */
  memcpy(h, LUA_SIGNATURE, sizeof(LUA_SIGNATURE)-1 );
  /*          "\033Lua"  LUA_SIGNATURE */
  h += sizeof(LUA_SIGNATURE)-1;
  /*                0x51  LUAC_VERSION */
  *h++ = (char)LUAC_VERSION;
  /*                 0  LUAC_FORMAT */
  *h++ = (char)LUAC_FORMAT;
  *h++ = (char)*(char*)&x; /* endianness */
  *h++ = (char)sizeof(int);
  *h++ = (char)sizeof(size_t);
  *h++ = (char)sizeof(Instruction);
  *h++ = (char)sizeof(lua_Number);
  *h++ = (char)( ( (lua_Number)0.5) == 0 ); /* is lua_Number integral? */
}


