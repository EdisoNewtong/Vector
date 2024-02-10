/*
** $Id: lauxlib.c,v 1.159.1.3 2008/01/21 13:20:51 roberto Exp $
** Auxiliary functions for building Lua libraries
** See Copyright Notice in lua.h
*/


#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* This file uses only the official API of Lua.
** Any function declared here could be written as an application function.
*/

#define lauxlib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"


#define FREELIST_REF	0	/* free list of references */


/* convert a stack index to positive */
#define abs_index(L, i)		((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : \
					lua_gettop(L) + (i) + 1)


/*
** {======================================================
** Error-report functions
** =======================================================
*/


extern int luaL_argerror (lua_State *L, int narg, const char *extramsg) {
  lua_Debug ar;
  if (!lua_getstack(L, 0, &ar)) { /* no stack frame? */
    return luaL_error(L, "bad argument #%d (%s)", narg, extramsg);
  }
  lua_getinfo(L, "n", &ar);
  if (strcmp(ar.namewhat, "method") == 0) {
    narg--; /* do not count `self' */
    if (narg == 0) { /* error is in the self argument itself? */
      return luaL_error(L, "calling " "'" "%s" "'" " on bad self (%s)",
                           ar.name, extramsg);
    }
  }
  if (ar.name == NULL) {
    ar.name = "?";
  }
  return luaL_error(L, "bad argument #%d to " "'" "%s" "'" " (%s)",
                        narg, ar.name, extramsg);
}


extern int luaL_typerror (lua_State *L, int narg, const char *tname) {
  const char *msg = lua_pushfstring(L, "%s expected, got %s",
                                    tname, lua_typename(L, lua_type(L,narg) ) );
  return luaL_argerror(L, narg, msg);
}


static void tag_error (lua_State *L, int narg, int tag) {
  luaL_typerror(L, narg, lua_typename(L, tag));
}


extern void luaL_where (lua_State *L, int level) {
  lua_Debug ar;
  if (lua_getstack(L, level, &ar)) { /* check function at level */
    lua_getinfo(L, "Sl", &ar); /* get info about it */
    if (ar.currentline > 0) { /* is there info? */
      lua_pushfstring(L, "%s:%d: ", ar.short_src, ar.currentline);
      return;
    }
  }

  /* lua_pushlstring(L, "" "", (sizeof("")/sizeof(char))-1); */  /* else, no information available... */
  lua_pushliteral(L, "");                                        /* else, no information available... */
}


extern int luaL_error (lua_State *L, const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  luaL_where(L, 1);
  lua_pushvfstring(L, fmt, argp);
  va_end(argp);
  lua_concat(L, 2);
  return lua_error(L);
}


/* }====================================================== */
extern int luaL_checkoption (lua_State *L, int narg, const char *def,
                                 const char *const lst[]) {
  const char *name = def ? luaL_optlstring(L, narg, def, NULL) :
                             luaL_checklstring(L, narg, NULL);
  int i;
  for (i=0; lst[i]; i++) {
    if (strcmp(lst[i], name) == 0) {
      return i;
    }
  }
  return luaL_argerror(L, narg,
                       lua_pushfstring(L, "invalid option " "'" "%s" "'", name));
}


extern int luaL_newmetatable (lua_State *L, const char *tname) {
  lua_getfield(L, LUA_REGISTRYINDEX, tname); /* get registry.name */
  if ( !lua_isnil(L, -1) ) { /* name already in use? */
    return 0; /* leave previous value on top, but return 0 */
  }

  /* lua_pop(L, 1); */
  lua_settop(L, -(1)-1);

  /* lua_newtable(L); */
  lua_createtable(L, 0, 0); /* create metatable */

  lua_pushvalue(L, -1);
  lua_setfield(L, LUA_REGISTRYINDEX, tname); /* registry.name = metatable */
  return 1;
}


