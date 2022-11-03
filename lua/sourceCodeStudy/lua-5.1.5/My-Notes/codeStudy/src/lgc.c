/*
** $Id: lgc.c,v 2.38.1.2 2011/03/18 18:05:38 roberto Exp $
** Garbage Collector
** See Copyright Notice in lua.h
*/

#include <string.h>

#define lgc_c
#define LUA_CORE

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"


#define GCSTEPSIZE	1024u
#define GCSWEEPMAX	40
#define GCSWEEPCOST	10
#define GCFINALIZECOST	100


#define maskmarks	cast_byte(~(bitmask(BLACKBIT)|WHITEBITS))

#define makewhite(g,x)	\
   ((x)->gch.marked = cast_byte(((x)->gch.marked & maskmarks) | luaC_white(g)))

#define white2gray(x)	reset2bits((x)->gch.marked, WHITE0BIT, WHITE1BIT)
#define black2gray(x)	resetbit((x)->gch.marked, BLACKBIT)

#define stringmark(s)	reset2bits((s)->tsv.marked, WHITE0BIT, WHITE1BIT)


#define isfinalized(u)		testbit((u)->marked, FINALIZEDBIT)
#define markfinalized(u)	l_setbit((u)->marked, FINALIZEDBIT)


#define KEYWEAK         bitmask(KEYWEAKBIT)
#define VALUEWEAK       bitmask(VALUEWEAKBIT)



#define markvalue(g,o) { checkconsistency(o); \
  if (iscollectable(o) && iswhite(gcvalue(o))) reallymarkobject(g,gcvalue(o)); }

#define markobject(g,t) { if (iswhite(obj2gco(t))) \
		reallymarkobject(g, obj2gco(t)); }


#define setthreshold(g)  (g->GCthreshold = (g->estimate/100) * g->gcpause)


static void removeentry (Node *n) {
  ((void)0); /* lua_assert(ttisnil(gval(n))); */
   /*    iscollectable(gkey(n) */
  if (((((&(n)->i_key.nk))->tt) >= LUA_TSTRING )) {
    /*  
	 
	    setttype(gkey(n), LUA_TDEADKEY); 

        #define LUA_TTHREAD   8
		#define LAST_TAG	LUA_TTHREAD

		#define LUA_TDEADKEY	(LAST_TAG+3)

        LUA_TDEADKEY  (8+3)
	*/
    ((&(n)->i_key.nk))->tt = LUA_TDEADKEY; /* dead key; remove it */
  }
}


static void reallymarkobject (global_State *g, GCObject *o) {
  ((void)0); /* lua_assert(iswhite(o) && !isdead(g, o)); */
  /* white2gray(o); */
  o->gch.marked  &= ((lu_byte)(~( (1<<0) | (1<<1) )));

  switch (o->gch.tt) {
    case LUA_TSTRING: {
      return;
    }
    case LUA_TUSERDATA: {
      /*        =           gco2u(o)->metatable; */
      Table *mt = (&(&((o)->u))->uv)->metatable;
      /* gray2black(o); */
      (o->gch.marked) |= (1<<2); /* udata are never gray */

      if (mt) {
        /* markobject(g, mt); */
        /* ((((((GCObject *)((mt)))))->gch.marked) & ((((1<<(0)) | (1<<(1)))))) */
        { 
			if (   ( (GCObject *)(mt) )->gch.marked 
				 & ( (1<<0) | (1<<1) )  
			) { 
				reallymarkobject(g, (((GCObject *)((mt))))); 
			}
		};
      }

      /* markobject(g, gco2u(o)->env); */
      /* ((((((GCObject *)(((&(&((o)->u))->uv)->env)))))->gch.marked) & ((((1<<(0)) | (1<<(1)))))) */
      { 
		  if (  ( ( (GCObject *)((&(&((o)->u))->uv)->env) ))->gch.marked 
			 & ( (1<<0) | (1<<1) ) 
		  ) { 
			  reallymarkobject(g, (GCObject *)( (&(&((o)->u))->uv)->env) ); 
		  }
	  };
      return;
    }
    case LUA_TUPVAL: {
      /*           gco2uv(o); */
      UpVal *uv = (&((o)->uv));

      /* markvalue(g, uv->v); */
      { 
		  ((void)0); 
		  if (    (uv->v)->tt >= LUA_TSTRING 
		      && ( (((uv->v)->value.gc)->gch.marked) & ( (1<<0) | (1<<1) ) )
		  ) { 
			  reallymarkobject(g,((uv->v)->value.gc)); 
		  } 
	  };

      if (uv->v == &uv->u.value) { /* closed? */
        /* gray2black(o); */
        o->gch.marked |= (1<<2); /* open upvalues are never black */
      }
      return;
    }
    case LUA_TFUNCTION: {
      /* gco2cl(o)->c.gclist = g->gray; */
      (&((o)->cl))->c.gclist = g->gray;
      g->gray = o;
      break;
    }
    case LUA_TTABLE: {
      /* gco2h(o)->gclist = g->gray; */
      (&((o)->h))->gclist = g->gray;
      g->gray = o;
      break;
    }
    case LUA_TTHREAD: {
      /* gco2th(o)->gclist = g->gray; */
      (&((o)->th))->gclist = g->gray;
      g->gray = o;
      break;
    }
    case LUA_TPROTO: {
      /* gco2p(o)->gclist = g->gray; */
      (&((o)->p))->gclist = g->gray;
      g->gray = o;
      break;
    }
    default: {
      ((void)0);  /* lua_assert(0); */
    }
  }
}


