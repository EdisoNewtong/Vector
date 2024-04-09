# Call Stack Trace 

int main(...)  // lua.c:508
  status = lua_cpcall(L, &pmain, &s);  // lua.c:519  --> lapi.c:1134
    status = luaD_pcall(L, f_Ccall, &c, savestack(L, L->top), 0);  // lapi.c:1142 --> ldo.c:663
      status = luaD_rawrunprotected(L, func, u); // ldo.c:672  --> ldo.c:123
        (*f)(L, ud);   // ldo.c:135 --> lapi.c:1116
          luaD_call(L, L->top - 2, 0); // lapi.c:1130   --> ldo.c:532
            if (luaD_precall(L, func, nResults) == PCRLUA)  // ldo.c:544 --> ldo.c:364
                n = (*((&(L->ci->func)->value.gc->cl))->c.f)(L); // ldo.c:461 --> lua.c:455  static int pmain (lua_State *L) {
                  luaL_openlibs(L); // lua.c:466  -->  linit.c:37
                    lua_call(L, 1, 0); // linit.c:42  --> lapi.c:1040
                        luaD_call(L, func, nresults);  // lapi.c:1046 -->  ldo.c:532
  