extern void *luaL_checkudata (lua_State *L, int ud, const char *tname) {
  void *p = lua_touserdata(L, ud);
  if (p != NULL) { /* value is a userdata? */
    if (lua_getmetatable(L, ud)) { /* does it have a metatable? */
      lua_getfield(L, LUA_REGISTRYINDEX, tname); /* get correct metatable */
      if (lua_rawequal(L, -1, -2)) { /* does it have the correct mt? */
        /* lua_pop(L, 2); */
        lua_settop(L, -(2)-1); /* remove both metatables */

        return p;
      }
    }
  }
  luaL_typerror(L, ud, tname); /* else error */
  return NULL; /* to avoid warnings */
}


extern void luaL_checkstack (lua_State *L, int space, const char *mes) {
  if (!lua_checkstack(L, space)) {
    luaL_error(L, "stack overflow (%s)", mes);
  }
}


extern void luaL_checktype (lua_State *L, int narg, int t) {
  if (lua_type(L, narg) != t) {
    tag_error(L, narg, t);
  }
}


extern void luaL_checkany (lua_State *L, int narg) {
  if (lua_type(L, narg) == LUA_TNONE) {
    luaL_argerror(L, narg, "value expected");
  }
}


extern const char *luaL_checklstring (lua_State *L, int narg, size_t *len) {
  const char *s = lua_tolstring(L, narg, len);
  if (!s) {
    tag_error(L, narg, LUA_TSTRING);
  }
  return s;
}


extern const char *luaL_optlstring (lua_State *L, int narg,
                                        const char *def, size_t *len) {

  /* if (lua_isnoneornil(L, narg)) { */
  if ( lua_type(L, narg) <= 0 ) {
    if (len) {
      *len = (def ? strlen(def) : 0);
    }
    return def;
  }
  else {
    return luaL_checklstring(L, narg, len);
  }
}


extern lua_Number luaL_checknumber (lua_State *L, int narg) {
  lua_Number d = lua_tonumber(L, narg);
  if (d == 0 && !lua_isnumber(L, narg)) { /* avoid extra test when d is not 0 */
    tag_error(L, narg, LUA_TNUMBER);
  }
  return d;
}


extern lua_Number luaL_optnumber (lua_State *L, int narg, lua_Number def) {
  /* return luaL_opt(L, luaL_checknumber, narg, def); */
  return ( (lua_type(L, (narg)) <= 0) ? def : luaL_checknumber(L,narg) );
}


extern lua_Integer luaL_checkinteger (lua_State *L, int narg) {
  lua_Integer d = lua_tointeger(L, narg);
  if (d == 0 && !lua_isnumber(L, narg)) { /* avoid extra test when d is not 0 */
    tag_error(L, narg, LUA_TNUMBER);
  }
  return d;
}


extern lua_Integer luaL_optinteger (lua_State *L, int narg,
                                                      lua_Integer def) {
  /* return luaL_opt(L, luaL_checkinteger, narg, def); */
  return ( (lua_type(L, narg) <= 0) ? (def) : luaL_checkinteger(L,narg) );
}


extern int luaL_getmetafield (lua_State *L, int obj, const char *event) {
  if (!lua_getmetatable(L, obj)) { /* no metatable? */
    return 0;
  }
  lua_pushstring(L, event);
  lua_rawget(L, -2);
  /* if (lua_isnil(L, -1)) { */
  if ( lua_isnil(L, -1) )  {
    /* lua_pop(L, 2); */
    lua_settop(L, -(2)-1); /* remove metatable and metafield */
    return 0;
  }
  else {
    lua_remove(L, -2); /* remove only metatable */
    return 1;
  }
}


extern int luaL_callmeta (lua_State *L, int obj, const char *event) {
  /* obj = abs_index(L, obj); */
  obj = ( obj > 0 || obj <= LUA_REGISTRYINDEX  ? obj : lua_gettop(L) + obj + 1);
  if (!luaL_getmetafield(L, obj, event)) { /* no metafield? */
    return 0;
  }
  lua_pushvalue(L, obj);
  lua_call(L, 1, 1);
  return 1;
}


extern void (luaL_register) (lua_State *L, const char *libname,
                                const luaL_Reg *l) {
  luaL_openlib(L, libname, l, 0);
}


static int libsize (const luaL_Reg *l) {
  int size = 0;
  for (; l->name; l++) {
    size++;
  }
  return size;
}