static void marktmu (global_State *g) {
  GCObject *u = g->tmudata;
  if (u) {
    do {
      u = u->gch.next;
      /* makewhite(g, u); */
      u->gch.marked = (lu_byte)( (u->gch.marked 
	          & ((lu_byte)(~( (1<<2) | ( (1<<0) | (1<<1) ) )))) 
			  | ( (lu_byte)(g->currentwhite & ( (1<<0) | (1<<1) )) )
	  ); /* may be marked, if left from previous GC */

      reallymarkobject(g, u);
    } while (u != g->tmudata);
  }
}


/* move `dead' udata that need finalization to list `tmudata' */
size_t luaC_separateudata (lua_State *L, int all) {
  global_State *g = G(L); /* (L->l_G); */
  size_t deadmem = 0;
  GCObject **p = &g->mainthread->next;
  GCObject *curr;
  while ((curr = *p) != NULL) {
    /*        iswhite(curr)                                  || all */
    if ( !( (((curr)->gch.marked) & ( (1<<0) | (1<<1) )) || all ) 
	     || ( ((&(&((curr)->u))->uv)->marked) & (1<<3) ) 
	) 
	{
      p = &curr->gch.next; /* don't bother with them */
    /*          fasttm(L, gco2u(curr)->metatable, TM_GC)                                                                                                                                                                  == NULL */
	/* (((&(&((curr)->u))->uv)->metatable) == NULL 
	 * ? NULL 
	 * : (((&(&((curr)->u))->uv)->metatable)->flags & (1u<<(TM_GC))) 
	 *	   ? NULL : 
	 *	   luaT_gettm((&(&((curr)->u))->uv)->metatable, TM_GC, ((L->l_G))->tmname[TM_GC]))                  
	*/


    } else if ( 
	    ( (&(&(curr->u))->uv)->metatable == NULL 
	  ? NULL 
	  : ( ( (&(&(curr->u))->uv)->metatable->flags & ( 1u << TM_GC ) ) 
	 	  ? NULL 
		  : luaT_gettm( (&(&(curr->u))->uv)->metatable, TM_GC, G(L)->tmname[TM_GC]) 
		)
	 ) == NULL ) {
      /* markfinalized(gco2u(curr)); */
      (&(&((curr)->u))->uv)->marked |= (1<<3); /* don't need finalization */
      p = &curr->gch.next;
    }
    else { /* must call its gc method */
      /*          sizeudata(gco2u(curr)); */
      deadmem += (sizeof(union Udata)+((&(&((curr)->u))->uv))->len);
      /* markfinalized(gco2u(curr)); */
      (&(&((curr)->u))->uv)->marked |= (1<<3);

      *p = curr->gch.next;
      /* link `curr' at the end of `tmudata' list */
      if (g->tmudata == NULL) { /* list is empty? */
        g->tmudata = curr->gch.next = curr; /* creates a circular list */
      } else {
        curr->gch.next = g->tmudata->gch.next;
        g->tmudata->gch.next = curr;
        g->tmudata = curr;
      }
    }
  }
  return deadmem;
}


static int traversetable (global_State *g, Table *h) {
  int i;
  int weakkey = 0;
  int weakvalue = 0;
  const TValue *mode;
  if (h->metatable) {
    /* markobject(g, h->metatable); */
    { 
		if ( ( ( (GCObject *)(h->metatable) )->gch.marked ) 
			& ( (1<<0) | (1<<1) ) ) 
		{ 
			reallymarkobject(g, (GCObject *)(h->metatable) ); 
		}
	};
  }

  /* mode = gfasttm(g, h->metatable, TM_MODE); */
  mode = ( h->metatable == NULL 
		  ? NULL 
		  : ( ( h->metatable->flags & (1u<<(TM_MODE)) ) 
		      ? NULL 
			  : luaT_gettm(h->metatable, TM_MODE, g->tmname[TM_MODE]) 
			)
  );
  /*           ttisstring(mode) */
  if ( mode &&  (mode->tt) == LUA_TSTRING  ) { /* is there a weak mode? */
    /*                  svalue(mode),                                  'k') != NULL) */
    weakkey = (strchr(((const char *)(((&(mode)->value.gc->ts)) + 1)), 'k') != NULL);
    /*                  svalue(mode)                                     'v'  != NULL */
    weakvalue = (strchr(((const char *)(((&(mode)->value.gc->ts)) + 1)), 'v') != NULL);

    if (weakkey || weakvalue) { /* is really weak? */
      /*          ~(KEYWEAK | VALUEWEAK); */
      h->marked &= ~((1<<3) | (1<<4)); /* clear bits */

	  /*
      h->marked |= cast_byte((weakkey << KEYWEAKBIT) |
                             (weakvalue << VALUEWEAKBIT));
           KEYWEAKBIT    3
           VALUEWEAKBIT  4
	  */
      h->marked |= (lu_byte)( (weakkey << KEYWEAKBIT) | (weakvalue << VALUEWEAKBIT) );
      h->gclist = g->weak; /* must be cleared after GC, ... */
      /*             obj2gco(h); */
      g->weak = ((GCObject *)h); /* ... so put in the appropriate list */
    }
  }
  if (weakkey && weakvalue) {
    return 1;
  }
  if (!weakvalue) {
    i = h->sizearray;
    while (i--) {
      /* markvalue(g, &h->array[i]); */
      { 
		  ((void)0); 
		  if ( ( (&h->array[i])->tt >= LUA_TSTRING) && ( ( ((&h->array[i])->value.gc)->gch.marked) & ( (1<<0) | (1<<1) ) ) ) { 
			  reallymarkobject(g,((&h->array[i])->value.gc)); 
		  }
	  };
    }
  }
  /*    sizenode(h); */
  i = (1<< (h->lsizenode) );
  while (i--) {
    /*         gnode(h, i); */
    Node *n = (&(h)->node[i]);
    ((void)0); /* lua_assert(ttype(gkey(n)) != LUA_TDEADKEY || ttisnil(gval(n))); */
    /*      ttisnil(gval(n) ) */
    if ( ((&(n)->i_val))->tt == LUA_TNIL ) {
      removeentry(n); /* remove empty entries */
    } else {
      ((void)0); /* lua_assert(!ttisnil(gkey(n))); */
      if (!weakkey) {
        /* markvalue(g, gkey(n)); */
        {
			((void)0); 
			if ( ( (&(n)->i_key.nk)->tt >= LUA_TSTRING) && ( ( ( (&(n)->i_key.nk)->value.gc)->gch.marked) & ( (1<<0) | (1<<1) ) ) ) {
				reallymarkobject(g, (&(n)->i_key.nk)->value.gc); 
			}
		};
      }
      if (!weakvalue) {
        /* markvalue(g, gval(n)); */
        { 
			((void)0); 
			if ( ((&(n)->i_val)->tt >= LUA_TSTRING) && ( (&(n)->i_val)->value.gc->gch.marked & ( (1<<0) | (1<<1) ) ) ) { 
				reallymarkobject(g, (&(n)->i_val)->value.gc );
			}
		};
      }
    }
  }
  return weakkey || weakvalue;
}


