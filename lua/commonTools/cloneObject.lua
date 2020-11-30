function cloneObject(old)
	-- 	 nil, boolean, number, string,			<==   Value  Data
-- function, userdata, thread, and table   	 <==   Reference Data
	local cloned = nil
	if type(old)~="table" then
		 cloned = old
	else 
		-- == table
		cloned = {}
		for k,v in pairs(old) do
                       -- Recursively call
			cloned[k] = cloneObject(v)
		end
	end

	return cloned
end