extern void luaL_openlib (lua_State *L, const char *libname,
                              const luaL_Reg *l, int nup) {
  if (libname) {
    int size = libsize(l);
    /* check whether lib already exists */
	/* lauxlib.c:409
		extern const char *luaL_findtable (lua_State *L, int idx,
                                       const char *fname, int szhint) { ... }

		search a key's name "_LOADED"  inside the table    (&(L->l_G)->l_registry)

		Push a new empty table onto the stack :    {     }   ====>   (&(L->l_G)->l_registry)["_LOADED"]

	*/

    luaL_findtable(L, LUA_REGISTRYINDEX, "_LOADED", 1);
    lua_getfield(L, -1, libname); /* get _LOADED[libname] */
    if ( !lua_istable(L, -1) ) { /* not found? */
      /* lua_pop(L, 1); */
      lua_settop(L, -(1)-1); /* remove previous result */

      /* try global variable (and create one if it does not exist) */
      if (luaL_findtable(L, LUA_GLOBALSINDEX, libname, size) != NULL) {
        luaL_error(L, "name conflict for module " "'" "%s" "'", libname);
      }
      lua_pushvalue(L, -1);
      lua_setfield(L, -3, libname); /* _LOADED[libname] = new table */
    }
    lua_remove(L, -2); /* remove _LOADED table */
    lua_insert(L, -(nup+1)); /* move library table to below upvalues */
  }


  /*
	    ["coroutine"]["create"]   = &luaB_cocreate;
	    ["coroutine"]["resume"]   = &luaB_coresume;
	    ["coroutine"]["running"]  = &luaB_corunning;

		         ....

	    ["coroutine"]["yield"]  = &luaB_yield;
  */
  for (; l->name; l++) {
    int i;
    for (i=0; i<nup; i++) { /* copy upvalues to the top */
      lua_pushvalue(L, -nup);
    }
    lua_pushcclosure(L, l->func, nup);
    lua_setfield(L, -(nup+2), l->name);
  }

  /* lua_pop(L, nup); */
  lua_settop(L, -(nup)-1); /* remove upvalues */
}




/*
** {======================================================
** getn-setn: size for arrays
** =======================================================
*/

#if defined(LUA_COMPAT_GETN)

static int checkint (lua_State *L, int topop) {
  int n = (lua_type(L, -1) == LUA_TNUMBER) ? lua_tointeger(L, -1) : -1;
  lua_pop(L, topop);
  return n;
}


static void getsizes (lua_State *L) {
  lua_getfield(L, LUA_REGISTRYINDEX, "LUA_SIZES");
  if (lua_isnil(L, -1)) {  /* no `size' table? */
    lua_pop(L, 1);  /* remove nil */
    lua_newtable(L);  /* create it */
    lua_pushvalue(L, -1);  /* `size' will be its own metatable */
    lua_setmetatable(L, -2);
    lua_pushliteral(L, "kv");
    lua_setfield(L, -2, "__mode");  /* metatable(N).__mode = "kv" */
    lua_pushvalue(L, -1);
    lua_setfield(L, LUA_REGISTRYINDEX, "LUA_SIZES");  /* store in register */
  }
}


LUALIB_API void luaL_setn (lua_State *L, int t, int n) {
  t = abs_index(L, t);
  lua_pushliteral(L, "n");
  lua_rawget(L, t);
  if (checkint(L, 1) >= 0) {  /* is there a numeric field `n'? */
    lua_pushliteral(L, "n");  /* use it */
    lua_pushinteger(L, n);
    lua_rawset(L, t);
  }
  else {  /* use `sizes' */
    getsizes(L);
    lua_pushvalue(L, t);
    lua_pushinteger(L, n);
    lua_rawset(L, -3);  /* sizes[t] = n */
    lua_pop(L, 1);  /* remove `sizes' */
  }
}


