--------------------------------------------------
lua.c
--------------------------------------------------
int main (int argc, char **argv) {
    lua_State *L = luaL_newstate();
    ##################################################



--------------------------------------------------
lauxlib.c
--------------------------------------------------
extern lua_State *luaL_newstate (void) { ... } // Done

