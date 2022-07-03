--
-- genArrayCase.lua
--

-- 
-- Decimal base number --> Binary bit array 
--
--              for a given arysize ( fill up with 0 if the bit is less than the given arysize )
--
function dec2bin(n, arysize)
    local tb = { }

    local q = 0
    local rest = 0
    local tmptb = { }
    local idx = 1
    while true do
        q = math.floor(n / 2)
        rest = n % 2

        if q == 0 then
            tmptb[idx] = rest
            break
        else
            tmptb[idx] = rest
            idx = idx + 1

            n = q
        end
    end

    idx = 1
    local delta = (arysize - #tmptb)
    for i=1,delta do
        tb[idx] = 0
        idx = idx + 1
    end

    for i=#tmptb,1,-1 do
        tb[idx] = tmptb[i]
        idx = idx + 1
    end

    return tb
end


--[[
function tb2str(tb)
    local binstr = ""
    for i=1,#tb do
        binstr = binstr .. tb[i]
    end
    return binstr
end
--]]




function printBody(pownum)
    local pow = pownum

    maxnum = (2^pow-1)
    maxlen = #(tostring(maxnum))

    print(string.format("    local pow    = %s", tostring(pow) ))
    print(string.format("    local maxnum = %s", "(2^pow)" ))
    print("    print( string.format(\"in case%d() :  %d element%s , total %d possibilities\\n\", pow, pow, (maxnum==1 and \"\" or \"s\"),  maxnum) )")
    print("    local tb = {}")
    print("    local strtb = \"\"")
    print("    local commentline = \"\"")

    tbHead            = "          tb = { "
    tableFullBodyPart = "               { "

    for i=1,pow do
        tableFullBodyPart = tableFullBodyPart .. string.format("%5d%s",(i), ((i==pow) and "" or ",") )
    end
    commentFullPart = "    --" .. tableFullBodyPart:sub(3+4) .. " }"
    
    for n=0,maxnum do
        local bitary = dec2bin(n, pow)
        tbFull  = tbHead

        for j=1,#bitary do
            tmpEleStr = (bitary[j] == 0 and "nil" or tostring(j) )
            element = string.format("%5s",  tmpEleStr)
            tbFull = tbFull  .. (element .. (j == #bitary and "" or ",") )
        end

        tbFull = tbFull .. " }"
        print(commentFullPart)
        print(tbFull)

        print( string.format("    commentline = \"--    %s\"",  commentFullPart:sub(7)) )
        print("    print(commentline)")

        print( string.format("    strtb = \"%s\"", tbFull) )
        print("    print( string.format(\"%s\", \"--\" .. strtb:sub(3) ) )")
        print("    print(\"         #tb\ = \" .. #tb)")
        print("    print()")
        print()
    end
end








----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

function mainfunc(...)
    if arg.n == 0 then
        return
    end

    local maxPower = tonumber(arg[1])
    if not maxPower then
        print( string.format("Sorry \"%s\" is not a number", tostring(arg[1]) ))
        return
    end

    for i=1,maxPower do
        print("function case" .. i .. "()")
        printBody(i)
        print("end")
        print()
    end

    print()
    print()
    print()

    for i=1,maxPower do
        print("print(\"==================================================\")")
        print("case" .. i .. "()")
        print("print(\"==================================================\")")
        print()
        print()
    end
end

mainfunc(...)



--[[


    $ lua  genArrayCase.lua    8    >   array8.lua   # generate a lua script file named  "array8.lua"
    $ lua  array8.lua                                # execute the generated script mentioned before


--]]