/*
** All marks are conditional because a GC may happen while the
** prototype is still being created
*/
static void traverseproto (global_State *g, Proto *f) {
  int i;
  if (f->source) {
    /* stringmark(f->source); */
    f->source->tsv.marked &= (lu_byte)(~( (1<<0) | (1<<1) ));
  }
  for (i=0; i<f->sizek; i++) { /* mark literals */
    /* markvalue(g, &f->k[i]); */
    { 
		((void)0); 
		if (( (&f->k[i])->tt >= LUA_TSTRING) && ( ((&f->k[i])->value.gc)->gch.marked & ( (1<<0) | (1<<1) ) ) ) { 
			reallymarkobject(g, (&f->k[i])->value.gc ); 
		}
	};
  }
  for (i=0; i<f->sizeupvalues; i++) { /* mark upvalue names */
    if (f->upvalues[i]) {
      /* stringmark(f->upvalues[i]); */
      f->upvalues[i]->tsv.marked &=  (lu_byte)(~( (1<<0) | (1<<1) ));
    }
  }
  for (i=0; i<f->sizep; i++) { /* mark nested protos */
    if (f->p[i]) {
      /* markobject(g, f->p[i]); */
      { 
		  if (  ( (GCObject *)(f->p[i]) )->gch.marked & ( (1<<0) | (1<<1) ) )  { 
			  reallymarkobject(g, (GCObject *)(f->p[i]) ); 
		  }
	  };
    }
  }
  for (i=0; i<f->sizelocvars; i++) { /* mark local-variable names */
    if (f->locvars[i].varname) {
      /* stringmark(f->locvars[i].varname); */
       (f->locvars[i].varname)->tsv.marked &=  (lu_byte)(~( (1<<0) | (1<<1) )) ;
    }
  }
}


static void traverseclosure (global_State *g, Closure *cl) {
  /* markobject(g, cl->c.env); */
  { 
	  if (  ( (GCObject *)(cl->c.env) )->gch.marked & ( (1<<0) | (1<<1) )  ) { 
		  reallymarkobject(g, ( (GCObject *)(cl->c.env) ) ); 
	  }
  };
  if (cl->c.isC) {
    int i;
    for (i=0; i<cl->c.nupvalues; i++) { /* mark its upvalues */
      /* markvalue(g, &cl->c.upvalue[i]); */
      { 
		  ((void)0); 
		  if ( (&cl->c.upvalue[i])->tt >= LUA_TSTRING && ( ((&cl->c.upvalue[i])->value.gc)->gch.marked & ( (1<<0) | (1<<1) ) ) ) { 
			  reallymarkobject(g, (&cl->c.upvalue[i])->value.gc ); 
		  }
	  };
    }
  }
  else {
    int i;
    ((void)0); /* lua_assert(cl->l.nupvalues == cl->l.p->nups); */
    /* markobject(g, cl->l.p); */
    { 
		if ( ( ( (GCObject *)(cl->l.p) )->gch.marked & ( (1<<0) | (1<<1) ) ) ) { 
			reallymarkobject(g, (GCObject *)(cl->l.p) ); 
		}
	};
    for (i=0; i<cl->l.nupvalues; i++) { /* mark its upvalues */
      /* markobject(g, cl->l.upvals[i]); */
      { 
		  if (   ((GCObject *)(cl->l.upvals[i]) )->gch.marked & ( (1<<0) | (1<<1) )  ) { 
			  reallymarkobject(g,  (GCObject *)(cl->l.upvals[i]) ); 
		  }
	  };
    }
  }
}


static void checkstacksizes (lua_State *L, StkId max) {
  /*            cast_int(L->ci - L->base_ci); */
  int ci_used = (int)(L->ci - L->base_ci); /* number of `ci' in use */
  /*            cast_int(max - L->stack); */
  int s_used = (int)(max - L->stack); /* part of stack in use */
  /*              LUAI_MAXCALLS 20000 */
  if (L->size_ci > LUAI_MAXCALLS ) { /* handling overflow? */
    return; /* do not touch the stacks */
  }
  /*                             BASIC_CI_SIZE 8 */
  if ( 4*ci_used < L->size_ci && 2*BASIC_CI_SIZE < L->size_ci ) {
    luaD_reallocCI(L, L->size_ci/2); /* still big enough... */
  }
  ((void)0); /* condhardstacktests(luaD_reallocCI(L, ci_used + 1)); */

  if ( 4*s_used < L->stacksize 
      /*   
         BASIC_STACK_SIZE (2*20)
         EXTRA_STACK  5
      */
      &&   2*((2*20)+5) < L->stacksize) {
    luaD_reallocstack(L, L->stacksize/2); /* still big enough... */
  }
  ((void)0); /* condhardstacktests(luaD_reallocstack(L, s_used)); */
}


