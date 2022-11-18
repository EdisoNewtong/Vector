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
  /*            1   EXIT_FAILURE */
  exit(EXIT_FAILURE);
}


static void cannot(const char* what)
{
  fprintf(stderr,"%s: cannot %s %s: %s\n",progname,what,output,strerror(errno));
  /*            1   EXIT_FAILURE */
  exit(EXIT_FAILURE);
}


static void usage(const char* message)
{
  if (*message == '-') {
    /*                                         LUA_QS */
    fprintf(stderr,"%s: unrecognized option " "'" "%s" "'" "\n",progname,message);
  } else {
    fprintf(stderr,"%s: %s\n",progname,message);
  }
  fprintf(stderr,
          "usage: %s [options] [filenames].\n"
          "Available options are:\n"
          "  -        process stdin\n"
          "  -l       list\n"
          /*                        LUA_QL("name") */
          "  -o name  output to file " "'" "name" "'" " (default is \"%s\")\n"
          "  -p       parse only\n"
          "  -s       strip debug information\n"
          "  -v       show version information\n"
          "  --       stop handling options\n",
          progname,Output);

  /*            1   EXIT_FAILURE */
  exit(EXIT_FAILURE);
}


static int doargs(int argc, char* argv[])
{
  int i;
  int version = 0;
  if (argv[0] != NULL && *argv[0] != 0) {
    progname = argv[0];
  }

  for (i = 1; i<argc; i++)
  {
    if (*argv[i] != '-') { /* end of options; keep it */
      break;
    }
    /*                    IS("--") */
    else if ( strcmp(argv[i],"--") == 0 ) /* end of options; skip it */
    {
      ++i;
      if (version) {
        ++version;
      }
      break;
    }
    /*                    IS("-") */
    else if ( strcmp(argv[i],"-") == 0 ) { /* end of options; use stdin */
      break;
    /*                      IS("-l") */
    } else if ( strcmp(argv[i],"-l") == 0 ) { /* list */
      ++listing;
    }
    /*                    IS("-o") */
    else if ( strcmp(argv[i],"-o") == 0) /* output file */
    {
      output = argv[++i];
      if ( output == NULL || *output == 0 ) {
        /* LUA_QL("-o") */
        usage("'" "-o" "'" " needs argument");
      }
      /*               IS("-") */
      if ( strcmp(argv[i],"-") == 0 ) {
        output = NULL;
      }
    }
    /*                    IS("-p") */
    else if ( strcmp(argv[i],"-p") == 0 ) { /* parse only */
      dumping = 0;
    }
    /*                    IS("-s") */
    else if ( strcmp(argv[i],"-s") == 0) { /* strip debug information */
      stripping = 1;
    /*                      IS("-v") */
    } else if ( strcmp(argv[i],"-v") == 0 ) { /* show version */
      ++version;
    } else { /* unknown option */
      usage(argv[i]);
    }
  }

  if ( i == argc && (listing || !dumping) )
  {
    dumping = 0;
    argv[--i] = Output;
  }

  if (version)
  {
    /*                "Lua 5.1.5"  "Copyright (C) 1994-2012 Lua.org, PUC-Rio"  */
    printf("%s  %s\n",LUA_RELEASE, LUA_COPYRIGHT);
    if ( version == argc-1 ) {
      /*           0    EXIT_SUCCESS */
      exit(EXIT_SUCCESS);
    }
  }
  return i;
}

#define toproto(L,i) (clvalue(L->top+(i))->l.p)

