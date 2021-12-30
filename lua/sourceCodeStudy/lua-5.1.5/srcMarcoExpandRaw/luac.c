/*
** $Id: luac.c,v 1.54 2006/06/02 17:37:11 lhf Exp $
** Lua compiler (saves bytecodes to files; also list bytecodes)
** See Copyright Notice in lua.h
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define luac_c
#define LUA_CORE

#include "lua.h"
#include "lauxlib.h"

#include "ldo.h"
#include "lfunc.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstring.h"
#include "lundump.h"

#define PROGNAME	"luac"		/* default program name */
#define	OUTPUT		PROGNAME ".out"	/* default output file */

static int listing = 0; /* list bytecodes? */
static int dumping = 1; /* dump bytecodes? */
static int stripping = 0; /* strip debug information? */
static char Output[] = { "luac" ".out" }; /* default output file name */
static const char* output = Output; /* actual output file name */
static const char* progname = "luac"; /* actual program name */

static void fatal(const char* message)
{
  fprintf(stderr,"%s: %s\n",progname,message);
  exit(1);
}


static void cannot(const char* what)
{
  fprintf(stderr,"%s: cannot %s %s: %s\n",progname,what,output,strerror((*__errno_location ())));
  exit(1);
}


static void usage(const char* message)
{
  if (*message == '-') {
    fprintf(stderr,"%s: unrecognized option " "'" "%s" "'" "\n",progname,message);
  } else {
    fprintf(stderr,"%s: %s\n",progname,message);
  }
  fprintf(stderr,
          "usage: %s [options] [filenames].\n"
          "Available options are:\n"
          "  -        process stdin\n"
          "  -l       list\n"
          "  -o name  output to file " "'" "name" "'" " (default is \"%s\")\n"
          "  -p       parse only\n"
          "  -s       strip debug information\n"
          "  -v       show version information\n"
          "  --       stop handling options\n",
          progname,Output);
  exit(1);
}


static int doargs(int argc, char* argv[])
{
  int i;
  int version = 0;
  if (argv[0] != ((void *)0) && *argv[0] != 0) {
    progname = argv[0];
  }
  for (i = 1; i<argc; i++)
  {
    if (*argv[i] != '-') { /* end of options; keep it */
      break;
    }
    else if ((strcmp(argv[i],"--") == 0)) /* end of options; skip it */
    {
      ++i;
      if (version) {
        ++version;
      }
      break;
    }
    else if ((strcmp(argv[i],"-") == 0)) { /* end of options; use stdin */
      break;
    } else if ((strcmp(argv[i],"-l") == 0)) { /* list */
      ++listing;
    }
    else if ((strcmp(argv[i],"-o") == 0)) /* output file */
    {
     output = argv[++i];
     if (output == ((void *)0) || *output == 0 ) {
       usage("'" "-o" "'" " needs argument");
     }
     if ((strcmp(argv[i],"-") == 0)) {
       output = ((void *)0);
     }
    }
    else if ((strcmp(argv[i],"-p") == 0)) { /* parse only */
      dumping = 0;
    }
    else if ((strcmp(argv[i],"-s") == 0)) { /* strip debug information */
      stripping = 1;
    } else if ((strcmp(argv[i],"-v") == 0)) { /* show version */
      ++version;
    } else { /* unknown option */
      usage(argv[i]);
    }
  }
  if (i == argc && (listing || !dumping))
  {
    dumping = 0;
    argv[--i] = Output;
  }
  if (version)
  {
    printf("%s  %s\n","Lua 5.1.5","Copyright (C) 1994-2012 Lua.org, PUC-Rio");
    if ( version == argc-1 ) {
      exit(0);
    }
  }
  return i;
}


