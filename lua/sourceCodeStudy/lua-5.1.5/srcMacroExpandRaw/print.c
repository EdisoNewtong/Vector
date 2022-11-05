/*
** $Id: print.c,v 1.55a 2006/05/31 13:30:05 lhf Exp $
** print bytecodes
** See Copyright Notice in lua.h
*/

#include <ctype.h>
#include <stdio.h>

#define luac_c
#define LUA_CORE

#include "ldebug.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lundump.h"

#define PrintFunction	luaU_print

#define Sizeof(x)	((int)sizeof(x))
#define VOID(p)		((const void*)(p))

static void PrintString(const TString* ts)
{
  const char* s = ((const char *)((ts) + 1));
  size_t i,n = ts->tsv.len;
  putchar('"');
  for (i = 0; i<n; i++)
  {
    int c = s[i];
    switch (c)
    {
      case '"': {
        printf("\\\"");
        break;
      }
      case '\\': {
        printf("\\\\");
        break;
      }
      case '\a': {
        printf("\\a");
        break;
      }
      case '\b': {
        printf("\\b");
        break;
      }
      case '\f': {
        printf("\\f");
        break;
      }
      case '\n': {
        printf("\\n");
        break;
      }
      case '\r': {
        printf("\\r");
        break;
      }
      case '\t': {
        printf("\\t");
        break;
      }
      case '\v': {
        printf("\\v");
        break;
      }
      default: {
        if ( ((*__ctype_b_loc ())[(int) (((unsigned char)c))] & (unsigned short int) _ISprint) ) {
        putchar(c);
        } else {
          printf("\\%03u",(unsigned char)c);
        }
      }
    }
  }
  putchar('"');
}


static void PrintConstant(const Proto* f, int i)
{
  const TValue* o = &f->k[i];
  switch (((o)->tt))
  {
   case 0: {
     printf("nil");
     break;
   }
   case 1: {
     printf(((o)->value.b) ? "true" : "false");
     break;
   }
   case 3: {
     printf("%.14g",((o)->value.n));
     break;
   }
   case 4: {
     PrintString((&(o)->value.gc->ts));
     break;
   }
   default: { /* cannot happen */
     printf("? type=%d",((o)->tt));
     break;
   }
  }
}


static void PrintCode(const Proto* f)
{
  const Instruction* code=f->code;
  int pc,n = f->sizecode;
  for (pc=0; pc<n; pc++)
  {
    Instruction i = code[pc];
    OpCode o = (((OpCode)(((i)>>0) & ((~((~(Instruction)0)<<6))<<0))));
    int a = (((int)(((i)>>(0 + 6)) & ((~((~(Instruction)0)<<8))<<0))));
    int b = (((int)(((i)>>(((0 + 6) + 8) + 9)) & ((~((~(Instruction)0)<<9))<<0))));
    int c = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<9))<<0))));
    int bx = (((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))));
    int sbx = ((((int)(((i)>>((0 + 6) + 8)) & ((~((~(Instruction)0)<<(9 + 9)))<<0))))-(((1<<(9 + 9))-1)>>1));
    int line = (((f)->lineinfo) ? (f)->lineinfo[pc] : 0);
    printf("\t%d\t",pc+1);
    if (line>0) {
      printf("[%d]\t",line);
    } else {
      printf("[-]\t");
    }
    printf("%-9s\t",luaP_opnames[o]);
    switch ((((enum OpMode)(luaP_opmodes[o] & 3))))
    {
      case iABC: {
        printf("%d",a);
        if ((((enum OpArgMask)((luaP_opmodes[o] >> 4) & 3)))!=OpArgN) {
          printf(" %d",((b) & (1 << (9 - 1))) ? (-1-((int)(b) & ~(1 << (9 - 1)))) : b);
        }
        if ((((enum OpArgMask)((luaP_opmodes[o] >> 2) & 3)))!=OpArgN) {
          printf(" %d",((c) & (1 << (9 - 1))) ? (-1-((int)(c) & ~(1 << (9 - 1)))) : c);
        }
        break;
      }
      case iABx: {
        if ((((enum OpArgMask)((luaP_opmodes[o] >> 4) & 3)))==OpArgK) {
          printf("%d %d",a,-1-bx);
        } else {
          printf("%d %d",a,bx);
        }
        break;
      }
      case iAsBx: {
        if (o==OP_JMP) {
          printf("%d",sbx);
        } else {
          printf("%d %d",a,sbx);
        }
        break;
      }
    }
    switch (o)
    {
      case OP_LOADK: {
        printf("\t; "); PrintConstant(f,bx);
        break;
      }
      case OP_GETUPVAL:
      case OP_SETUPVAL: {
        printf("\t; %s", (f->sizeupvalues>0) ? ((const char *)((f->upvalues[b]) + 1)) : "-");
        break;
      }
      case OP_GETGLOBAL:
      case OP_SETGLOBAL: {
        printf("\t; %s",((const char *)(((&(&f->k[bx])->value.gc->ts)) + 1)));
        break;
      }
      case OP_GETTABLE:
      case OP_SELF: {
        if (((c) & (1 << (9 - 1)))) {
          printf("\t; ");
          PrintConstant(f,((int)(c) & ~(1 << (9 - 1))));
        }
        break;
      }
      case OP_SETTABLE:
      case OP_ADD:
      case OP_SUB:
      case OP_MUL:
      case OP_DIV:
      case OP_POW:
      case OP_EQ:
      case OP_LT:
      case OP_LE: {
        if (((b) & (1 << (9 - 1))) || ((c) & (1 << (9 - 1))))
        {
          printf("\t; ");
          if (((b) & (1 << (9 - 1)))) {
            PrintConstant(f,((int)(b) & ~(1 << (9 - 1))));
          } else {
            printf("-");
          }
          printf(" ");
          if (((c) & (1 << (9 - 1)))) {
            PrintConstant(f,((int)(c) & ~(1 << (9 - 1))));
          } else {
            printf("-");
          }
        }
        break;
      }
      case OP_JMP:
      case OP_FORLOOP:
      case OP_FORPREP: {
        printf("\t; to %d",sbx+pc+2);
        break;
      }
      case OP_CLOSURE: {
        printf("\t; %p",((const void*)(f->p[bx])));
        break;
      }
      case OP_SETLIST: {
        if (c==0) {
          printf("\t; %d",(int)code[++pc]);
        } else {
          printf("\t; %d",c);
        }
        break;
      }
      default: {
        break;
      }
   }
   printf("\n");
  }
}