static void traversestack (global_State *g, lua_State *l) {
  StkId o, lim;
  CallInfo *ci;
  /* markvalue(g, gt(l)); */
  { 
	  ((void)0); /* checkconsistency(o); */
	  if ( ( gt(l)->tt >= LUA_TSTRING) && ( ((&l->l_gt)->value.gc)->gch.marked & ( (1<<0) | (1<<1) )) ) { 
		  reallymarkobject(g, (&l->l_gt)->value.gc );
	  }
  };

  lim = l->top;
  for (ci = l->base_ci; ci <= l->ci; ci++) {
    ((void)0); /* lua_assert(ci->top <= l->stack_last); */
    if (lim < ci->top) {
      lim = ci->top;
    }
  }
  for (o = l->stack; o < l->top; o++) {
    /* markvalue(g, o); */
    { 
		((void)0);  /* checkconsistency(o); */
		if ( ( o->tt >= LUA_TSTRING ) && ( (o->value.gc)->gch.marked & ( (1<<0) | (1<<1) ) ) ) { 
			reallymarkobject(g, o->value.gc); 
		}
	};
  }
  for (; o <= lim; o++) {
    /* setnilvalue(o); */
    o->tt = LUA_TNIL;
  }
  checkstacksizes(l, lim);
}


/*
** traverse one gray object, turning it to black.
** Returns `quantity' traversed.
*/
static l_mem propagatemark (global_State *g) {
  GCObject *o = g->gray;
  ((void)0); /* lua_assert(isgray(o)); */
  o->gch.marked |=  (1<<2);
  switch (o->gch.tt) {
    case LUA_TTABLE: {
      Table *h = &(o->h); /* gco2h(o); */
      g->gray = h->gclist;
      if (traversetable(g, h)) { /* table is weak? */
        /* black2gray(o); */
        o->gch.marked &= (lu_byte)( ~(1<<2) ); /* keep it gray */
      }
      return sizeof(Table) + sizeof(TValue) * h->sizearray +
      /*                                        sizenode(h); */
                             sizeof(Node) * ( 1<< (h->lsizenode) );
    }
    case LUA_TFUNCTION: {
      Closure *cl = &(o->cl); /* gco2cl(o); */
      g->gray = cl->c.gclist;
      traverseclosure(g, cl);
      return (cl->c.isC) 
          /*    sizeCclosure(cl->c.nupvalues)           */
		  ? ( (int)(sizeof(CClosure)) + (int)(sizeof(TValue)*(cl->c.nupvalues-1)))
          /* sizeLclosure(cl->l.nupvalues) */
          : ( (int)(sizeof(LClosure)) + (int)(sizeof(TValue *)*(cl->l.nupvalues-1)));
    }
    case LUA_TTHREAD: {
      lua_State *th = (&((o)->th)); /* gco2th(o); */
      g->gray = th->gclist;
      th->gclist = g->grayagain;
      g->grayagain = o;
      /* black2gray(o); */
      o->gch.marked &= (lu_byte)(~(1<<2));
      traversestack(g, th);
      return sizeof(lua_State) + sizeof(TValue) * th->stacksize +
                                 sizeof(CallInfo) * th->size_ci;
    }
    case LUA_TPROTO: { /* (8 +1) */
      Proto *p = &(o->p); /* gco2p(o); */
      g->gray = p->gclist;
      traverseproto(g, p);
      return sizeof(Proto) + sizeof(Instruction) * p->sizecode +
                             sizeof(Proto *) * p->sizep +
                             sizeof(TValue) * p->sizek +
                             sizeof(int) * p->sizelineinfo +
                             sizeof(LocVar) * p->sizelocvars +
                             sizeof(TString *) * p->sizeupvalues;
    }
    default: {
      ((void)0); /* lua_assert(0); */
      return 0;
    }
  }
}


static size_t propagateall (global_State *g) {
  size_t m = 0;
  while (g->gray) {
    m += propagatemark(g);
  }
  return m;
}

/*
** The next function tells whether a key or value can be cleared from
** a weak table. Non-collectable objects are never removed from weak
** tables. Strings behave as `values', so are never removed too. for
** other objects: if really collected, cannot keep them; for userdata
** being finalized, keep them in keys, but not in values
*/
static int iscleared (const TValue *o, int iskey) {
  /*  !iscollectable(o) */
  if (!( o->tt >= LUA_TSTRING ) ) {
    return 0;
  }
  /*    ttisstring(o) */
  if ( o->tt == LUA_TSTRING  ) {
    /* stringmark(rawtsvalue(o)); */
    (&(o)->value.gc->ts)->tsv.marked &= ( (lu_byte)(~( (1<<0) | (1<<1) ) ) ); /* strings are `values', so are never weak */
    return 0;
  }
  /*       iswhite(gcvalue(o))                            */
  return ( (o->value.gc)->gch.marked & ( (1<<0) | (1<<1) ) ) ||
    /* ttisuserdata(o)                        isfinalized(uvalue(o)                                 */
    ( (o->tt == LUA_TUSERDATA) && (!iskey && ((&(&(o)->value.gc->u)->uv)->marked) & (1<<3)));
}


