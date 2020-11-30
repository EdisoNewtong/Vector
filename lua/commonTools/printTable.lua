function makeNspace(n)
	local str = ""
	local space = "   "
	for i=1,n,1 do
		str = str .. space
	end
	return str
end

function makeTableStr(tb,depth,visited)
	if not visited then
		visited = { { key="<root>", value=tb } }
	else
		local v_len = #visited
		if v_len>0 then
			local finded = false
			local finded_str = nil
			local finded_key = nil
			for _,v in ipairs(visited) do
				if v.value==tb then
					finded = true
					finded_key = v.key
					finded_str = "<already visited table! it's key = " .. v.key .. "\t" .. tostring(v.value)
					break
				end
			end

			if finded then
				return { key=finded_key, value=finded_str }
			end
		end
	end

	local real_depth = depth and depth or 0
	local left = makeNspace(real_depth) .. "{\n"

	local content = ""
	for k,v in pairs(tb) do
		local tp_key = type(k)
		local tp_val = type(v)

		-- Make key string
		if tp_key=="number" then
			content = content .. makeNspace(real_depth+1) .. "[" .. tostring(k) .. "]"
		elseif tp_key=="string" then
			content = content .. makeNspace(real_depth+1) .. "\""  .. k .. "\""
		else
			content = content .. makeNspace(real_depth+1) .. "<" .. tp_key .. ">"
		end
		content = content .. " = "

		if tp_val~="table" then
			if tp_val~="string" then
				content = content .. tostring(v) .. ","
			else
				content = content .. "\"" .. tostring(v) .. "\"" .. ","
			end
		else
			visited[#visited+1] = { key=tostring(k), value=tb }
			local ret = makeTableStr(v,real_depth+1,visited)
			if type(ret)=="string" then
				content = content .. "\n" .. ret .. ","
			else
				content = content .. ret.value .. ","
			end
		end
		content = content .. "\n"
	end

	local right = makeNspace(real_depth) .. "}"
	local final_str =  left .. content .. right
	return final_str
end

function makeVarStr(var)
	local retStr = ""
	local tp = type(var)
	if tp~="table" then
		if tp~="string" then
			retStr = tostring(var)
		else
			retStr = "\"" .. tostring(var) .. "\""
		end
	else
		retStr = "\n" .. makeTableStr(var)
	end
	return retStr
end

print("_G = ", makeVarStr(_G) )
