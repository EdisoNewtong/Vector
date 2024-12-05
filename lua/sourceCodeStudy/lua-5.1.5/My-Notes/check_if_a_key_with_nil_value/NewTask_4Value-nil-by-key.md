# Lua Table with a key-value pair which the value part is nil 

```lua
tb = { a=1, b=2 }


--[[

Q1:   How can i tell the 2 condition apart ?
1. No statement to set the 'c' field of tb  ,  the result of expression   'tb.c'  is nil because that the 'c' field is <NOT FOUND> in the given table 'tb' 
2. source code manually set the 'c' field to nil  :  tb.c = nil ,  so it result in the expression   'tb.c'  is nil because it is set by the source code , the 'c' field is existed in the given table 'tb' 

e.g.
local bExisted = table.isField(tb, 'c' )    -- return true / false





Q2:   How to travelsal the table with the existed key whose value is nil ?

-- Can i get the 'c' field by modify the original source code of Lua 5.1.5 ?

for k,v in pairs(tb) do
    print(k,v)                      
end

a 1
b 2

can I get the field 'c' ?

c nil   ?

--]]


tb.c = nil
print("tb.c = ", tb.c )    -- output :  tb.c = nil


```