static void PrintHeader(const Proto* f)
{
  const char* s=((const char *)((f->source) + 1));
  if (*s=='@' || *s=='=') {
    s++;
  } else if (*s=="\033Lua"[0]) {
    s="(bstring)";
  } else {
    s="(string)";
  }
  printf("\n%s <%s:%d,%d> (%d instruction%s, %d bytes at %p)\n",
   (f->linedefined==0)?"main":"function",s,
  f->linedefined,f->lastlinedefined,
  f->sizecode,(f->sizecode==1)?"":"s",f->sizecode*((int)sizeof(Instruction)),((const void*)(f)));
  printf("%d%s param%s, %d slot%s, %d upvalue%s, ",
  f->numparams,f->is_vararg?"+":"",(f->numparams==1)?"":"s",
  f->maxstacksize,(f->maxstacksize==1)?"":"s",f->nups,(f->nups==1)?"":"s");
  printf("%d local%s, %d constant%s, %d function%s\n",
  f->sizelocvars,(f->sizelocvars==1)?"":"s",f->sizek,(f->sizek==1)?"":"s",f->sizep,(f->sizep==1)?"":"s");
}


static void PrintConstants(const Proto* f)
{
  int i,n = f->sizek;
  printf("constants (%d) for %p:\n",n,((const void*)(f)));
  for (i=0; i<n; i++)
  {
    printf("\t%d\t",i+1);
    PrintConstant(f,i);
    printf("\n");
  }
}


static void PrintLocals(const Proto* f)
{
  int i,n = f->sizelocvars;
  printf("locals (%d) for %p:\n",n,((const void*)(f)));
  for (i=0; i<n; i++)
  {
    printf("\t%d\t%s\t%d\t%d\n",
    i,((const char *)((f->locvars[i].varname) + 1)),f->locvars[i].startpc+1,f->locvars[i].endpc+1);
  }
}


static void PrintUpvalues(const Proto* f)
{
  int i,n = f->sizeupvalues;
  printf("upvalues (%d) for %p:\n",n,((const void*)(f)));
  if (f->upvalues==((void *)0)) {
    return;
  }
  for (i=0; i<n; i++)
  {
    printf("\t%d\t%s\n",i,((const char *)((f->upvalues[i]) + 1)));
  }
}


void luaU_print(const Proto* f, int full)
{
  int i,n = f->sizep;
  PrintHeader(f);
  PrintCode(f);
  if (full)
  {
    PrintConstants(f);
    PrintLocals(f);
    PrintUpvalues(f);
  }
  for (i=0; i<n; i++) {
    luaU_print(f->p[i],full);
  }
}