LUALIB_API int luaL_getn (lua_State *L, int t) {
  int n;
  t = abs_index(L, t);
  lua_pushliteral(L, "n");  /* try t.n */
  lua_rawget(L, t);
  if ((n = checkint(L, 1)) >= 0) { 
    return n;
  }
  getsizes(L);  /* else try sizes[t] */
  lua_pushvalue(L, t);
  lua_rawget(L, -2);
  if ((n = checkint(L, 2)) >= 0) { 
    return n;
  }
  return (int)lua_objlen(L, t);
}

#endif

/* }====================================================== */



extern const char *luaL_gsub (lua_State *L, const char *s, const char *p,
                                                               const char *r) {
  const char *wild;
  size_t l = strlen(p);
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  while ((wild = strstr(s, p)) != NULL) {
    luaL_addlstring(&b, s, wild - s); /* push prefix */
    luaL_addstring(&b, r); /* push replacement in place of pattern */
    s = wild + l; /* continue after `p' */
  }
  luaL_addstring(&b, s); /* push last suffix */
  luaL_pushresult(&b);

  /* return lua_tostring(L, -1); */
  return lua_tolstring(L, (-1), NULL);
}

/**********************************************************************************************************************************************
We assume the `fname` argument's value is  "aaa.bbb.ccc"               |
                                                                       |
                                                                       |
Before execute :                                                       |   After execute :
                                                                       |                     
                   |                      |                            |                                                           |                      |   <==    L->top
                   |______________________|                            |                                                           |______________________|
                   |                      |  <==    L->top             |    (&(L->l_G)->l_registry)["aaa"]["bbb"]["ccc"] -->       |   {               }  |   
                   |______________________|                            |                                                           |______________________|
                   |   A certain Value    |                            |                                                           |   A certain Value    |
				   |______________________|                            |                                                           |______________________|
				   |       ......         |                            |                                                           |       ......         |
				                                                       |

***********************************************************************************************************************************************/
extern const char *luaL_findtable (lua_State *L, int idx,
                                       const char *fname, int szhint) {
  const char *e;
  lua_pushvalue(L, idx); // push the target table ( to be searched ) which is indicate by the given idx into the stack
  do {
	/* 
	    #include <string.h>

	    char*  strchr(const char* str, int ch);

	    在 str 这个字符串中，找出第1次 ch 出现的位置
        ----------------------------------------------------------------------------------------------------
        	如果找到了  :  则返回 ch 开头的子串
        	如果没有找到:  返回 NULL
        ----------------------------------------------------------------------------------------------------

	*/
    e = strchr(fname, '.');
    if (e == NULL) {
	  /* 
		 e -->  the null-terminated char  '\0'   

		 e.g. 

		      "_LOADED"

		 |_|L|O|A|D|E|D|\0|

		                ^^
						 e

	  */
      e = fname + strlen(fname);
    }

	/* e - fname = strlen(`substring`)   ==> The delta offset between the cursor of 'e' and the cursor of 'fname' */
    lua_pushlstring(L, fname, e - fname);
    lua_rawget(L, -2); /*  push  the value of table[fname] into stack ( if not existed , push `nil` ) and override the table[key]  into  `key`'s  #Slot  */
      /* lua_type(L, (-1)) == 0 */
    if ( lua_isnil(L, -1) ) { /* no such field? */
      /* lua_pop(L, 1); */
      lua_settop(L, -(1)-1); /* remove this nil */

	  /* 
			assume field is  "aaa.bbb.ccc"


			//                                a new table created by function :  lua_createtable(...)
			(&(L->l_G)->l_registry)["aaa"] = { }


		 -----------------
     -----  {        }     // lua_pushvalue(L,-2) which has the same address of  lua_createtable(...)
	 |	 -----------------
Same |        "aaa"        // lua_pushlstring(...)
	 |	 -----------------
	 |----> {         }    // lua_createtable
		 -----------------
			l_registry
		 -----------------


        After set :
			    (&(L->l_G)->l_registry)["aaa"] = { }


                       <----   L->top
	 	 -----------------
	       {         }    // lua_createtable
		 -----------------
			l_registry
		 -----------------
		       tmp
		 -----------------







	 	 -----------------
	                    <----    L->top
		 -----------------
		   {         }	
		 -----------------
		       tmp
		 -----------------




		 
		t1 = (&(L->l_G)->l_registry)["aaa"] = { }
		t2 =                      t1["bbb"] = { }
		t3 =                      t2["ccc"] = { }



		t3    ===>     (&(L->l_G)->l_registry)["aaa"]["bbb"]["ccc"] =    { }


	  */

      lua_createtable(L, 0, (*e == '.' ? 1 : szhint)); /* new table for field */
      lua_pushlstring(L, fname, e - fname);
      lua_pushvalue(L, -2);
      lua_settable(L, -4); /* set new table into field */
    }
    else if ( !lua_istable(L, -1) ) { /* field has a non-table value? */
      /* lua_pop(L, 2); */
      lua_settop(L, -(2)-1); /* remove table and value */

      return fname; /* return problematic part of the name */
    }
    lua_remove(L, -2); /* remove previous table */
    fname = e + 1;
  } while (*e == '.');
  return NULL;
}


