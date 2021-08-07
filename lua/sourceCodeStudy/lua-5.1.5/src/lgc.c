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
  ((void)0);
  if (((((&(n)->i_key.nk))->tt) >= 4)) {
    ((((&(n)->i_key.nk))->tt) = ((8 +3))); /* dead key; remove it */
  }
}


static void reallymarkobject (global_State *g, GCObject *o) {
  ((void)0);
  (((o)->gch.marked) &= ((lu_byte)(~((((1<<(0)) | (1<<(1))))))));
  switch (o->gch.tt) {
    case 4: {
      return;
    }
    case 7: {
      Table *mt = (&(&((o)->u))->uv)->metatable;
      (((o)->gch.marked) |= ((1<<(2)))); /* udata are never gray */
      if (mt) {
        { if (((((((GCObject *)((mt)))))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g, (((GCObject *)((mt))))); };
      }
      { if (((((((GCObject *)(((&(&((o)->u))->uv)->env)))))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g, (((GCObject *)(((&(&((o)->u))->uv)->env))))); };
      return;
    }
    case (8 +2): {
      UpVal *uv = (&((o)->uv));
      { ((void)0); if ((((uv->v)->tt) >= 4) && (((((uv->v)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g,((uv->v)->value.gc)); };
      if (uv->v == &uv->u.value) { /* closed? */
        (((o)->gch.marked) |= ((1<<(2)))); /* open upvalues are never black */
      }
      return;
    }
    case 6: {
      (&((o)->cl))->c.gclist = g->gray;
      g->gray = o;
      break;
    }
    case 5: {
      (&((o)->h))->gclist = g->gray;
      g->gray = o;
      break;
    }
    case 8: {
      (&((o)->th))->gclist = g->gray;
      g->gray = o;
      break;
    }
    case (8 +1): {
      (&((o)->p))->gclist = g->gray;
      g->gray = o;
      break;
    }
    default: {
      ((void)0);
    }
  }
}


static void marktmu (global_State *g) {
  GCObject *u = g->tmudata;
  if (u) {
    do {
      u = u->gch.next;
      ((u)->gch.marked = ((lu_byte)((((u)->gch.marked & ((lu_byte)((~((1<<(2))|((1<<(0)) | (1<<(1)))))))) | ((lu_byte)((g)->currentwhite & ((1<<(0)) | (1<<(1))))))))); /* may be marked, if left from previous GC */
      reallymarkobject(g, u);
    } while (u != g->tmudata);
  }
}


/* move `dead' udata that need finalization to list `tmudata' */
size_t luaC_separateudata (lua_State *L, int all) {
  global_State *g = (L->l_G);
  size_t deadmem = 0;
  GCObject **p = &g->mainthread->next;
  GCObject *curr;
  while ((curr = *p) != ((void *)0)) {
    if (!((((curr)->gch.marked) & ((((1<<(0)) | (1<<(1)))))) || all) || ((((&(&((curr)->u))->uv))->marked) & ((1<<(3))))) {
      p = &curr->gch.next; /* don't bother with them */
    } else if ((((&(&((curr)->u))->uv)->metatable) == ((void *)0) ? ((void *)0) : (((&(&((curr)->u))->uv)->metatable)->flags & (1u<<(TM_GC))) ? ((void *)0) : luaT_gettm((&(&((curr)->u))->uv)->metatable, TM_GC, ((L->l_G))->tmname[TM_GC])) == ((void *)0)) {
      ((((&(&((curr)->u))->uv))->marked) |= ((1<<(3)))); /* don't need finalization */
      p = &curr->gch.next;
    }
    else { /* must call its gc method */
      deadmem += (sizeof(union Udata)+((&(&((curr)->u))->uv))->len);
      ((((&(&((curr)->u))->uv))->marked) |= ((1<<(3))));
      *p = curr->gch.next;
      /* link `curr' at the end of `tmudata' list */
      if (g->tmudata == ((void *)0)) { /* list is empty? */
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
    { if (((((((GCObject *)((h->metatable)))))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g, (((GCObject *)((h->metatable))))); };
  }
  mode = ((h->metatable) == ((void *)0) ? ((void *)0) : ((h->metatable)->flags & (1u<<(TM_MODE))) ? ((void *)0) : luaT_gettm(h->metatable, TM_MODE, (g)->tmname[TM_MODE]));
  if (mode && (((mode)->tt) == 4)) { /* is there a weak mode? */
    weakkey = (strchr(((const char *)(((&(mode)->value.gc->ts)) + 1)), 'k') != ((void *)0));
    weakvalue = (strchr(((const char *)(((&(mode)->value.gc->ts)) + 1)), 'v') != ((void *)0));
    if (weakkey || weakvalue) { /* is really weak? */
      h->marked &= ~((1<<(3)) | (1<<(4))); /* clear bits */
      h->marked |= ((lu_byte)(((weakkey << 3) | (weakvalue << 4))));
      h->gclist = g->weak; /* must be cleared after GC, ... */
      g->weak = (((GCObject *)((h)))); /* ... so put in the appropriate list */
    }
  }
  if (weakkey && weakvalue) {
    return 1;
  }
  if (!weakvalue) {
    i = h->sizearray;
    while (i--) {
      { ((void)0); if ((((&h->array[i])->tt) >= 4) && (((((&h->array[i])->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g,((&h->array[i])->value.gc)); };
    }
  }
  i = ((1<<((h)->lsizenode)));
  while (i--) {
    Node *n = (&(h)->node[i]);
    ((void)0);
    if (((((&(n)->i_val))->tt) == 0)) {
      removeentry(n); /* remove empty entries */
    } else {
      ((void)0);
      if (!weakkey) {
        { ((void)0); if (((((&(n)->i_key.nk))->tt) >= 4) && ((((((&(n)->i_key.nk))->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g,(((&(n)->i_key.nk))->value.gc)); };
      }
      if (!weakvalue) {
        { ((void)0); if (((((&(n)->i_val))->tt) >= 4) && ((((((&(n)->i_val))->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g,(((&(n)->i_val))->value.gc)); };
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
    (((f->source)->tsv.marked) &= ((lu_byte)(~((((1<<(0)) | (1<<(1))))))));
  }
  for (i=0; i<f->sizek; i++) { /* mark literals */
    { ((void)0); if ((((&f->k[i])->tt) >= 4) && (((((&f->k[i])->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g,((&f->k[i])->value.gc)); };
  }
  for (i=0; i<f->sizeupvalues; i++) { /* mark upvalue names */
    if (f->upvalues[i]) {
      (((f->upvalues[i])->tsv.marked) &= ((lu_byte)(~((((1<<(0)) | (1<<(1))))))));
    }
  }
  for (i=0; i<f->sizep; i++) { /* mark nested protos */
    if (f->p[i]) {
      { if (((((((GCObject *)((f->p[i])))))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g, (((GCObject *)((f->p[i]))))); };
    }
  }
  for (i=0; i<f->sizelocvars; i++) { /* mark local-variable names */
    if (f->locvars[i].varname) {
      (((f->locvars[i].varname)->tsv.marked) &= ((lu_byte)(~((((1<<(0)) | (1<<(1))))))));
    }
  }
}


static void traverseclosure (global_State *g, Closure *cl) {
  { if (((((((GCObject *)((cl->c.env)))))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g, (((GCObject *)((cl->c.env))))); };
  if (cl->c.isC) {
    int i;
    for (i=0; i<cl->c.nupvalues; i++) { /* mark its upvalues */
      { ((void)0); if ((((&cl->c.upvalue[i])->tt) >= 4) && (((((&cl->c.upvalue[i])->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g,((&cl->c.upvalue[i])->value.gc)); };
    }
  }
  else {
    int i;
    ((void)0);
    { if (((((((GCObject *)((cl->l.p)))))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g, (((GCObject *)((cl->l.p))))); };
    for (i=0; i<cl->l.nupvalues; i++) { /* mark its upvalues */
      { if (((((((GCObject *)((cl->l.upvals[i])))))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g, (((GCObject *)((cl->l.upvals[i]))))); };
    }
  }
}


static void checkstacksizes (lua_State *L, StkId max) {
  int ci_used = ((int)((L->ci - L->base_ci))); /* number of `ci' in use */
  int s_used = ((int)((max - L->stack))); /* part of stack in use */
  if (L->size_ci > 20000) { /* handling overflow? */
    return; /* do not touch the stacks */
  }
  if (4*ci_used < L->size_ci && 2*8 < L->size_ci) {
    luaD_reallocCI(L, L->size_ci/2); /* still big enough... */
  }
  ((void)0);
  if (4*s_used < L->stacksize &&
      2*((2*20)+5) < L->stacksize) {
    luaD_reallocstack(L, L->stacksize/2); /* still big enough... */
  }
  ((void)0);
}


static void traversestack (global_State *g, lua_State *l) {
  StkId o, lim;
  CallInfo *ci;
  { ((void)0); if (((((&l->l_gt))->tt) >= 4) && ((((((&l->l_gt))->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g,(((&l->l_gt))->value.gc)); };
  lim = l->top;
  for (ci = l->base_ci; ci <= l->ci; ci++) {
    ((void)0);
    if (lim < ci->top) {
      lim = ci->top;
    }
  }
  for (o = l->stack; o < l->top; o++) {
    { ((void)0); if ((((o)->tt) >= 4) && (((((o)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g,((o)->value.gc)); };
  }
  for (; o <= lim; o++) {
    ((o)->tt=0);
  }
  checkstacksizes(l, lim);
}


/*
** traverse one gray object, turning it to black.
** Returns `quantity' traversed.
*/
static l_mem propagatemark (global_State *g) {
  GCObject *o = g->gray;
  ((void)0);
  (((o)->gch.marked) |= ((1<<(2))));
  switch (o->gch.tt) {
    case 5: {
      Table *h = (&((o)->h));
      g->gray = h->gclist;
      if (traversetable(g, h)) { /* table is weak? */
        (((o)->gch.marked) &= ((lu_byte)(~((1<<(2)))))); /* keep it gray */
      }
      return sizeof(Table) + sizeof(TValue) * h->sizearray +
                             sizeof(Node) * ((1<<((h)->lsizenode)));
    }
    case 6: {
      Closure *cl = (&((o)->cl));
      g->gray = cl->c.gclist;
      traverseclosure(g, cl);
      return (cl->c.isC) ? (((int)(sizeof(CClosure))) + ((int)(sizeof(TValue)*((cl->c.nupvalues)-1)))) :
                           (((int)(sizeof(LClosure))) + ((int)(sizeof(TValue *)*((cl->l.nupvalues)-1))));
    }
    case 8: {
      lua_State *th = (&((o)->th));
      g->gray = th->gclist;
      th->gclist = g->grayagain;
      g->grayagain = o;
      (((o)->gch.marked) &= ((lu_byte)(~((1<<(2))))));
      traversestack(g, th);
      return sizeof(lua_State) + sizeof(TValue) * th->stacksize +
                                 sizeof(CallInfo) * th->size_ci;
    }
    case (8 +1): {
      Proto *p = (&((o)->p));
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
      ((void)0);
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
  if (!(((o)->tt) >= 4)) {
    return 0;
  }
  if ((((o)->tt) == 4)) {
    ((((&(o)->value.gc->ts))->tsv.marked) &= ((lu_byte)(~((((1<<(0)) | (1<<(1)))))))); /* strings are `values', so are never weak */
    return 0;
  }
  return (((((o)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1)))))) ||
    ((((o)->tt) == 7) && (!iskey && ((((&(&(o)->value.gc->u)->uv))->marked) & ((1<<(3))))));
}


/*
** clear collected entries from weaktables
*/
static void cleartable (GCObject *l) {
  while (l) {
    Table *h = (&((l)->h));
    int i = h->sizearray;
    ((void)0);
    if (((h->marked) & ((1<<(4))))) {
      while (i--) {
        TValue *o = &h->array[i];
        if (iscleared(o, 0)) { /* value was collected? */
          ((o)->tt=0); /* remove value */
        }
      }
    }
    i = ((1<<((h)->lsizenode)));
    while (i--) {
      Node *n = (&(h)->node[i]);
      if (!((((&(n)->i_val))->tt) == 0) && /* non-empty entry? */
          (iscleared((&(n)->i_key.tvk), 1) || iscleared((&(n)->i_val), 0))) {
        (((&(n)->i_val))->tt=0); /* remove value ... */
        removeentry(n); /* remove entry from table */
      }
    }
    l = h->gclist;
  }
}


static void freeobj (lua_State *L, GCObject *o) {
  switch (o->gch.tt) {
    case (8 +1): {
      luaF_freeproto(L, (&((o)->p)));
      break;
    }
    case 6: {
      luaF_freeclosure(L, (&((o)->cl)));
      break;
    }
    case (8 +2): {
      luaF_freeupval(L, (&((o)->uv)));
      break;
    }
    case 5: {
      luaH_free(L, (&((o)->h)));
      break;
    }
    case 8: {
      ((void)0);
      luaE_freethread(L, (&((o)->th)));
      break;
    }
    case 4: {
      (L->l_G)->strt.nuse--;
      luaM_realloc_(L, (o), ((sizeof(union TString)+(((&(&((o)->ts))->tsv))->len+1)*sizeof(char))), 0);
      break;
    }
    case 7: {
      luaM_realloc_(L, (o), ((sizeof(union Udata)+((&(&((o)->u))->uv))->len)), 0);
      break;
    }
    default: {
      ((void)0);
    }
  }
}


static GCObject **sweeplist (lua_State *L, GCObject **p, lu_mem count) {
  GCObject *curr;
  global_State *g = (L->l_G);
  int deadmask = (g->currentwhite ^ ((1<<(0)) | (1<<(1))));
  while ((curr = *p) != ((void *)0) && count-- > 0) {
    if (curr->gch.tt == 8) { /* sweep open upvalues of each thread */
      sweeplist(L,&(&((curr)->th))->openupval,((lu_mem)(~(lu_mem)0)-2));
    }
    if ((curr->gch.marked ^ ((1<<(0)) | (1<<(1)))) & deadmask) { /* not dead? */
      ((void)0);
      ((curr)->gch.marked = ((lu_byte)((((curr)->gch.marked & ((lu_byte)((~((1<<(2))|((1<<(0)) | (1<<(1)))))))) | ((lu_byte)((g)->currentwhite & ((1<<(0)) | (1<<(1))))))))); /* make it white (for next cycle) */
      p = &curr->gch.next;
    }
    else { /* must erase `curr' */
      ((void)0);
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
  global_State *g = (L->l_G);
  /* check size of string hash */
  if (g->strt.nuse < ((lu_int32)(g->strt.size/4)) &&
      g->strt.size > 32*2) {
    luaS_resize(L, g->strt.size/2); /* table is too big */
  }
  /* check size of buffer */
  if (((&g->buff)->buffsize) > 32*2) { /* buffer too big? */
    size_t newsize = ((&g->buff)->buffsize) / 2;
    ((((&g->buff)->buffer)=((char *)(((((size_t)((newsize)+1)) <= ((size_t)(~(size_t)0)-2)/(sizeof(char))) ? luaM_realloc_(L, ((&g->buff)->buffer), ((&g->buff)->buffsize)*(sizeof(char)), (newsize)*(sizeof(char))) : luaM_toobig(L))))), (&g->buff)->buffsize = newsize);
  }
}


static void GCTM (lua_State *L) {
  global_State *g = (L->l_G);
  GCObject *o = g->tmudata->gch.next; /* get first element */
  Udata *udata = (&((o)->u));
  const TValue *tm;
  /* remove udata from `tmudata' */
  if (o == g->tmudata) { /* last element? */
    g->tmudata = ((void *)0);
  } else {
    g->tmudata->gch.next = udata->uv.next;
  }
  udata->uv.next = g->mainthread->next; /* return it to `root' list */
  g->mainthread->next = o;
  ((o)->gch.marked = ((lu_byte)((((o)->gch.marked & ((lu_byte)((~((1<<(2))|((1<<(0)) | (1<<(1)))))))) | ((lu_byte)((g)->currentwhite & ((1<<(0)) | (1<<(1)))))))));
  tm = ((udata->uv.metatable) == ((void *)0) ? ((void *)0) : ((udata->uv.metatable)->flags & (1u<<(TM_GC))) ? ((void *)0) : luaT_gettm(udata->uv.metatable, TM_GC, ((L->l_G))->tmname[TM_GC]));
  if (tm != ((void *)0)) {
    lu_byte oldah = L->allowhook;
    lu_mem oldt = g->GCthreshold;
    L->allowhook = 0; /* stop debug hooks during GC tag method */
    g->GCthreshold = 2*g->totalbytes; /* avoid GC steps */
    { const TValue *o2=(tm); TValue *o1=(L->top); o1->value = o2->value; o1->tt=o2->tt; ((void)0); };
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
  while ((L->l_G)->tmudata) {
    GCTM(L);
  }
}


void luaC_freeall (lua_State *L) {
  global_State *g = (L->l_G);
  int i;
  g->currentwhite = ((1<<(0)) | (1<<(1))) | (1<<(6)); /* mask to collect all elements */
  sweeplist(L,&g->rootgc,((lu_mem)(~(lu_mem)0)-2));
  for (i = 0; i < g->strt.size; i++) { /* free all string lists */
    sweeplist(L,&g->strt.hash[i],((lu_mem)(~(lu_mem)0)-2));
  }
}


static void markmt (global_State *g) {
  int i;
  for (i=0; i<(8 +1); i++) {
    if (g->mt[i]) {
      { if (((((((GCObject *)((g->mt[i])))))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g, (((GCObject *)((g->mt[i]))))); };
    }
  }
}


/* mark root set */
static void markroot (lua_State *L) {
  global_State *g = (L->l_G);
  g->gray = ((void *)0);
  g->grayagain = ((void *)0);
  g->weak = ((void *)0);
  { if (((((((GCObject *)((g->mainthread)))))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g, (((GCObject *)((g->mainthread))))); };
  /* make global table be traversed before main stack */
  { ((void)0); if (((((&g->mainthread->l_gt))->tt) >= 4) && ((((((&g->mainthread->l_gt))->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g,(((&g->mainthread->l_gt))->value.gc)); };
  { ((void)0); if (((((&(L->l_G)->l_registry))->tt) >= 4) && ((((((&(L->l_G)->l_registry))->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g,(((&(L->l_G)->l_registry))->value.gc)); };
  markmt(g);
  g->gcstate = 1;
}


static void remarkupvals (global_State *g) {
  UpVal *uv;
  for (uv = g->uvhead.u.l.next; uv != &g->uvhead; uv = uv->u.l.next) {
    ((void)0);
    if ((!((((((GCObject *)((uv)))))->gch.marked) & ((1<<(2)))) && !((((((GCObject *)((uv)))))->gch.marked) & ((((1<<(0)) | (1<<(1)))))))) {
      { ((void)0); if ((((uv->v)->tt) >= 4) && (((((uv->v)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g,((uv->v)->value.gc)); };
    }
  }
}


static void atomic (lua_State *L) {
  global_State *g = (L->l_G);
  size_t udsize; /* total size of userdata to be finalized */
  /* remark occasional upvalues of (maybe) dead threads */
  remarkupvals(g);
  /* traverse objects cautch by write barrier and by 'remarkupvals' */
  propagateall(g);
  /* remark weak tables */
  g->gray = g->weak;
  g->weak = ((void *)0);
  ((void)0);
  { if (((((((GCObject *)((L)))))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) reallymarkobject(g, (((GCObject *)((L))))); }; /* mark running thread */
  markmt(g); /* mark basic metatables (again) */
  propagateall(g);
  /* remark gray again */
  g->gray = g->grayagain;
  g->grayagain = ((void *)0);
  propagateall(g);
  udsize = luaC_separateudata(L, 0); /* separate userdata to be finalized */
  marktmu(g); /* mark `preserved' userdata */
  udsize += propagateall(g); /* remark, to propagate `preserveness' */
  cleartable(g->weak); /* remove collected objects from weak tables */
  /* flip current white */
  g->currentwhite = ((lu_byte)(((g->currentwhite ^ ((1<<(0)) | (1<<(1)))))));
  g->sweepstrgc = 0;
  g->sweepgc = &g->rootgc;
  g->gcstate = 2;
  g->estimate = g->totalbytes - udsize; /* first estimate */
}


static l_mem singlestep (lua_State *L) {
  global_State *g = (L->l_G);
  /*lua_checkmemory(L);*/
  switch (g->gcstate) {
    case 0: {
      markroot(L); /* start a new collection */
      return 0;
    }
    case 1: {
      if (g->gray) {
        return propagatemark(g);
      } else { /* no more `gray' objects */
        atomic(L); /* finish mark phase */
        return 0;
      }
    }
    case 2: {
      lu_mem old = g->totalbytes;
      sweeplist(L,&g->strt.hash[g->sweepstrgc++],((lu_mem)(~(lu_mem)0)-2));
      if (g->sweepstrgc >= g->strt.size) { /* nothing more to sweep? */
        g->gcstate = 3; /* end sweep-string phase */
      }
      ((void)0);
      g->estimate -= old - g->totalbytes;
      return 10;
    }
    case 3: {
      lu_mem old = g->totalbytes;
      g->sweepgc = sweeplist(L, g->sweepgc, 40);
      if (*g->sweepgc == ((void *)0)) { /* nothing more to sweep? */
        checkSizes(L);
        g->gcstate = 4; /* end sweep phase */
      }
      ((void)0);
      g->estimate -= old - g->totalbytes;
      return 40*10;
    }
    case 4: {
      if (g->tmudata) {
        GCTM(L);
        if (g->estimate > 100) {
          g->estimate -= 100;
        }
        return 100;
      }
      else {
        g->gcstate = 0; /* end collection */
        g->gcdept = 0;
        return 0;
      }
    }
    default: {
      ((void)0);
      return 0;
    }
  }
}


void luaC_step (lua_State *L) {
  global_State *g = (L->l_G);
  l_mem lim = (1024u/100) * g->gcstepmul;
  if (lim == 0) {
    lim = (((lu_mem)(~(lu_mem)0)-2)-1)/2; /* no limit */
  }
  g->gcdept += g->totalbytes - g->GCthreshold;
  do {
    lim -= singlestep(L);
    if (g->gcstate == 0) {
      break;
    }
  } while (lim > 0);
  if (g->gcstate != 0) {
    if (g->gcdept < 1024u) {
      g->GCthreshold = g->totalbytes + 1024u; /* - lim/g->gcstepmul;*/
    } else {
      g->gcdept -= 1024u;
      g->GCthreshold = g->totalbytes;
    }
  }
  else {
    (g->GCthreshold = (g->estimate/100) * g->gcpause);
  }
}


void luaC_fullgc (lua_State *L) {
  global_State *g = (L->l_G);
  if (g->gcstate <= 1) {
    /* reset sweep marks to sweep all elements (returning them to white) */
    g->sweepstrgc = 0;
    g->sweepgc = &g->rootgc;
    /* reset other collector lists */
    g->gray = ((void *)0);
    g->grayagain = ((void *)0);
    g->weak = ((void *)0);
    g->gcstate = 2;
  }
  ((void)0);
  /* finish any pending sweep phase */
  while (g->gcstate != 4) {
    ((void)0);
    singlestep(L);
  }
  markroot(L);
  while (g->gcstate != 0) {
    singlestep(L);
  }
  (g->GCthreshold = (g->estimate/100) * g->gcpause);
}


void luaC_barrierf (lua_State *L, GCObject *o, GCObject *v) {
  global_State *g = (L->l_G);
  ((void)0);
  ((void)0);
  ((void)0);
  /* must keep invariant? */
  if (g->gcstate == 1) {
    reallymarkobject(g, v); /* restore invariant */
  } else { /* don't mind */
    ((o)->gch.marked = ((lu_byte)((((o)->gch.marked & ((lu_byte)((~((1<<(2))|((1<<(0)) | (1<<(1)))))))) | ((lu_byte)((g)->currentwhite & ((1<<(0)) | (1<<(1))))))))); /* mark as white just to avoid other barriers */
  }
}


void luaC_barrierback (lua_State *L, Table *t) {
  global_State *g = (L->l_G);
  GCObject *o = (((GCObject *)((t))));
  ((void)0);
  ((void)0);
  (((o)->gch.marked) &= ((lu_byte)(~((1<<(2)))))); /* make table gray (again) */
  t->gclist = g->grayagain;
  g->grayagain = o;
}


void luaC_link (lua_State *L, GCObject *o, lu_byte tt) {
  global_State *g = (L->l_G);
  o->gch.next = g->rootgc;
  g->rootgc = o;
  o->gch.marked = ((lu_byte)((g)->currentwhite & ((1<<(0)) | (1<<(1)))));
  o->gch.tt = tt;
}


void luaC_linkupval (lua_State *L, UpVal *uv) {
  global_State *g = (L->l_G);
  GCObject *o = (((GCObject *)((uv))));
  o->gch.next = g->rootgc; /* link upvalue into `rootgc' list */
  g->rootgc = o;
  if ((!(((o)->gch.marked) & ((1<<(2)))) && !(((o)->gch.marked) & ((((1<<(0)) | (1<<(1)))))))) {
    if (g->gcstate == 1) {
      (((o)->gch.marked) |= ((1<<(2)))); /* closed upvalues need barrier */
      { if (((((uv->v)->tt) >= 4) && (((((uv->v)->value.gc))->gch.marked) & ((((1<<(0)) | (1<<(1))))))) && ((((((GCObject *)((uv)))))->gch.marked) & ((1<<(2))))) luaC_barrierf(L,(((GCObject *)((uv)))),((uv->v)->value.gc)); };
    }
    else { /* sweep phase: sweep it (turning it into white) */
      ((o)->gch.marked = ((lu_byte)((((o)->gch.marked & ((lu_byte)((~((1<<(2))|((1<<(0)) | (1<<(1)))))))) | ((lu_byte)((g)->currentwhite & ((1<<(0)) | (1<<(1)))))))));
      ((void)0);
    }
  }
}


