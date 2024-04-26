--[[

In the repeat-until loop, the inner block does not end at the until keyword, but only after the condition. 
So, the condition can refer to local variables declared inside the loop block.

--]]



sum = 0
repeat
    --
    -- the local varible i can be defined inside   loop-body
    --
    i = ((not i) and 0 or i) + 1
    sum = sum + i
until (i>=100)      -- end loop only if condition (i>=100) yield false ( =>  !(i<100) )

print("sum = " .. sum)
print("i = " .. (i and i or "nil"))


--[[

sum = 5050
i = 100

--]]




