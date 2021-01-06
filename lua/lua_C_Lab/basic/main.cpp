#include <iostream>
#include <string>
using namespace std;


extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

struct RGBA { int r,g,b,a; };


/*
int luaL_newmetatable (lua_State *L, const char *tname);
void luaL_getmetatable (lua_State *L, const char *tname);
int lua_setmetatable (lua_State *L, int index);
void lua_setfield (lua_State *L, int index, const char *k);
void lua_pushcfunction (lua_State *L, lua_CFunction f);
void lua_setglobal (lua_State *L, const char *name);
void* lua_newuserdata(lua_State *L, size_t size);

void lua_register (lua_State *L,
               const char *name,
               lua_CFunction f);
*/

int createRGBAObject(lua_State* L)
{
    RGBA* obj = (RGBA*)lua_newuserdata(L, sizeof(RGBA));
    // dummy set the init value for the field
    obj->r = 1; obj->g = 2; obj->b = 3; obj->a = 4;

    luaL_getmetatable(L,"rgbaMeta");
    lua_setmetatable(L,1);

    // the below code has already push the object RGBA into lua stack , so return 1
    return 1;
}

int getRGBAField(lua_State* L)
{
    // __index = function(table_Object, key)
    RGBA* obj = static_cast<RGBA*>(lua_touserdata(L,1));
    string keystr(lua_tostring(L,2));
    // cout << "keystr = " << keystr << endl;
    if ( keystr == "r" ) {
        lua_pushinteger(L, obj->r);
    } else if ( keystr == "g" ) {
        lua_pushinteger(L, obj->g);
    } else if ( keystr == "b" ) {
        lua_pushinteger(L, obj->b);
    } else if ( keystr == "a" ) {
        lua_pushinteger(L, obj->a);
    } else {
        cout << "Unknown key : <" << keystr << "> for object  RGBA  to <Get> field" << endl;
        lua_pushnil(L);
    }

    return 1;
}


int setRGBAField(lua_State* L)
{
    // __newindex = function(table_Object,key,value);
    RGBA* obj = static_cast<RGBA*>(lua_touserdata(L,1));
    string keystr(lua_tostring(L,2));
    int assignedValue = luaL_checkint(L, 3);
    if ( keystr == "r" ) {
        obj->r = assignedValue;
    } else if ( keystr == "g" ) {
        obj->g = assignedValue;
    } else if ( keystr == "b" ) {
        obj->b = assignedValue;
    } else if ( keystr == "a" ) {
        obj->a = assignedValue;
    } else {
        cout << "Unknown key : <" << keystr << "> for object  RGBA to <Set> field " << endl;
    }

    return 0;
}





void init_my_lib(lua_State* L)
{
    // To Register a function named  "lua_createRGBAObject"  that is available in your lua script
    lua_register (L, "lua_createRGBAObject", &createRGBAObject);

    // void lua_setfield (lua_State *L, int index, const char *k);
    // generate a metatable for   struct RGBA used
    // 1. new a table in registry
    luaL_newmetatable(L,"rgbaMeta");
    // 1. create a function to   access an userdata's field
    lua_pushcfunction(L, &getRGBAField);
    lua_setfield (L, -2, "__index");

    // 2. create a function to   set an userdata's certain field
    lua_pushcfunction(L, setRGBAField);
    lua_setfield (L, -2, "__newindex");

}

int main(int argc, char* argv[], char* env[])
{
    if ( argc != 2 ) {
        cout << "Invalid execute " << endl;
        cout << "Usage: " 
             << "       $ ./main   xxxx.lua"
             << endl;

        return -1;
    }

    // defined in lua.h
    //      #define lua_open()	luaL_newstate()
    lua_State* L = lua_open();
    if ( L == nullptr ) {
        cout << "Init lua environment Failed , program exit " << endl;
        return -1;
    }

    // 
    // Must do this function first , 
    // otherwise some lua function such as table.sort(...) , string.lower(...) , math.abs(...) is not avalible
    // defined int lualib.h
    luaL_openlibs(L);  // open libraries , defined in  lauxlib.h

    ////////////////////////////////////////////////////
    //
    // !!!IMPORTANT!!!
    //
    //      Register your self-defined struct/class/function before   executing lua script
    //  Otherwise your lua script will have no idea about   function lua_createRGBAObject(...) ,    xxxObject.r , xxxObject.a ?
    //
    ////////////////////////////////////////////////////
    init_my_lib(L);

    // defined in lauxlib.h
    // luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0)
    int ret = luaL_dofile(L, argv[1]);
    if ( ret != 0 ) {   // if ( luaL_loadfile(L, argv[1]) || lua_pcall(L, 0, 0, 0) ) {
        // == 1 , there must be some error occurs
        const char* errmsg = lua_tostring(L,-1);
        cout << "luaL_dofile dofile(...) , meet Error : " << endl
             <<  errmsg << endl;
    } else {
        cout << endl;
        cout << "==================================================" << endl;
        cout << "Execute file : " << argv[1] << " finished.  Successful !" << endl;
        cout << "==================================================" << endl;
    }

    
    lua_close(L);

    return 0;
}

