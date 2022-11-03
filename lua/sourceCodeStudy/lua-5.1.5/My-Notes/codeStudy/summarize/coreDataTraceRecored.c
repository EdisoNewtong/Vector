

struct lua_State {
  GCObject *next; // NULL : lstate.c:245
  lu_byte tt;     // LUA_TTHREAD  
  lu_byte marked; // 0x61 : lstate.c:251
  // CommonHeader;

  lu_byte status; // 0
  StkId top;  /* first free slot in the stack */ // the 0th address of element inside a 45(2*20+5) elements TValue array
              // top->tt = LUA_TNIL ( 0th element's type = LUA_TNIL ) , top -> 1st of 45  @ lstate.c:69
              
  StkId base;  /* base of current function */ // 1st of 45
  global_State *l_G; // g;
  CallInfo *ci;  /* call info for current function */ // the address of 0th element in a 8 elements CallInfo array 
                 // ci->func = 0th address inside 45
                 // ci->base = 1st of 45
                 // ci->top  = 21th of 45
                 
  const Instruction *savedpc;  /* `savedpc' of current function */ // NULL
  StkId stack_last;  /* last free slot in the stack */ // the 39th address of the element inside a 45-elements TValue array
  StkId stack;  /* stack base */ // ->  a 45 (2*20+5) elements TValue array
  CallInfo *end_ci;  /* points after end of ci array*/ // the 7th address of the element inside a  8-elements array
  CallInfo *base_ci;  /* array of CallInfo's */ // the address of 0th element in a 8 elements CallInfo array
  int stacksize;  // 45
  int size_ci;  /* size of array `base_ci' */ // a 8 elements CallInfo array
  unsigned short nCcalls;  /* number of nested C calls */ // 0 : lstate.c:142
  unsigned short baseCcalls;  /* nested C calls when resuming coroutine */ // 0 : // 0 : lstate.c:142
  lu_byte hookmask; // 0
  lu_byte allowhook; // 1
  int basehookcount; // 0
  int hookcount;  // 0   : hookcount = basehookcount; // lstate.c:139
  lua_Hook hook; // NULL
  TValue l_gt;  /* table of globals */   // l_gt.tt = LUA_TNIL , 
  TValue env;  /* temporary place for environments */
  GCObject *openupval;  /* list of open upvalues in this stack */ // NULL
  GCObject *gclist;
  struct lua_longjmp *errorJmp;  /* current error recover point */ // NULL
  ptrdiff_t errfunc;  /* current error handling function (stack index) */ // 0
}
