
--
-- readCommandLineArgs.lua
--

function cmdlineArgs(...)
    --[[
          arg is an lua reserved keyword for variadic function use
    ]]
    print("--------------------------------------------------")
    print("arg.n = " .. (arg.n and arg.n or "nil") )
    print("--------------------------------------------------")
    print()
    
    for i,v in ipairs(arg) do
        print(i,v)
    end

end


-- execute function with the arguments from command line
cmdlineArgs(...)

--[[


                                 arg.n = 3
                                 arg[1]    arg[2]     arg[3]
    $ lua readCommandLineArgs.lua   aaa        bbb        ccc


--]]

-- arg[1]
-- arg[2]
-- arg[3]
--    ......
-- arg[n]