static const Proto* combine(lua_State* L, int n)
{
 if (n == 1) {
   /*      toproto(L,-1); */
   return ((&(L->top+(-1))->value.gc->cl)->l.p);
 } else {
   int i,pc;
   Proto* f = luaF_newproto(L);
   /* setptvalue2s(L,L->top,f); incr_top(L); */
   { 
       TValue *i_o = L->top; 
       i_o->value.gc = (GCObject *)(f); 
       /*        (  8     +1) */
       i_o->tt = (LAST_TAG+1); 
       ((void)0); 
   }; 
   {
       if ((char *)L->stack_last - (char *)L->top <= 1 * (int)sizeof(TValue) ) { 
          luaD_growstack(L, 1);
       } else { 
          ((void)0);; 
       } 
       L->top++;
   };
   /*           luaS_newliteral(L,"=(" PROGNAME ")"); */
   f->source = (luaS_newlstr(L, "" "=(" "luac" ")", (sizeof("=(" "luac" ")")/sizeof(char))-1) );
   f->maxstacksize = 1;
   pc = 2*n+1;
   /*        luaM_newvector(L,pc,Instruction); */
   f->code = (Instruction *)(   ( (size_t)(pc+1) <= MAX_SIZET/sizeof(Instruction) ) 
                              ? luaM_realloc_(L, NULL, 0 * sizeof(Instruction), pc * sizeof(Instruction) ) 
                              : luaM_toobig(L)
                            );
   f->sizecode = pc;
   /*      luaM_newvector(L,n,Proto*); */
   f->p = (Proto* *)(   ((size_t)(n+1) <= MAX_SIZET/sizeof(Proto*) ) 
                      ? luaM_realloc_(L, NULL, 0 * sizeof(Proto*), n * sizeof(Proto*) ) 
                      : luaM_toobig(L) 
                    );
   f->sizep = n;
   pc = 0;
   for (i = 0; i<n; i++)
   {
     /*       toproto(L,i-n-1); */
     f->p[i]= (&(L->top+(i-n-1))->value.gc->cl)->l.p;
     /*              CREATE_ABx(OP_CLOSURE,0,i); */
     f->code[pc++] = (   ( (Instruction)OP_CLOSURE) << 0 )
                       | ( (Instruction)(0)         <<  (0 + 6)     ) 
                       | ( (Instruction)(i)         <<  ((0 + 6) + 8) );
     /*              CREATE_ABC(OP_CALL,0,1,1); */
     f->code[pc++] = (       ( (Instruction)OP_CALL)    << 0 ) 
                           | ( ((Instruction)0)         << (0 + 6)) 
                           | ( ((Instruction)1)         << (((0 + 6) + 8) + 9)) 
                           | ( ((Instruction)1)         << ((0 + 6) + 8) );
   }
   /*              CREATE_ABC(OP_RETURN,0,1,0); */
   f->code[pc++] = (     ( (Instruction)OP_RETURN) <<   0 ) 
                       | ( ((Instruction)0)        <<  (0 + 6) ) 
                       | ( ((Instruction)1)        <<  (((0 + 6) + 8) + 9) ) 
                       | ( ((Instruction)0)        <<  ((0 + 6) + 8) );
   return f;
 }
}


static int writer(lua_State* L, const void* p, size_t size, void* u)
{
  /* UNUSED(L); */
  (void)L;
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
    /*                                  IS("-") */
    const char* filename = (strcmp(argv[i],"-") == 0) ? NULL : argv[i];
    if (luaL_loadfile(L,filename)!=0) {
      /*     lua_tostring(L,-1) */
      fatal( lua_tolstring(L, -1, NULL) );
    }
  }

  f = combine(L,argc);
  if (listing) {
    luaU_print(f,listing>1);
  }

  if (dumping)
  {
    FILE* D = (output == NULL) ? stdout : fopen(output,"wb");
    if (D == NULL) {
      cannot("open");
    }
    /* lua_lock(L); */
    ((void) 0);
    luaU_dump(L,f,writer,D,stripping);
    /* lua_unlock(L); */
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
  if (L == NULL) {
    fatal("not enough memory for state");
  }
  s.argc = argc;
  s.argv = argv;
  if (lua_cpcall(L,pmain,&s) != 0) {
    /*     lua_tostring(L,-1) */
    fatal( lua_tolstring(L, -1, NULL));
  }
  lua_close(L);
  /*           0       EXIT_SUCCESS */
  return EXIT_SUCCESS;
}


