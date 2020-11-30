local tb_test = {
	a = 1,
	b = "sss",
	c = {
		ddd = 3.14,
		fff = { x = 1, y = 2, }
	},
};

--  make a passed  table   as    const
function c_table(tb)
	local const_tb = {};
	setmetatable(const_tb, {
		__index = tb,
		__newindex = function(tb,key,v)
			print("!!! const table !!! , can't set key [", key, "]");
		end
	});
	return const_tb;
end


--  clone a table and make all of its content as const
--   recursively contains it's sub-table 
function copyTable(tb)
	local cp = {}
	for k,v in pairs(tb) do
		if type(v) == "table" then
			cp[k] = c_table( copyTable(v) );
		else
			cp[k] = v;
		end
	end

	local f_tb = c_table(cp);
	return f_tb;
end

local cpt1 = copyTable(tb_test);
print("========== Begin Visit ==========");
for k,v in pairs(cpt1) do
	print(k,v);
end

print("t.a = ", cpt1.a);
print("t.b = ", cpt1.b);
print("t.c = ", cpt1.c);
print("t.c.ddd = ", cpt1.c.ddd);
print(" Now Set it !!!!");
cpt1.a = 123;	// !!! const table !!! , can't set key [ aaa ]
cpt1.b = "Hello World";  // !!! const table !!! , can't set key [ bbb ]
cpt1.c = 998;		// !!! const table !!! , can't set key [ c ]
cpt1.c.ddd = 345;	// !!! const table !!! , can't set key [ ddd ]
print(" After set ");
print("t.a = ", cpt1.a);
print("t.b = ", cpt1.b);
print("t.c = ", cpt1.c);
print("t.c.ddd = ", cpt1.c.ddd);

print("========== End Visit ==========");


