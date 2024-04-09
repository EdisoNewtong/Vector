--------------------------------------------------
lua.c
--------------------------------------------------
int main (int argc, char **argv) {
    lua_State *L = luaL_newstate();  // Done : implement in lauxlib.c:786
    ##################################################



--------------------------------------------------
lauxlib.c
--------------------------------------------------
// 分配一块内存空间，用来存储 lua_State 结构体，并初始化一些lua的核心数据
// 并设置 static int panic (lua_State *L) { ... } 
extern lua_State *luaL_newstate (void) { ... }

// C API   alloc/free memory function
static void *l_alloc (void *ud, void *ptr, size_t osize, size_t nsize) { ... }


--------------------------------------------------
lapi.c
--------------------------------------------------
// panic 在英文里的意思是 惊恐的 、 惊慌，人心惶惶
// 用传入的参数 回调函数 panicf  替换 原始的 G(L)->panic  , 并返回原始的 G(L)->panic 函数 ( 从源码中可以查询到，原始的 panic 函数在 lstate.c 中被设置成了 NULL )
extern lua_CFunction lua_atpanic (lua_State *L, lua_CFunction panicf) { ... } // lapi.c:160  Done
static int panic (lua_State *L) { ... } //  输出 栈顶下方的那个槽的 错误内容的字符串到  stderr
// wrap `func` && `ud` into a struct named CCallS , and invoke this function in lua way
extern int lua_cpcall (lua_State *L, lua_CFunction func, void *ud) { ... }
// push an new alloced Closure function object && the only wrapped argument  into lua state and invoke it
static void f_Ccall (lua_State *L, void *ud) { ... }
// return the corrected filed data with its type ( ttype(o) )
extern void *lua_touserdata (lua_State *L, int idx) { ... }
// get the TValue based upon L->base if (idx > 0)
//          TValue* result = L->base + (idx-1);
static TValue *index2adr (lua_State *L, int idx) { ... } // partially
// Do garbage collection , only for the switch-case LUA_GCSTOP:
extern int lua_gc (lua_State *L, int what, int data) { ... } // partially
// push a new-created table         with 'narray' array element(s)    
//                              &&       'nrec'   key-value pair(s)  into the stack 
void lua_createtable (lua_State *L, int narray, int nrec)



--------------------------------------------------
lstate.c
--------------------------------------------------
// Alloc a new lua_State and init its inner field and state
LUA_API  lua_State *lua_newstate (lua_Alloc f, void *ud) { ... } // Done

////////////////////////////////////////////////////
// base_ci      ->    [0] / 8 * CallInfo
// ci           ->    [0] / 8 * CallInfo
//   ci->func   ->    [0] / 45 * TValue
//   ci->base   ->    [1] / 45 * TValue
//   ci->top    ->    [21] / 45 * TValue // [1] + 20
// size_ci      = 8
// end_ci       ->   [7] / 8 * CallInfo
// base         ->    [1] / 45 * TValue
////////////////////////////////////////////////////
// stack        ->    [0] / 45 * TValue
// stacksize    ->    45
// top          ->    [0] / 45 * TValue ( top->tt = LUA_TNIL )
//  top          ->   [1] / 45 * TValue // latest
// stack_last   ->    [39]/ 45 * TValue
// 
////////////////////////////////////////////////////
static void stack_init (lua_State *L1, lua_State *L) { ... } // Done

// 初始化 lua_State 结构体 中的成员
static void preinit_state (lua_State *L, global_State *g) { ... } // Done
// 初始化 stack , gt(L) , registry(L) , string table , tag-method , lua keyword , ...
static void f_luaopen (lua_State *L, void *ud) { ... }


--------------------------------------------------
lgc.h
--------------------------------------------------
// 将从右往左的, 第b1位 和 第b2位 , 设置为 1 ( b1,b2 都是从 0 开始计数的 )
#define bit2mask(b1,b2)	(bitmask(b1) | bitmask(b2))




--------------------------------------------------
lmem.c
--------------------------------------------------
// Only Alloc memory , and update (+/-) the alloced-size by varible 'g->totalbytes'
void *luaM_realloc_ (lua_State *L, void *block, size_t osize, size_t nsize) { ... } // Done
// throw memory allocation error
void *luaM_toobig (lua_State *L) { ... }


--------------------------------------------------
ltable.c
--------------------------------------------------
// 分配一个新的Table , 这个Table 包含 2 部分
// Part-1: 包含 narray 个 array element
// Part-2: 包含 nhash  个 Key-Value 对
// 返回 new 出来的 Table
Table *luaH_new (lua_State *L, int narray, int nhash) {

// 分配 n 个 TValue 个内存块， 并将每个Element  的值设置成 nil
static void setarrayvector (lua_State *L, Table *t, int size) { ... }

/*


    分配 ceil( log2(n) ) 个 Key-Value 个内存块， 
    也就是说 传入 size = 5
    代码会分配  8 对 Key-Value ( 2^( ceil(log2(5)) ) = 8 )


    并将每个 Key-Value 的值设置成 
   
                 key_0    =nil , value_0    =nil     |   key_0->next     = NULL ( not key_1 ) <==   Core Core Core
                 key_1    =nil , value_1    =nil     |   key_1->next     = NULL ( not key_2 )
                 key_2    =nil , value_2    =nil     |   key_2->next     = NULL ( not key_3 )
                 key_3    =nil , value_3    =nil     |   key_3->next     = NULL ( not key_4 )
                  
                       ...
   
                 key_(n-1)=nil , value_(n-1)=nil     |   key_(n-1)->next = NULL ( not key_n )
                 key_n=nil     , value_n    =nil     |   key_n->next     = NULL 
   
*/
static void setnodevector  (lua_State *L, Table *t, int size)  { ... }

--------------------------------------------------
lstring.c
--------------------------------------------------
// resize the global stringtable of the global
void luaS_resize (lua_State *L, int newsize) { ... }
// Core Core Core :
// Hash the given string `str' and <prepend>  it into a mapped slot' chain    of hash-table in G(L)->strt
// The hash function is inside the function body
TString *luaS_newlstr (lua_State *L, const char *str, size_t l) { ... }

// push a new string into global's    hash-table in G(L)->strt
static TString *newlstr (lua_State *L, const char *str, size_t l, unsigned int h) { ... }



--------------------------------------------------
lgc.c
--------------------------------------------------
// prepend  object `o`   into   global-table's   g->rootgc    field
void luaC_link (lua_State *L, GCObject *o, lu_byte tt) { ... }


--------------------------------------------------
lobject.h
--------------------------------------------------
/*
** `module' operation for hashing (size is always a power of 2)
*/
#define lmod(s,size) \
	(check_exp((size&(size-1))==0, (cast(int, (s) & ((size)-1)))))

--------------------------------------------------
lobject.c
--------------------------------------------------
// get the floor interger value of   log2(x)
int luaO_log2 (unsigned int x) { ... } 


--------------------------------------------------
ldo.c
--------------------------------------------------
/* 
   Raw Run Code in a protected mode : 
   use if ( setjmp(...)  == 0 ) { 
        Pfunc(L,ud); 
        {
            if ( meetError ) {
                  //
                  // Also see the following code
                  //
                  if (L->errorJmp) {
                    L->errorJmp->status = errcode;
                    // _longjmp((L->errorJmp)->b, 1); 
                    LUAI_THROW(L, L->errorJmp);
                  }
            
                // lj.status = `a certain errorCode`
                longjmp(...);
            }
        }
    }
*/
int luaD_rawrunprotected (lua_State *L, Pfunc f, void *ud) { ... }



--------------------------------------------------
ldebug.c
--------------------------------------------------
// TODO : paritially read  ==>    wrap a C variadic function to generate a formated string 
void luaG_runerror (lua_State *L, const char *fmt, ...) { ... }

/*
** Call a function (C or Lua). The function to be called is at *func.
** The arguments are on the stack, right after the function.
** When returns, all the results are on the stack, starting at the original
** function position.
*/
void luaD_call (lua_State *L, StkId func, int nResults) { ... } // paritially

// Call the given function by the passed argument `func` ( either C or Lua ) 
// I read the only C function in else { ... }
int luaD_precall (lua_State *L, StkId func, int nresults) { ... } // partially


--------------------------------------------------
ltm.c
--------------------------------------------------
// init lua language string's  reserved word : __index , __newindex , __gc , __mode , ... , __call
// into varible  :  G(L)->tmname[0~16]  
void luaT_init (lua_State *L) { ... }



--------------------------------------------------
llex.c
--------------------------------------------------
// push all lua reserved key-words ( such as "local" "and" , "or" , "then" , "repeat" , "until" ... ) into  lua's global string table :  G(L)->strt
void luaX_init (lua_State *L) { ... }


--------------------------------------------------
lfunc.c
--------------------------------------------------
// Alloc a new <C> Closure struct  without assign the callback function , and return the block memory
Closure *luaF_newCclosure (lua_State *L, int nelems, Table *e) { ... }


--------------------------------------------------
lfunc.h
--------------------------------------------------
// because the struct has already defined a field named  : TValue upvalue[1];
// so  use   (n)-1 as the correct alloced memory size
#define sizeCclosure(n)	(cast(int, sizeof(CClosure)) + \
                         cast(int, sizeof(TValue)*((n)-1)))




--------------------------------------------------
linit.c   // All Done
--------------------------------------------------
// Open All Lua Library : such as lua build-in function    coroutine / debug / io / math / os / package / string / table
LUALIB_API void luaL_openlibs (lua_State *L) { ... }


