#!/bin/bash

# c_src/lua_need_to_require.c      =>    CDivide.so

internal_marco := -DLUA_USE_LINUX

internal_inc_path := inc

# to link the static library named   "liblua.a"  , Please use the option   : -llua
internal_lib_path := preBuildStaticLibrary/linux_gcc





outputDynamicLib/CDivide.so:c_src/lua_need_to_require.c
	gcc  -Wall  $(internal_marco) -I$(internal_inc_path) -c              -o  outputDynamicLib/CDivide.o    c_src/lua_need_to_require.c  -L$(internal_lib_path) -llua
	gcc  -Wall  $(internal_marco) -I$(internal_inc_path) -shared -fPIC   -o  outputDynamicLib/CDivide.so   outputDynamicLib/CDivide.o


.PHONY:clean
clean:
	-rm   -Rf    outputDynamicLib/*.o  outputDynamicLib/*.so  outputDynamicLib/*.dll   