/*
** {======================================================
** Generic Buffer manipulation
** =======================================================
*/


#define bufflen(B)	((B)->p - (B)->buffer)
#define bufffree(B)	((size_t)(LUAL_BUFFERSIZE - bufflen(B)))

#define LIMIT	(LUA_MINSTACK/2)


static int emptybuffer (luaL_Buffer *B) {
  /*               bufflen(B);        */
  size_t l = ((B)->p - (B)->buffer);
  if (l == 0) {
    return 0; /* put nothing on stack */
  }
  else {
    lua_pushlstring(B->L, B->buffer, l);
    B->p = B->buffer;
    B->lvl++;
    return 1;
  }
}


static void adjuststack (luaL_Buffer *B) {
  if (B->lvl > 1) {
    lua_State *L = B->L;
    int toget = 1; /* number of levels to concat */
    /*              lua_strlen(L, -1);    */
    size_t toplen = lua_objlen(L, -1);
    do {
      size_t l = lua_objlen(L, (-(toget+1)));
      /*                        LIMIT                */
      if (B->lvl - toget + 1 >= (20/2) || toplen > l) {
        toplen += l;
        toget++;
      }
      else {
        break;
      }
    } while (toget < B->lvl);
    lua_concat(L, toget);
    B->lvl = B->lvl - toget + 1;
  }
}


extern char *luaL_prepbuffer (luaL_Buffer *B) {
  if (emptybuffer(B)) {
    adjuststack(B);
  }
  return B->buffer;
}


extern void luaL_addlstring (luaL_Buffer *B, const char *s, size_t l) {
  while (l--) {
    /* luaL_addchar(B, *s++); */
    /* LUAL_BUFFERSIZE 8192 */
    ((void)((B)->p < ((B)->buffer + LUAL_BUFFERSIZE) || luaL_prepbuffer(B)), (*(B)->p++ = (char)(*s++)));
  }
}


extern void luaL_addstring (luaL_Buffer *B, const char *s) {
  luaL_addlstring(B, s, strlen(s));
}


extern void luaL_pushresult (luaL_Buffer *B) {
  emptybuffer(B);
  lua_concat(B->L, B->lvl);
  B->lvl = 1;
}


extern void luaL_addvalue (luaL_Buffer *B) {
  lua_State *L = B->L;
  size_t vl;
  const char *s = lua_tolstring(L, -1, &vl);
  /*                          bufffree(B)                
                                        8192
              #define LUAL_BUFFERSIZE  BUFSIZ
  */
  if (vl <= ((size_t)(LUAL_BUFFERSIZE - ((B)->p - (B)->buffer)))) { /* fit into buffer? */
    memcpy(B->p, s, vl); /* put it there */
    B->p += vl;
    /* lua_pop(L, 1); */
    lua_settop(L, -(1)-1); /* remove from stack */
  }
  else {
    if (emptybuffer(B)) {
      lua_insert(L, -2); /* put buffer before new value */
    }
    B->lvl++; /* add new value into B stack */
    adjuststack(B);
  }
}


extern void luaL_buffinit (lua_State *L, luaL_Buffer *B) {
  B->L = L;
  B->p = B->buffer;
  B->lvl = 0;
}


