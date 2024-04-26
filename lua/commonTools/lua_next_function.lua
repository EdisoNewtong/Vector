--[[

next (table [, index])

 Allows a program to traverse all fields of a table. Its first argument is a table and its second argument is an index in this table. next returns the next index of the table and its associated value. 
 When called with nil as its second argument, next returns an initial index and its associated value. 
 When called with the last index, or with nil in an empty table, next returns nil. 
 If the second argument is absent, then it is interpreted as nil. 
 In particular, you can use next(t) to check whether a table is empty.
 
 The order in which the indices are enumerated is not specified, even for numeric indices. (To traverse a table in numeric order, use a numerical for or the ipairs function.)
 
 The behavior of next is undefined if, during the traversal, you assign any value to a non-existent field in the table. 
 You may however modify existing fields. In particular, you may clear existing fields.


--]]




tb1 = { a=1, b=2, c=3 }
tb2 = {}

function travelsalTable(tb)
    print(tb)
    nextKey = nil
    currentValue = nil

    --[[   
           ---> Here <---

              You can use the ==return value== of next(table,nil)
           to check if a table is empty or not

    --]]
    nextKey,currentValue = next(tb,nextKey)
    if nextKey==nil then
        print("\t It's an empty Table")
        return
    end



    while true do
        print("\t", nextKey,currentValue)
        nextKey,currentValue = next(tb,nextKey)
        if nextKey==nil then
            break
        end
    end
end

print("Travelsal Result : ")
print()

travelsalTable(tb1)

print()
travelsalTable(tb2)


--[[

Travelsal Result : 

table: 00C92A48
                a       1
                c       3
                b       2

table: 00C92868
                It's an empty Table

--]]



















--[[
# next(...) in detail 



Lua 库中实现的 pairs 是一个用 next 实现的原始方法:   ( Also see《Lua中文教程》  **==54/243==** )

```lua
function pairs (t)
    return next, t, nil
end
```

还可以不使用 ipairs 直接使用 next 

```lua

for k, v in next, t do
      ...
end

```

记住：exp-list 返回结果会被调整为三个，所以 Lua 获取 ==next、t、nil==；确切地说当
他调用 pairs 时获取.

--]]

