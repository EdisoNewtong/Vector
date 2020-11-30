-- For Debug , getTrackStack
function edisonGetCallStack()
	-- Start with 2
	-- 0 debug.getinfo() itself
	-- 1 edisonGetCallStack() , the function invoked debug.getinfo()
	-- 2 some function who invoke edisonGetCallStack
	local level = 2
	local info = nil
	local infoList = {}
	while true do
		info = debug.getinfo(level)
		infoList[#infoList + 1] = info
		if info then
			level = level + 1
		else
			break
		end
	end

	--"func" = <Lua Function :   0x06022CC0>,
	--"name" = "__create"
	--"currentline" = 394,
	--"linedefined" = 348,
	--"lastlinedefined" = 423,
	--"source" = "src/framework/cocos2dx/NodeEx.lua",
	local idx = 1
	print("========== Call Stack Begin ==========")
	for _,ele in ipairs(infoList) do
		if ele then
			if type(ele.source)=="string" and string.find(ele.source,"%.lua") then
				-- is Call by xxx.lua
				if type(ele.name)=="string" and #ele.name > 0 then
					--linedefined ~ lastlinedefined : currentline
					--edisonPrint(string.format("In luaFile : [ %s ] , Calling func = <%s> @line %d, defined range is line [ %d ~ %d ] ",ele.source, ele.name, ele.currentline, ele.linedefined, ele.lastlinedefined))
					print(string.format("%d. In luaFile : [ %s ]:%d , call func = '%s'",idx, ele.source, ele.currentline, ele.name))
				else
					-- Anonymous Lua Call
					--print(string.format("%d. In luaFile : [ %s ] , Calling func = <???> @line %d, defined range is line [ %d ~ %d ] ",idx , ele.source,  ele.currentline, ele.linedefined, ele.lastlinedefined))
					print(string.format("%d. In luaFile : [ %s ]:%d , Calling func = '???' ",idx , ele.source,  ele.currentline))
				end
			else
				print(string.format("%d. In <Client C/C++ Call> , No Info",idx))
			end

			idx = idx + 1
		end
	end
	print("========== Call Stack End ==========")
end