/* }====================================================== */


extern int luaL_ref (lua_State *L, int t) {
  int ref;
  /*      
        abs_index(L, t);  
        LUA_REGISTRYINDEX (-10000)
  */
  t = ( t > 0 || t <= LUA_REGISTRYINDEX ? t : lua_gettop(L) + t + 1 );
  if ( lua_isnil(L, -1) ) {
    /* lua_pop(L, 1); */
    lua_settop(L, -(1)-1); /* remove from stack */
    /*      LUA_REFNIL (-1) */
    return LUA_REFNIL; /* `nil' has a unique fixed reference */
  }
  /*               FREELIST_REF  0 */
  lua_rawgeti(L, t, FREELIST_REF); /* get first free element */
  ref = (int)lua_tointeger(L, -1); /* ref = t[FREELIST_REF] */
  /* lua_pop(L, 1); */
  lua_settop(L, -(1)-1); /* remove it from stack */
  if (ref != 0) { /* any free element? */
    lua_rawgeti(L, t, ref); /* remove it from list */
    /*              FREELIST_REF  0   */
    lua_rawseti(L, t, FREELIST_REF); /* (t[FREELIST_REF] = t[ref]) */
  }
  else { /* no free elements */
    ref = (int)lua_objlen(L, t);
    ref++; /* create new reference */
  }
  lua_rawseti(L, t, ref);
  return ref;
}


extern void luaL_unref (lua_State *L, int t, int ref) {
  if (ref >= 0) {
    /*               
                     abs_index(L, t);                              
        LUA_REGISTRYINDEX (-10000)
    */
    t = (t > 0 || t <= LUA_REGISTRYINDEX ? (t) : lua_gettop(L) + t + 1);
    /*               FREELIST_REF  0 */
    lua_rawgeti(L, t, FREELIST_REF);
    lua_rawseti(L, t, ref); /* t[ref] = t[FREELIST_REF] */
    lua_pushinteger(L, ref);
    /*               FREELIST_REF   0 */
    lua_rawseti(L, t, FREELIST_REF); /* t[FREELIST_REF] = ref */
  }
}


/*
** {======================================================
** Load functions
** =======================================================
*/

typedef struct LoadF {
  int extraline;
  FILE *f;
  /* 
                                 8192
      #define LUAL_BUFFERSIZE  BUFSIZ 

  */
  char buff[LUAL_BUFFERSIZE];
} LoadF;


static const char *getF (lua_State *L, void *ud, size_t *size) {
  LoadF *lf = (LoadF *)ud;
  (void)L;
  if (lf->extraline) {
    lf->extraline = 0;
    *size = 1;
    return "\n";
  }
  if (feof(lf->f)) {
    return NULL;
  }
  *size = fread(lf->buff, 1, sizeof(lf->buff), lf->f);
  return (*size > 0) ? lf->buff : NULL;
}


static int errfile (lua_State *L, const char *what, int fnameindex) {
  const char *serr = strerror(errno);
  /*                     lua_tostring(L, fnameindex)            */
  const char *filename = lua_tolstring(L, fnameindex, NULL) + 1;
  lua_pushfstring(L, "cannot %s %s: %s", what, filename, serr);
  lua_remove(L, fnameindex);

  /* 
      #define LUA_ERRERR      5               / * lua.h:47 * /
      #define LUA_ERRFILE     (LUA_ERRERR+1)  / * lauxlib.h:29 * /

  */

  /* return (5+1); */
  return LUA_ERRFILE;
}


