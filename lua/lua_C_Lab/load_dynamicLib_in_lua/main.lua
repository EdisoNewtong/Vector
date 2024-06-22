
local util = require("outputDynamicLib.CDivide")
print("util = ",util)

local a = 15
local b =  6 

local ret1 = util.dylib_divide(a,b)
local ret2 = util.dylib_divide(b,a)

-- local ret1 = c_Divide(a,b)
-- local ret2 = c_Divide(b,a)

print("ret1 = " ..  a .. " / " .. b .. " = " .. (ret1 and ret1 or "nil"))
print("ret2 = " ..  b .. " / " .. a .. " = " .. (ret2 and ret2 or "nil"))




