struct stringtable; // lstate.h:38
union GCObject;     // lstate.h:136
struct GCheader;    // lobject.h:49
union TString;      // lobject.h:199
union Udata;        // lobject.h:215
union Closure;      // lobject.h:295
struct Table;       // lobject.h:338
struct Proto;       // lobject.h:231
struct UpVal;       // lobject.h:274
struct lua_State;   // lstate.h:100
struct Mbuffer;     // lzio.h:24
struct lua_longjmp; // ldo.c:44

/*

lu_mem   => size_t
lu_int32 => unsigned int
lu_byte  => unsigned char

*/

struct CallInfo;                // lstate.h:48
typedef TValue *StkId;          /* index to stack elements */ // lobject.h:193
typedef lu_int32 Instruction;   // llimits.h:84
typedef unsigned char lu_byte;  // llimits.h:27
typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize); // lua.h:66
typedef int (*lua_CFunction) (lua_State *L); // lua.h:52
typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar); // lua.h:330


typedef struct stringtable {
  GCObject **hash;
  lu_int32 nuse;  /* number of elements :  lu_int32 => unsigned int */
  int size;
} stringtable;

/*
** Union of all collectable objects
*/
union GCObject {
  GCheader gch;
  union TString ts;
  union Udata u;
  union Closure cl;
  struct Table h;
  struct Proto p;
  struct UpVal uv;
  struct lua_State th;  /* thread */
};

/*
** informations about a call
*/
typedef struct CallInfo {
  StkId base;  /* base for this function */
  StkId func;  /* function index in the stack */
  StkId	top;  /* top for this function */
  const Instruction *savedpc;
  int nresults;  /* expected number of results from this function */
  int tailcalls;  /* number of tail calls lost under this entry */
} CallInfo;



/*
** Common header in struct form
*/
typedef struct GCheader {
  GCObject *next; 
  lu_byte tt; 
  lu_byte marked;
  /*
  ** Common Header for all collectable objects (in macro form, to be
  ** included in other objects)
  */
  // CommonHeader; // lobject.h:43     #define CommonHeader	GCObject *next; lu_byte tt; lu_byte marked
} GCheader;


/*
** String headers for string table
*/
typedef union TString {
  // #define LUAI_USER_ALIGNMENT_T	union { double u; void *s; long l; }  // luaconf.h:595
  // typedef LUAI_USER_ALIGNMENT_T L_Umaxalign;  // llimits.h:47
  L_Umaxalign dummy;  /* ensures maximum alignment for strings */
  struct {
    GCObject *next; 
    lu_byte tt; 
    lu_byte marked;
    // CommonHeader;

    lu_byte reserved;
    unsigned int hash;
    size_t len;
  } tsv;
} TString;


typedef union Udata {
  L_Umaxalign dummy;  /* ensures maximum alignment for `local' udata */
  struct {
    GCObject *next; 
    lu_byte tt; 
    lu_byte marked;
    // CommonHeader;

    struct Table *metatable;
    struct Table *env;
    size_t len;
  } uv;
} Udata;


typedef union Closure {
  CClosure c;
  LClosure l;
} Closure;



typedef struct Table {
  GCObject *next; 
  lu_byte tt; 
  lu_byte marked;
  // CommonHeader;

  lu_byte flags;  /* 1<<p means tagmethod(p) is not present */ 
  lu_byte lsizenode;  /* log2 of size of `node' array */
  struct Table *metatable;
  TValue *array;  /* array part */
  Node *node;
  Node *lastfree;  /* any free position is before this position */
  GCObject *gclist;
  int sizearray;  /* size of `array' array */
} Table;




/*
** Function Prototypes
*/
typedef struct Proto {
  GCObject *next; 
  lu_byte tt; 
  lu_byte marked;
  // CommonHeader;

  TValue *k;  /* constants used by the function */
  Instruction *code;
  struct Proto **p;  /* functions defined inside the function */
  int *lineinfo;  /* map from opcodes to source lines */
  struct LocVar *locvars;  /* information about local variables */
  TString **upvalues;  /* upvalue names */
  TString  *source;
  int sizeupvalues;
  int sizek;  /* size of `k' */
  int sizecode;
  int sizelineinfo;
  int sizep;  /* size of `p' */
  int sizelocvars;
  int linedefined;
  int lastlinedefined;
  GCObject *gclist;
  lu_byte nups;  /* number of upvalues */
  lu_byte numparams;
  lu_byte is_vararg;
  lu_byte maxstacksize;
} Proto;


