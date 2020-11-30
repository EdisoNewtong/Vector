--
-- function  makeVarStr(...) to see print_Table.lua

function printMultiArg(...)
	print("... = " .. makeVarStr(arg))
end


function my_print1(fmt, ... )
	print("=================== Begin my_print1 ===================")
	print("fmt = " .. makeVarStr(fmt))


	print("... = " .. makeVarStr(arg))
	print("=================== End my_print1 ===================\n")
end

function my_print2(fmt, ... )
	print("=================== Begin my_print2 ===================")
	print("fmt = " .. makeVarStr(fmt))

	printMultiArg(arg)
	print("=================== End my_print2 ===================\n")
end

function my_print3(fmt, ... )
	print("=================== Begin my_print3 ===================")
	print("fmt = " .. makeVarStr(fmt))

	printMultiArg(...)
	print("=================== End my_print3 ===================\n")
end



a =1
b = "Hello"
c = true
my_print1("a = ??? , b = ??? , c= ??? ",a,b,c)
my_print2("a = ??? , b = ??? , c= ??? ",a,b,c)
my_print3("a = ??? , b = ??? , c= ??? ",a,b,c)

--[[
=================== Begin my_print1 ===================
fmt = "a = ??? , b = ??? , c= ??? "
... = 
{
   [1] = 1,
   [2] = "Hello",
   [3] = true,
   "n" = 3,
}
=================== End my_print1 ===================

=================== Begin my_print2 ===================
fmt = "a = ??? , b = ??? , c= ??? "
... = 
{
   [1] = 
   {
      [1] = 1,
      [2] = "Hello",
      [3] = true,
      "n" = 3,
   },
   "n" = 1,
}
=================== End my_print2 ===================

=================== Begin my_print3 ===================
fmt = "a = ??? , b = ??? , c= ??? "
... = 
{
   [1] = 1,
   [2] = "Hello",
   [3] = true,
   "n" = 3,
}
=================== End my_print3 ===================


]]
