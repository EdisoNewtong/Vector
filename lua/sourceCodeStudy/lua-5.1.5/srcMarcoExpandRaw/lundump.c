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
  if (r!=0) error(S,"unexpected end");
}


static int LoadChar(LoadState* S)
{
  char x;
  LoadBlock(S,&x,(1)*(sizeof(x)));
  return x;
}


static int LoadInt(LoadState* S)
{
  int x;
  LoadBlock(S,&x,(1)*(sizeof(x)));
  if (x<0) error(S,"bad integer");
  return x;
}


static lua_Number LoadNumber(LoadState* S)
{
  lua_Number x;
  LoadBlock(S,&x,(1)*(sizeof(x)));
  return x;
}


static TString* LoadString(LoadState* S)
{
  size_t size;
  LoadBlock(S,&size,(1)*(sizeof(size)));
  if (size==0) {
    return ((void *)0);
  } else {
    char* s=luaZ_openspace(S->L,S->b,size);
    LoadBlock(S,s,size);
    return luaS_newlstr(S->L,s,size-1); /* remove trailing '\0' */
  }
}


static void LoadCode(LoadState* S, Proto* f)
{
  int n=LoadInt(S);
  f->code=((Instruction *)(((((size_t)((n)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(Instruction))) ? luaM_realloc_(S->L, (((void *)0)), (0)*(sizeof(Instruction)), (n)*(sizeof(Instruction))) : luaM_toobig(S->L))));
  f->sizecode=n;
  LoadBlock(S,f->code,(n)*(sizeof(Instruction)));
}


static Proto* LoadFunction(LoadState* S, TString* p);

static void LoadConstants(LoadState* S, Proto* f)
{
  int i,n;
  n = LoadInt(S);
  f->k = ((TValue *)(((((size_t)((n)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(TValue))) ? luaM_realloc_(S->L, (((void *)0)), (0)*(sizeof(TValue)), (n)*(sizeof(TValue))) : luaM_toobig(S->L))));
  f->sizek = n;
  for (i=0; i<n; i++) {
    ((&f->k[i])->tt=0);
  }
  for (i=0; i<n; i++)
  {
    TValue* o=&f->k[i];
    int t=LoadChar(S);
    switch (t)
    {
      case 0: {
        ((o)->tt=0);
        break;
      }
      case 1: {
        { TValue *i_o=(o); i_o->value.b=(LoadChar(S)!=0); i_o->tt=1; };
        break;
      }
      case 3: {
        { TValue *i_o=(o); i_o->value.n=(LoadNumber(S)); i_o->tt=3; };
        break;
      }
      case 4: {
        { TValue *i_o=(o); i_o->value.gc=((GCObject *)((LoadString(S)))); i_o->tt=4; ((void)0); };
        break;
      }
      default: {
       error(S,"bad constant");
       break;
      }
    }
  }
  n = LoadInt(S);
  f->p = ((Proto* *)(((((size_t)((n)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(Proto*))) ? luaM_realloc_(S->L, (((void *)0)), (0)*(sizeof(Proto*)), (n)*(sizeof(Proto*))) : luaM_toobig(S->L))));
  f->sizep = n;
  for (i=0; i<n; i++) {
    f->p[i] = ((void *)0);
  }
  for (i=0; i<n; i++) {
    f->p[i]= LoadFunction(S,f->source);
  }
}


static void LoadDebug(LoadState* S, Proto* f)
{
  int i,n;
  n = LoadInt(S);
  f->lineinfo = ((int *)(((((size_t)((n)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(int))) ? luaM_realloc_(S->L, (((void *)0)), (0)*(sizeof(int)), (n)*(sizeof(int))) : luaM_toobig(S->L))));
  f->sizelineinfo = n;
  LoadBlock(S,f->lineinfo,(n)*(sizeof(int)));
  n = LoadInt(S);
  f->locvars = ((LocVar *)(((((size_t)((n)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(LocVar))) ? luaM_realloc_(S->L, (((void *)0)), (0)*(sizeof(LocVar)), (n)*(sizeof(LocVar))) : luaM_toobig(S->L))));
  f->sizelocvars = n;
  for (i=0; i<n; i++) {
    f->locvars[i].varname=((void *)0);
  }
  for (i=0; i<n; i++)
  {
    f->locvars[i].varname = LoadString(S);
    f->locvars[i].startpc = LoadInt(S);
    f->locvars[i].endpc = LoadInt(S);
  }
  n = LoadInt(S);
  f->upvalues = ((TString* *)(((((size_t)((n)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(TString*))) ? luaM_realloc_(S->L, (((void *)0)), (0)*(sizeof(TString*)), (n)*(sizeof(TString*))) : luaM_toobig(S->L))));
  f->sizeupvalues = n;
  for (i=0; i<n; i++) {
    f->upvalues[i] = ((void *)0);
  }
  for (i=0; i<n; i++) {
    f->upvalues[i] = LoadString(S);
  }
}


static Proto* LoadFunction(LoadState* S, TString* p)
{
  Proto* f;
  if (++S->L->nCcalls > 200) {
    error(S,"code too deep");
  }
  f = luaF_newproto(S->L);
  { TValue *i_o=(S->L->top); i_o->value.gc=((GCObject *)((f))); i_o->tt=(8 +1); ((void)0); }; {if ((char *)S->L->stack_last - (char *)S->L->top <= (1)*(int)sizeof(TValue)) luaD_growstack(S->L, 1); else ((void)0);; S->L->top++;};
  f->source = LoadString(S);
  if (f->source == ((void *)0)) {
    f->source = p;
  }
  f->linedefined = LoadInt(S);
  f->lastlinedefined = LoadInt(S);
  f->nups = (lu_byte)LoadChar(S);
  f->numparams = (lu_byte)LoadChar(S);
  f->is_vararg = (lu_byte)LoadChar(S);
  f->maxstacksize = (lu_byte)LoadChar(S);
  LoadCode(S,f);
  LoadConstants(S,f);
  LoadDebug(S,f);
  if (!luaG_checkcode(f)) error(S,"bad code");
  S->L->top--;
  S->L->nCcalls--;
  return f;
}


static void LoadHeader(LoadState* S)
{
  char h[12];
  char s[12];
  luaU_header(h);
  LoadBlock(S,s,12);
  if (memcmp(h,s,12) != 0) error(S,"bad header");
}


/*
** load precompiled chunk
*/
Proto* luaU_undump (lua_State* L, ZIO* Z, Mbuffer* buff, const char* name)
{
  LoadState S;
  if (*name=='@' || *name=='=') {
    S.name = name+1;
  } else if (*name=="\033Lua"[0]) {
    S.name = "binary string";
  } else {
    S.name = name;
  }
  S.L = L;
  S.Z = Z;
  S.b = buff;
  LoadHeader(&S);
  return LoadFunction(&S,(luaS_newlstr(L, "" "=?", (sizeof("=?")/sizeof(char))-1)));
}


/*
* make header
*/
void luaU_header (char* h)
{
  int x = 1;
  memcpy(h,"\033Lua",sizeof("\033Lua")-1);
  h += sizeof("\033Lua")-1;
  *h++ = (char)0x51;
  *h++ = (char)0;
  *h++ = (char)*(char*)&x; /* endianness */
  *h++ = (char)sizeof(int);
  *h++ = (char)sizeof(size_t);
  *h++ = (char)sizeof(Instruction);
  *h++ = (char)sizeof(lua_Number);
  *h++ = (char)(((lua_Number)0.5)==0); /* is lua_Number integral? */
}


