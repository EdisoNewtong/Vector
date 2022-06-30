

int main (int argc, char **argv) {
  int status;
  struct Smain s;
  // 为 struct lua_State 类型动态分配内存，并初始化结构体中的成员内容
  // extern lua_State *luaL_newstate (void) { // lauxlib.c:770
  /*             lua_open(); */
  lua_State *L = luaL_newstate(); /* create state */ 
  if (L == NULL) {
    l_message(argv[0], "cannot create state: not enough memory");
    return 1;
  }
  s.argc = argc;
  s.argv = argv;
  //       lua_cpcall( ... )   =>    lapi.c:1134 
  status = lua_cpcall(L, &pmain, &s);
  report(L, status);
  lua_close(L);
  return (status || s.status) ? 1 : 0;
}