extern int luaL_loadfile (lua_State *L, const char *filename) {
  LoadF lf;
  int status, readstatus;
  int c;
  int fnameindex = lua_gettop(L) + 1; /* index of filename on the stack */
  lf.extraline = 0;
  if (filename == NULL) {
    lua_pushliteral(L, "=stdin");
    /* lua_pushlstring(L, "" "=stdin", (sizeof("=stdin")/sizeof(char))-1); */
    lf.f = stdin;
  }
  else {
    lua_pushfstring(L, "@%s", filename);
    lf.f = fopen(filename, "r");
    if (lf.f == NULL) {
      return errfile(L, "open", fnameindex);
    }
  }
  c = getc(lf.f);
  if (c == '#') { /* Unix exec. file? */
    lf.extraline = 1;
    /*                         EOF  (-1)  */
    while ((c = getc(lf.f)) != EOF && c != '\n') { ; } /* skip first line */
    if (c == '\n') {
      c = getc(lf.f);
    }
  }

  /* #define	LUA_SIGNATURE	"\033Lua"    / * lua.h:27 * /           */
  if (c == LUA_SIGNATURE[0] && filename) { /* binary file? */
    lf.f = freopen(filename, "rb", lf.f); /* reopen in binary mode */
    if (lf.f == NULL) {
      return errfile(L, "reopen", fnameindex);
    }

    /*                        EOF  (-1)                     */
    /* #define	                          LUA_SIGNATURE	"\033Lua"    / * lua.h:27 * /           */
    /* skip eventual `#!...' */
    while ((c = getc(lf.f)) != (-1) && c != LUA_SIGNATURE[0]){ ; }
    lf.extraline = 0;
  }
  ungetc(c, lf.f);

  /*                              lua_tostring(L, -1)            */
  status = lua_load(L, getF, &lf, lua_tolstring(L, (-1), NULL));
  readstatus = ferror(lf.f);
  if (filename) {
    fclose(lf.f); /* close file (even in case of errors) */
  }
  if (readstatus) {
    lua_settop(L, fnameindex); /* ignore results from `lua_load' */
    return errfile(L, "read", fnameindex);
  }
  lua_remove(L, fnameindex);
  return status;
}


typedef struct LoadS {
  const char *s;
  size_t size;
} LoadS;


static const char *getS (lua_State *L, void *ud, size_t *size) {
  LoadS *ls = (LoadS *)ud;
  (void)L;
  if (ls->size == 0) {
    return NULL;
  }
  *size = ls->size;
  ls->size = 0;
  return ls->s;
}


extern int luaL_loadbuffer (lua_State *L, const char *buff, size_t size,
                                const char *name) {
  LoadS ls;
  ls.s = buff;
  ls.size = size;
  return lua_load(L, getS, &ls, name);
}


extern int (luaL_loadstring) (lua_State *L, const char *s) {
  return luaL_loadbuffer(L, s, strlen(s), s);
}



/* }====================================================== */

/*
   通过 nsize 参数的数值作为判定条件，realloc / free a block of memory

   为什么 用 realloc 函数，而不用 alloc 函数
   因为代码逻辑中，可能会在已经分配的内存地址处，[再次]在原地址处，重新分配，覆盖原有块的内存
   也可能用于扩容/缩减  数组占据的内存空间

arguments list
--------------------------------------------------
    void *ud     : unused
    void *ptr    : <the pointer address> to be alloced / release
    size_t osize : unused
    size_t nsize : the memory to be alloc (value > 0 ) /  release flag (value as 0)
--------------------------------------------------


*/
static void *l_alloc (void *ud, void *ptr, size_t osize, size_t nsize) {
  (void)ud;
  (void)osize;
  if (nsize == 0) {
    free(ptr); // 0 : =>   释放内存
    return NULL;
  }
  else {
    return realloc(ptr, nsize); // 非0 : => 动态分配内存
  }
}


static int panic (lua_State *L) {
  (void)L; /* to avoid warnings */
  fprintf(stderr, "PANIC: unprotected error in call to Lua API (%s)\n",
  /*                lua_tostring(L, -1)                              */
                   lua_tolstring(L, (-1), NULL));
  return 0;
}


extern lua_State *luaL_newstate (void) {
  // 使用 l_alloc  回调函数动态分配 lua_State 结构体的内存
  // l_alloc 函数在 此文件的 748 行 ( lauxlib.c:748 )
  // LUA_API  lua_State *lua_newstate (lua_Alloc f, void *ud) {  // lstate.c:233
  lua_State *L = lua_newstate(l_alloc, NULL);
  if (L) {
    lua_atpanic(L, &panic);
  }
  return L;
}