/*
** clear collected entries from weaktables
*/
static void cleartable (GCObject *l) {
  while (l) {
    Table *h = &(l->h); /* gco2h(l); */
    int i = h->sizearray;
    /*
    lua_assert(testbit(h->marked, VALUEWEAKBIT) ||
               testbit(h->marked, KEYWEAKBIT));
	*/

    ((void)0);
    /*  testbit(h->marked, VALUEWEAKBIT) */
    if ( h->marked & (1<<4) ) {
      while (i--) {
        TValue *o = &h->array[i];
        if (iscleared(o, 0)) { /* value was collected? */
          /* setnilvalue(o); */
          o->tt = LUA_TNIL; /* remove value */
        }
      }
    }
    /*    sizenode(h); */
    i = ( 1<< (h->lsizenode) );
    while (i--) {
      /*        gnode(h, i); */
      Node *n = (&(h)->node[i]);
      /*         ttisnil(gval(n) */
      if ( !( (&(n)->i_val)->tt == LUA_TNIL ) && /* non-empty entry? */
          /*          key2tval(n)                        gval(n) */
          (iscleared((&(n)->i_key.tvk), 1) || iscleared(&(n)->i_val, 0))) {

        /* setnilvalue(gval(n)); */
        (&(n)->i_val)->tt = LUA_TNIL; /* remove value ... */
        removeentry(n); /* remove entry from table */
      }
    }
    l = h->gclist;
  }
}


static void freeobj (lua_State *L, GCObject *o) {
  switch (o->gch.tt) {
    case LUA_TPROTO: { /* LUA_TPROTO (8 +1) */
      /*                 gco2p(o) */
      luaF_freeproto(L, &(o->p) );
      break;
    }
    case LUA_TFUNCTION: {
      /*                   gco2cl(o) */
      luaF_freeclosure(L, &(o->cl) );
      break;
    }
    case LUA_TUPVAL: { /* LUA_TUPVAL (8 +2) */
      /*                gco2uv(o) */
      luaF_freeupval(L, &(o->uv) );
      break;
    }
    case LUA_TTABLE: {
      /*           gco2h(o) */
      luaH_free(L, &(o->h) );
      break;
    }
    case LUA_TTHREAD: {
      ((void)0); /* lua_assert(gco2th(o) != L && gco2th(o) != G(L)->mainthread); */
      /*                   gco2th(o) */
      luaE_freethread(L, &(o->th) );
      break;
    }
    case LUA_TSTRING: {
      /* (L->l_G)  */
      G(L)->strt.nuse--;
      /*                      sizestring(gco2ts(o)  */
      luaM_realloc_(L, o, (sizeof(union TString) + ((&(&(o->ts))->tsv)->len+1) *sizeof(char)), 0);
      break;
    }
    case LUA_TUSERDATA: {
      /*                      sizeudata(gco2u(o) */
      luaM_realloc_(L, o, (sizeof(union Udata)+(&(&(o->u))->uv)->len), 0);
      break;
    }
    default: {
      ((void)0); /* lua_assert(0); */
    }
  }
}


#define sweepwholelist(L,p)	sweeplist(L,p,MAX_LUMEM)


static GCObject **sweeplist (lua_State *L, GCObject **p, lu_mem count) {
  GCObject *curr;
  global_State *g = G(L); /*   (L->l_G)  */
  /*              otherwhite(g) */
  int deadmask = g->currentwhite ^ ( (1<<0) | (1<<1) ) ;
  while ( (curr = *p) != NULL && count-- > 0) {
    /*                  LUA_TTHREAD 8 */
    if (curr->gch.tt == LUA_TTHREAD ) { /* sweep open upvalues of each thread */
      /* sweepwholelist(L, &gco2th(curr)->openupval); */
      sweeplist(L,&(&(curr->th))->openupval, ((lu_mem)(~(lu_mem)0)-2));
    }
    /*                    ^ WHITEBITS             */
    if ( ( curr->gch.marked ^ ( (1<<0) | (1<<1) ) ) & deadmask) { /* not dead? */
      ((void)0); /* lua_assert(!isdead(g, curr) || testbit(curr->gch.marked, FIXEDBIT)); */

      curr->gch.marked = (lu_byte)( 
			  ( curr->gch.marked 
				&  ( (lu_byte)(~( (1<<2) | ( (1<<0) | (1<<1) ) ) ) ) 
			  ) 
			  | ( (lu_byte)(g->currentwhite & ( (1<<0) | (1<<1) ) ) ) 
	 ); /* make it white (for next cycle) */
      p = &curr->gch.next;
    }
    else { /* must erase `curr' */
      ((void)0); /* lua_assert(isdead(g, curr) || deadmask == bitmask(SFIXEDBIT)); */
      *p = curr->gch.next;
      if (curr == g->rootgc) { /* is the first element of the list? */
        g->rootgc = curr->gch.next; /* adjust first */
      }
      freeobj(L, curr);
    }
  }
  return p;
}


static void checkSizes (lua_State *L) {
  global_State *g = L->l_G; /* G(L); */
  /* check size of string hash */
  /*                   cast(lu_int32, g->strt.size/4) */
  if ( g->strt.nuse < (lu_int32)(g->strt.size/4) &&
  /*    MINSTRTABSIZE 32    */
       g->strt.size > 32*2) {
    luaS_resize(L, g->strt.size/2); /* table is too big */
  }
  /* check size of buffer */
  /*   luaZ_sizebuffer(&g->buff) > LUA_MINBUFFER 32 */
  if ( (&g->buff)->buffsize > 32*2 ) { /* buffer too big? */
    /*                luaZ_sizebuffer(&g->buff) */
    size_t newsize = (&g->buff)->buffsize / 2;
    /* luaZ_resizebuffer(L, &g->buff, newsize); */
    ( ( (&g->buff)->buffer = (char *)( ( (  (size_t)(newsize+1) <= MAX_SIZET /(sizeof(char))) 
									   ? luaM_realloc_(L, (&g->buff)->buffer, (&g->buff)->buffsize * sizeof(char), newsize * sizeof(char)  ) 
									   : luaM_toobig(L) ) ) ),
	  (&g->buff)->buffsize = newsize
	);
  }
}


