
local luaUtil = {
    dylib_divide = function(a,b)
        print("\t In Lua luaUtil.dylib_divide(a,b)")
        return a^b
    end,
}

return luaUtil