static const Proto* combine(lua_State* L, int n)
{
 if (n == 1) {
   return ((&(L->top+(-1))->value.gc->cl)->l.p);
 } else {
   int i,pc;
   Proto* f = luaF_newproto(L);
   { TValue *i_o=(L->top); i_o->value.gc=((GCObject *)((f))); i_o->tt=(8 +1); ((void)0); }; {if ((char *)L->stack_last - (char *)L->top <= (1)*(int)sizeof(TValue)) luaD_growstack(L, 1); else ((void)0);; L->top++;};
   f->source = (luaS_newlstr(L, "" "=(" "luac" ")", (sizeof("=(" "luac" ")")/sizeof(char))-1));
   f->maxstacksize = 1;
   pc = 2*n+1;
   f->code = ((Instruction *)(((((size_t)((pc)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(Instruction))) ? luaM_realloc_(L, (((void *)0)), (0)*(sizeof(Instruction)), (pc)*(sizeof(Instruction))) : luaM_toobig(L))));
   f->sizecode = pc;
   f->p = ((Proto* *)(((((size_t)((n)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(Proto*))) ? luaM_realloc_(L, (((void *)0)), (0)*(sizeof(Proto*)), (n)*(sizeof(Proto*))) : luaM_toobig(L))));
   f->sizep = n;
   pc = 0;
   for (i = 0; i<n; i++)
   {
     f->p[i]= ((&(L->top+(i-n-1))->value.gc->cl)->l.p);
     f->code[pc++] = ((((Instruction)(OP_CLOSURE))<<0) | (((Instruction)(0))<<(0 + 6)) | (((Instruction)(i))<<((0 + 6) + 8)));
     f->code[pc++] = ((((Instruction)(OP_CALL))<<0) | (((Instruction)(0))<<(0 + 6)) | (((Instruction)(1))<<(((0 + 6) + 8) + 9)) | (((Instruction)(1))<<((0 + 6) + 8)));
   }
   f->code[pc++] = ((((Instruction)(OP_RETURN))<<0) | (((Instruction)(0))<<(0 + 6)) | (((Instruction)(1))<<(((0 + 6) + 8) + 9)) | (((Instruction)(0))<<((0 + 6) + 8)));
   return f;
 }
}


static int writer(lua_State* L, const void* p, size_t size, void* u)
{
  ((void)(L));
  return (fwrite(p,size,1,(FILE*)u) != 1) && (size != 0);
}


struct Smain {
 int argc;
 char** argv;
};


static int pmain(lua_State* L)
{
  struct Smain* s = (struct Smain*)lua_touserdata(L, 1);
  int argc = s->argc;
  char** argv = s->argv;
  const Proto* f;
  int i;
  if (!lua_checkstack(L,argc)) {
    fatal("too many input files");
  }
  for (i = 0; i<argc; i++)
  {
    const char* filename = (strcmp(argv[i],"-") == 0) ? ((void *)0) : argv[i];
    if (luaL_loadfile(L,filename)!=0) {
      fatal(lua_tolstring(L, (-1), ((void *)0)));
    }
  }
  f = combine(L,argc);
  if (listing) {
    luaU_print(f,listing>1);
  }
  if (dumping)
  {
    FILE* D = (output == ((void *)0)) ? stdout : fopen(output,"wb");
    if (D == ((void *)0)) {
      cannot("open");
    }
    ((void) 0);
    luaU_dump(L,f,writer,D,stripping);
    ((void) 0);
    if (ferror(D)) {
      cannot("write");
    }
    if (fclose(D)) {
      cannot("close");
    }
  }
  return 0;
}


int main(int argc, char* argv[])
{
  lua_State* L;
  struct Smain s;
  int i = doargs(argc,argv);
  argc -= i;
  argv += i;
  if (argc <= 0) {
    usage("no input files given");
  }
  L = luaL_newstate();
  if (L == ((void *)0)) {
    fatal("not enough memory for state");
  }
  s.argc = argc;
  s.argv = argv;
  if (lua_cpcall(L,pmain,&s) != 0) {
    fatal(lua_tolstring(L, (-1), ((void *)0)));
  }
  lua_close(L);
  return 0;
}


