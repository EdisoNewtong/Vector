
local function isNumber(v)
    return type(v) == "number"
end

local function isInterger(v)
    return        isNumber(v)
            and ( math.floor(v)==math.ceil(v) )
end

local Gen = {
    bool = function(v)
        if  v==true or v==false  then
            return v
        else
            error("in Gen.bool(...) , input arg is not a bool type.")
        end
    end,

    uchar = function(v)
        if isInterger(v) then
            error("in Gen.uchar(...) , input arg is not a uchar type.")
        end

        if not (v>=0 and v<=255) then
            error("in Gen.schar(...) , input arg is out of range.")
        end

        return v
    end,

    schar = function(v)
        if isInterger(v) then
            error("in Gen.schar(...) , input arg is not a schar type.")
        end

        if not (v>=-128 and v<=127) then
            error("in Gen.schar(...) , input arg is out of range.")
        end

        return v
    end,


	-- treat char as   signed char
    char = function(v)
        if isInterger(v) then
            error("in Gen.char(...) , input arg is not a char type.")
        end

        if not (v>=-128 and v<=127) then
            error("in Gen.char(...) , input arg is out of range.")
        end

        return v
    end,

	u_short = function(v)
        if isInterger(v) then
            error("in Gen.u_short(...) , input arg is not a char type.")
        end

        if not (v>=0 and v<=65535) then
            error("in Gen.u_short(...) , input arg is out of range.")
        end

        return v
	end,

	short = function(v)
        if isInterger(v) then
            error("in Gen.short(...) , input arg is not a char type.")
        end

        if not (v>=-32768 and v<=32767) then
            error("in Gen.short(...) , input arg is out of range.")
        end

        return v
	end,

	u_int = function(v)
        if isInterger(v) then
            error("in Gen.u_int(...) , input arg is not a char type.")
        end

        if not (v>=0 and v<=4294967295) then
            error("in Gen.u_int(...) , input arg is out of range.")
        end

        return v
	end,

	int = function(v)
        if isInterger(v) then
            error("in Gen.int(...) , input arg is not a char type.")
        end

        if not (v>=-2147483648 and v<=2147483647) then
            error("in Gen.int(...) , input arg is out of range.")
        end

        return v
	end,



}