/*
** Upvalues
*/

typedef struct UpVal {
  GCObject *next; 
  lu_byte tt; 
  lu_byte marked;
  // CommonHeader;

  TValue *v;  /* points to stack or to its own value */
  union {
    TValue value;  /* the value (when closed) */
    struct {  /* double linked list (when open) */
      struct UpVal *prev;
      struct UpVal *next;
    } l;
  } u;
} UpVal;


/*
** `per thread' state
*/
struct lua_State {
  GCObject *next; 
  lu_byte tt; 
  lu_byte marked;
  // CommonHeader;

  lu_byte status;
  StkId top;  /* first free slot in the stack */
  StkId base;  /* base of current function */
  global_State *l_G;
  CallInfo *ci;  /* call info for current function */
  const Instruction *savedpc;  /* `savedpc' of current function */
  StkId stack_last;  /* last free slot in the stack */
  StkId stack;  /* stack base */
  CallInfo *end_ci;  /* points after end of ci array*/
  CallInfo *base_ci;  /* array of CallInfo's */
  int stacksize;
  int size_ci;  /* size of array `base_ci' */
  unsigned short nCcalls;  /* number of nested C calls */
  unsigned short baseCcalls;  /* nested C calls when resuming coroutine */
  lu_byte hookmask;
  lu_byte allowhook;
  int basehookcount;
  int hookcount;
  lua_Hook hook;
  TValue l_gt;  /* table of globals */
  TValue env;  /* temporary place for environments */
  GCObject *openupval;  /* list of open upvalues in this stack */
  GCObject *gclist;
  struct lua_longjmp *errorJmp;  /* current error recover point */
  ptrdiff_t errfunc;  /* current error handling function (stack index) */
};


/*
** Tagged Values
*/

typedef struct lua_TValue {
  Value value; 
  int tt;
  // TValuefields; // lobject.h:71   #define TValuefields	Value value; int tt
} TValue;


/*
** `global state', shared by all threads of this state
*/
typedef struct global_State {
  stringtable strt;  /* hash table for strings */
  lua_Alloc frealloc;  /* function to reallocate memory */
  void *ud;         /* auxiliary data to `frealloc' */
  lu_byte currentwhite;
  lu_byte gcstate;  /* state of garbage collector */
  int sweepstrgc;  /* position of sweep in `strt' */
  GCObject *rootgc;  /* list of all collectable objects */
  GCObject **sweepgc;  /* position of sweep in `rootgc' */
  GCObject *gray;  /* list of gray objects */
  GCObject *grayagain;  /* list of objects to be traversed atomically */
  GCObject *weak;  /* list of weak tables (to be cleared) */
  GCObject *tmudata;  /* last element of list of userdata to be GC */
  Mbuffer buff;  /* temporary buffer for string concatentation */
  lu_mem GCthreshold;
  lu_mem totalbytes;  /* number of bytes currently allocated */
  lu_mem estimate;  /* an estimate of number of bytes actually in use */
  lu_mem gcdept;  /* how much GC is `behind schedule' */
  int gcpause;  /* size of pause between successive GCs */
  int gcstepmul;  /* GC `granularity' */
  lua_CFunction panic;  /* to be called in unprotected errors */
  TValue l_registry;
  struct lua_State *mainthread;
  UpVal uvhead;  /* head of double-linked list of all open upvalues */
  /*                                                                                  1        ~              4      |     5                 ~            9    */
  struct Table *mt[NUM_TAGS];  /* metatables for basic types      NUM_TAGS = 8+1 = 9 (nil,bool,lightuserdata,number,    string,table,function,userdata,thread )*/
  TString *tmname[TM_N];  /* array with tag-method names          TM_N = 17 (__index, __newindex, __gc, __mode, __eq, ... , __concat , __call ) */
} global_State;





typedef struct Mbuffer {
  char *buffer;
  size_t n;
  size_t buffsize;
} Mbuffer;


/* chain list of long jump buffers */
struct lua_longjmp {
  struct lua_longjmp *previous;
  jmp_buf b;      /*  luai_jmpbuf b; */
  volatile int status; /* error code */
};
