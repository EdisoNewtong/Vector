/*
** $Id: ldump.c,v 2.8.1.1 2007/12/27 13:02:25 roberto Exp $
** save precompiled Lua chunks
** See Copyright Notice in lua.h
*/

#include <stddef.h>

#define ldump_c
#define LUA_CORE

#include "lua.h"

#include "lobject.h"
#include "lstate.h"
#include "lundump.h"

typedef struct {
  lua_State* L;
  lua_Writer writer;
  void* data;
  int strip;
  int status;
} DumpState;

#define DumpMem(b,n,size,D)	DumpBlock(b,(n)*(size),D)
#define DumpVar(x,D)	 	DumpMem(&x,1,sizeof(x),D)

static void DumpBlock(const void* b, size_t size, DumpState* D)
{
  if (D->status==0)
  {
    ((void) 0);
    D->status=(*D->writer)(D->L,b,size,D->data);
    ((void) 0);
  }
}


static void DumpChar(int y, DumpState* D)
{
  char x = (char)y;
  DumpBlock(&x,(1)*(sizeof(x)),D);
}


static void DumpInt(int x, DumpState* D)
{
  DumpBlock(&x,(1)*(sizeof(x)),D);
}


static void DumpNumber(lua_Number x, DumpState* D)
{
  DumpBlock(&x,(1)*(sizeof(x)),D);
}


static void DumpVector(const void* b, int n, size_t size, DumpState* D)
{
  DumpInt(n,D);
  DumpBlock(b,(n)*(size),D);
}


static void DumpString(const TString* s, DumpState* D)
{
 if ( s == ((void *)0) || ((const char *)((s) + 1)) == ((void *)0))
 {
   size_t size = 0;
   DumpBlock(&size,(1)*(sizeof(size)),D);
 }
 else
 {
   size_t size = s->tsv.len + 1; /* include trailing '\0' */
   DumpBlock(&size,(1)*(sizeof(size)),D);
   DumpBlock(((const char *)((s) + 1)),size,D);
 }
}

#define DumpCode(f,D)	 DumpVector(f->code,f->sizecode,sizeof(Instruction),D)

static void DumpFunction(const Proto* f, const TString* p, DumpState* D);

static void DumpConstants(const Proto* f, DumpState* D)
{
  int i,n = f->sizek;
  DumpInt(n,D);
  for (i=0; i<n; i++)
  {
    const TValue* o=&f->k[i];
    DumpChar(((o)->tt),D);
    switch (((o)->tt))
    {
      case 0:
        break;
      case 1:
        DumpChar(((o)->value.b),D);
        break;
      case 3:
        DumpNumber(((o)->value.n),D);
        break;
      case 4:
        DumpString((&(o)->value.gc->ts),D);
        break;
      default:
        ((void)0); /* cannot happen */
        break;
    }
  }
  n = f->sizep;
  DumpInt(n,D);
  for (i=0; i<n; i++) {
    DumpFunction(f->p[i],f->source,D);
  }
}


static void DumpDebug(const Proto* f, DumpState* D)
{
  int i,n;
  n = (D->strip) ? 0 : f->sizelineinfo;
  DumpVector(f->lineinfo,n,sizeof(int),D);
  n = (D->strip) ? 0 : f->sizelocvars;
  DumpInt(n,D);
  for (i=0; i<n; i++)
  {
    DumpString(f->locvars[i].varname,D);
    DumpInt(f->locvars[i].startpc,D);
    DumpInt(f->locvars[i].endpc,D);
  }
  n = (D->strip) ? 0 : f->sizeupvalues;
  DumpInt(n,D);
  for (i=0; i<n; i++) {
    DumpString(f->upvalues[i],D);
  }
}


static void DumpFunction(const Proto* f, const TString* p, DumpState* D)
{
  DumpString((f->source==p || D->strip) ? ((void *)0) : f->source,D);
  DumpInt(f->linedefined,D);
  DumpInt(f->lastlinedefined,D);
  DumpChar(f->nups,D);
  DumpChar(f->numparams,D);
  DumpChar(f->is_vararg,D);
  DumpChar(f->maxstacksize,D);
  DumpVector(f->code,f->sizecode,sizeof(Instruction),D);
  DumpConstants(f,D);
  DumpDebug(f,D);
}


static void DumpHeader(DumpState* D)
{
  char h[12];
  luaU_header(h);
  DumpBlock(h,12,D);
}


/*
** dump Lua function as precompiled chunk
*/
int luaU_dump (lua_State* L, const Proto* f, lua_Writer w, void* data, int strip)
{
  DumpState D;
  D.L = L;
  D.writer = w;
  D.data = data;
  D.strip = strip;
  D.status = 0;
  DumpHeader(&D);
  DumpFunction(f,((void *)0),&D);
  return D.status;
}