static void GCTM (lua_State *L) {
  global_State *g = (L->l_G); /* G(L) */
  GCObject *o = g->tmudata->gch.next; /* get first element */
  /*             rawgco2u(o); */
  Udata *udata = (&((o)->u));
  const TValue *tm;
  /* remove udata from `tmudata' */
  if (o == g->tmudata) { /* last element? */
    g->tmudata = NULL;
  } else {
    g->tmudata->gch.next = udata->uv.next;
  }
  udata->uv.next = g->mainthread->next; /* return it to `root' list */
  g->mainthread->next = o;
  /* makewhite(g, o); */
  o->gch.marked = (lu_byte)( (o->gch.marked & ((lu_byte)(~( (1<<2) | ( (1<<0) | (1<<1) ) ) ) )) | ( (lu_byte)(g->currentwhite & ( (1<<0) | (1<<1) ) ) ) );

  /*     fasttm(L, udata->uv.metatable, TM_GC); */
  tm = ( udata->uv.metatable == NULL 
		  ? NULL
		  : ( udata->uv.metatable->flags & (1u<<TM_GC) ) 
			? NULL 
			: luaT_gettm(udata->uv.metatable, TM_GC,  G(L)->tmname[TM_GC] ) );
  if (tm != NULL) {
    lu_byte oldah = L->allowhook;
    lu_mem oldt = g->GCthreshold;
    L->allowhook = 0; /* stop debug hooks during GC tag method */
    g->GCthreshold = 2*g->totalbytes; /* avoid GC steps */

    /* setobj2s(L, L->top, tm); */
    { const TValue *o2=(tm); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };

    /* setuvalue(L, L->top+1, udata); */
    { TValue *i_o=(L->top+1); i_o->value.gc=((GCObject *)((udata))); i_o->tt=7; ((void)0); };

    L->top += 2;
    luaD_call(L, L->top - 2, 0);
    L->allowhook = oldah; /* restore hooks */
    g->GCthreshold = oldt; /* restore threshold */
  }
}


/*
** Call all GC tag methods
*/
void luaC_callGCTM (lua_State *L) {
  /*      (L->l_G)  */
  while ( G(L)->tmudata ) {
    GCTM(L);
  }
}


void luaC_freeall (lua_State *L) {
  /*                (L->l_G)  */
  global_State *g = G(L);
  int i;
  /*                  WHITEBITS */
  g->currentwhite = ( (1<<0) | (1<<1) ) | (1<<6); /* mask to collect all elements */

  /* sweepwholelist(L, &g->rootgc); */
  sweeplist(L,&g->rootgc, (lu_mem)(~(lu_mem)0) - 2 );
  for (i = 0; i < g->strt.size; i++) { /* free all string lists */
    /* sweepwholelist(L, &g->strt.hash[i]); */
    sweeplist(L,&g->strt.hash[i], (lu_mem)(~(lu_mem)0) -2);
  }
}


static void markmt (global_State *g) {
  int i;
  /*          (8 +1)        */
  for (i=0; i<NUM_TAGS; i++) {
    if (g->mt[i]) {
      /* markobject(g, g->mt[i]); */
      { 
		  if ( ( (GCObject *)(g->mt[i]))->gch.marked & ( (1<<0) | (1<<1) )  ) { 
			  reallymarkobject(g, (GCObject *)(g->mt[i]) ); 
		  }
	  };
    }
  }
}


/* mark root set */
static void markroot (lua_State *L) {
  /*                 G(L) */
  global_State *g = (L->l_G);
  g->gray = NULL;
  g->grayagain = NULL;
  g->weak = NULL;

  /* markobject(g, g->mainthread); */
  { 
	  if ( ( ( (GCObject *)(g->mainthread) )->gch.marked & ( (1<<0) | (1<<1) ))) { 
		  reallymarkobject(g, (GCObject *)(g->mainthread) ); 
	  }
  };
  /* make global table be traversed before main stack */
  /* markvalue(g, gt(g->mainthread)); */
  { 
	  ((void)0); 
	  if (  (&g->mainthread->l_gt)->tt >= LUA_TSTRING && (  ((&g->mainthread->l_gt)->value.gc)->gch.marked & (  (1<<0) | (1<<1)  ) ) ) { 
		  reallymarkobject(g, (&g->mainthread->l_gt)->value.gc ); 
	  }
  };


  /* markvalue(g, registry(L)); */
  { 
	  ((void)0); 
	  if ( (&(L->l_G)->l_registry)->tt >= LUA_TSTRING && ( ( (&(L->l_G)->l_registry)->value.gc)->gch.marked & ( (1<<0) | (1<<1) ) ) ) { 
		  reallymarkobject(g, (&(L->l_G)->l_registry)->value.gc); 
	  }
  };
  markmt(g);
  /*           GCSpropagate  1 */
  g->gcstate = GCSpropagate;
}


static void remarkupvals (global_State *g) {
  UpVal *uv;
  for (uv = g->uvhead.u.l.next; uv != &g->uvhead; uv = uv->u.l.next) {
    ((void)0); /* lua_assert(uv->u.l.next->u.l.prev == uv && uv->u.l.prev->u.l.next == uv); */
    /*    isgray(obj2gco(uv) */
    if ((!((((((GCObject *)((uv)))))->gch.marked) & ((1<<(2)))) && !((((((GCObject *)((uv)))))->gch.marked) & ((((1<<(0)) | (1<<(1)))))))) {

      /* markvalue(g, uv->v); */
      { 
		  ((void)0); 
		  if ( uv->v->tt >= LUA_TSTRING && (  uv->v->value.gc->gch.marked & ( (1<<0) | (1<<1) ) ) ) { 
			  reallymarkobject(g, uv->v->value.gc ); 
		  }
	  };
    }
  }
}


static void atomic (lua_State *L) {
  /*                 G(L);   */
  global_State *g = (L->l_G);
  size_t udsize; /* total size of userdata to be finalized */
  /* remark occasional upvalues of (maybe) dead threads */
  remarkupvals(g);
  /* traverse objects cautch by write barrier and by 'remarkupvals' */
  propagateall(g);
  /* remark weak tables */
  g->gray = g->weak;
  g->weak = NULL;
  ((void)0); /* lua_assert(!iswhite(obj2gco(g->mainthread))); */

  /* markobject(g, L); */
  { 
	  if ( ( ( ((GCObject *)L)->gch.marked) & ( (1<<0) | (1<<1) ) ) ) { 
		  reallymarkobject(g, (GCObject *)L ); 
	  }
  }; /* mark running thread */

  markmt(g); /* mark basic metatables (again) */
  propagateall(g);
  /* remark gray again */
  g->gray = g->grayagain;
  g->grayagain = NULL;
  propagateall(g);
  udsize = luaC_separateudata(L, 0); /* separate userdata to be finalized */
  marktmu(g); /* mark `preserved' userdata */
  udsize += propagateall(g); /* remark, to propagate `preserveness' */
  cleartable(g->weak); /* remove collected objects from weak tables */
  /* flip current white */
  /*                 cast_byte(otherwhite(g)); */
  g->currentwhite = (lu_byte)(g->currentwhite ^ ( (1<<0) | (1<<1) ) );
  g->sweepstrgc = 0;
  g->sweepgc = &g->rootgc;
  g->gcstate = GCSsweepstring; /* GCSsweepstring 2 */
  g->estimate = g->totalbytes - udsize; /* first estimate */
}


static l_mem singlestep (lua_State *L) {
  /*                G(L)  */
  global_State *g = (L->l_G);
  /*lua_checkmemory(L);*/
  switch (g->gcstate) {
    case GCSpause: {
      markroot(L); /* start a new collection */
      return 0;
    }
    case GCSpropagate: {
      if (g->gray) {
        return propagatemark(g);
      } else { /* no more `gray' objects */
        atomic(L); /* finish mark phase */
        return 0;
      }
    }
    case GCSsweepstring: {
      lu_mem old = g->totalbytes;
      /* sweepwholelist(L, &g->strt.hash[g->sweepstrgc++]); */
      sweeplist(L,&g->strt.hash[g->sweepstrgc++],(lu_mem)(~(lu_mem)0) - 2 );
      if (g->sweepstrgc >= g->strt.size) { /* nothing more to sweep? */
        g->gcstate = GCSsweep; /* GCSsweep 3 */  /* end sweep-string phase */
      }
      ((void)0); /* lua_assert(old >= g->totalbytes); */
      g->estimate -= old - g->totalbytes;
      return GCSWEEPCOST; /* GCSWEEPCOST 10 */
    }
    case GCSsweep: {
      lu_mem old = g->totalbytes;
      g->sweepgc = sweeplist(L, g->sweepgc, GCSWEEPMAX); /* GCSWEEPMAX 40 */
      if (*g->sweepgc == NULL) { /* nothing more to sweep? */
        checkSizes(L);
        g->gcstate = GCSfinalize; /* GCSfinalize 4 */ /* end sweep phase */
      }
      ((void)0); /* lua_assert(old >= g->totalbytes); */
      g->estimate -= old - g->totalbytes;
      /*      40*10;                */
      return GCSWEEPMAX * GCSWEEPCOST;
    }
    case GCSfinalize: {
      if (g->tmudata) {
        GCTM(L);
        if (g->estimate > GCFINALIZECOST) { /* GCFINALIZECOST 100 */
          g->estimate -= GCFINALIZECOST; /* GCFINALIZECOST 100 */
        }
        return GCFINALIZECOST;
      }
      else {
        g->gcstate = GCSpause; /* GCSpause 0 */ /* end collection */
        g->gcdept = 0;
        return 0;
      }
    }
    default: {
      ((void)0); /* lua_assert(0); */
      return 0;
    }
  }
}


void luaC_step (lua_State *L) {
  /*                G(L) */
  global_State *g = (L->l_G);
  /*           GCSTEPSIZE */
  l_mem lim = (1024u/100) * g->gcstepmul;
  if (lim == 0) {
    /*                   (MAX_LUMEM - 1) */
    lim = (((lu_mem)(~(lu_mem)0)-2) - 1)/2; /* no limit */
  }
  g->gcdept += g->totalbytes - g->GCthreshold;
  do {
    lim -= singlestep(L);
    if ( g->gcstate == GCSpause ) { /* GCSpause 0 */
      break;
    }
  } while (lim > 0);
  if (g->gcstate != GCSpause) { /* GCSpause 0 */
    if (g->gcdept < GCSTEPSIZE ) { /* GCSTEPSIZE 1024u */
      g->GCthreshold = g->totalbytes + GCSTEPSIZE; /* - lim/g->gcstepmul;*/
    } else {
      g->gcdept -= GCSTEPSIZE;
      g->GCthreshold = g->totalbytes;
    }
  }
  else {
    /* setthreshold(g); */
    g->GCthreshold = (g->estimate/100) * g->gcpause;
  }
}


void luaC_fullgc (lua_State *L) {
  /*                G(L); */
  global_State *g = (L->l_G);
  if (g->gcstate <= GCSpropagate) { /* GCSpropagate  1*/
    /* reset sweep marks to sweep all elements (returning them to white) */
    g->sweepstrgc = 0;
    g->sweepgc = &g->rootgc;
    /* reset other collector lists */
    g->gray = NULL;
    g->grayagain = NULL;
    g->weak = NULL;
    g->gcstate = GCSsweepstring; /* GCSsweepstring 2 */
  }
  ((void)0);
  /* finish any pending sweep phase */
  while (g->gcstate != GCSfinalize ) { /* GCSfinalize 4 */
    ((void)0); /* lua_assert(g->gcstate == GCSsweepstring || g->gcstate == GCSsweep); */
    singlestep(L);
  }
  markroot(L);
  while (g->gcstate != GCSpause) { /* GCSpause 0 */
    singlestep(L);
  }

  /* setthreshold(g); */
  g->GCthreshold = (g->estimate/100) * g->gcpause;
}


void luaC_barrierf (lua_State *L, GCObject *o, GCObject *v) {
  /*                G(L); */
  global_State *g = (L->l_G);
  ((void)0); /* lua_assert(isblack(o) && iswhite(v) && !isdead(g, v) && !isdead(g, o)); */
  ((void)0); /* lua_assert(g->gcstate != GCSfinalize && g->gcstate != GCSpause); */
  ((void)0); /* lua_assert(ttype(&o->gch) != LUA_TTABLE); */
  /* must keep invariant? */
  if (g->gcstate == GCSpropagate ) { /* GCSpropagate 1 */
    reallymarkobject(g, v); /* restore invariant */
  } else { /* don't mind */
    /* makewhite(g, o); */
    o->gch.marked = (lu_byte)( (o->gch.marked 
			& (lu_byte)( ~( (1<<2) | ( (1<<0) | (1<<1) ) ) )
    ) | (lu_byte)(g->currentwhite & ( (1<<0) | (1<<1) ) ) ); 
	/* mark as white just to avoid other barriers */
  }
}


void luaC_barrierback (lua_State *L, Table *t) {
  /*                G(L); */
  global_State *g = (L->l_G);
  /*             obj2gco(t); */
  GCObject *o = (GCObject *)(t);
  ((void)0); /* lua_assert(isblack(o) && !isdead(g, o)); */
  ((void)0); /* lua_assert(g->gcstate != GCSfinalize && g->gcstate != GCSpause); */

  /* black2gray(o); */
  o->gch.marked &= (lu_byte)(~((1<<2))); /* make table gray (again) */

  t->gclist = g->grayagain;
  g->grayagain = o;
}




/*
****************************************************************************************************

     <`Prepend`> a node into a link-table

              ( node1->gch.next -> NULL )
     g->roogc : node1 -> NULL 
     
              ( node2->gch.next -> node1 )
     g->roogc : node2 -> node1 -> NULL

              ( node3->gch.next -> node2 )
                       ( node2->gch.next -> node1 )
                                ( node1->gch.next -> NULL )
     g->roogc : node3 -> node2 -> node1 -> NULL

         and so forth ...

****************************************************************************************************
*/
void luaC_link (lua_State *L, GCObject *o, lu_byte tt) {
  /*                G(L); */
  global_State *g = (L->l_G);

  // Core Core Core : key code to make a chain list of a garbage collection objects
  o->gch.next = g->rootgc;
  g->rootgc = o;

  /*               luaC_white(g); */
  //     marked = (lu_byte)( 0x21 &    (0x3) ) = 0x1; // g->currentwhite  also see lstate.c:249
  o->gch.marked = (lu_byte)(g->currentwhite & ( (1<<0) | (1<<1) ));
  o->gch.tt = tt; // set value's datum type
}


void luaC_linkupval (lua_State *L, UpVal *uv) {
  /*                G(L); */
  global_State *g = (L->l_G);
  /*               obj2gco(uv); */
  GCObject *o = (GCObject *)uv;
  o->gch.next = g->rootgc; /* link upvalue into `rootgc' list */
  g->rootgc = o;
  /*   isgray(o) */
  if (  !( o->gch.marked & (1<<2) ) && !( o->gch.marked & ( (1<<0) | (1<<1) ) )  ) {
    if (g->gcstate == GCSpropagate ) { /*  GCSpropagate 1 */
      /* gray2black(o); */
      o->gch.marked |= (1<<2); /* closed upvalues need barrier */
      /* luaC_barrier(L, uv, uv->v); */
      { 
		  if ( ( uv->v->tt >= LUA_TSTRING && ( uv->v->value.gc->gch.marked & ( (1<<0) | (1<<1) ) ) ) 
				  && ( ( (GCObject *)uv)->gch.marked & (1<<2)  ) ) { 
			  luaC_barrierf(L, (GCObject *)uv, uv->v->value.gc); 
		  }
	  };
    }
    else { /* sweep phase: sweep it (turning it into white) */
      /* makewhite(g, o); */
      o->gch.marked = (lu_byte)( 
			  (o->gch.marked & (lu_byte)(~( (1<<2) | ( (1<<0) | (1<<1) ) ) ) )  
			  | (lu_byte)(g->currentwhite & ( (1<<0) | (1<<1) ) ) 
	  );
      ((void)0); /* lua_assert(g->gcstate != GCSfinalize && g->gcstate != GCSpause); */
    }
  }
}


