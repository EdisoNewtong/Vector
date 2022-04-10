This program tool is used to convert from decimal float point number (such as float/double)  into binary number


--------------------------------------------------
ieee754.cpp
--------------------------------------------------
Usage :
    $ ./ieee754    <floatNumber>(f/F)



--------------------------------------------------
demo.cpp
--------------------------------------------------

pair<string,float>  pr = make_pair("1.1f", 1.1f);
Convert   pr.second (1.1f)  =>   binary format 




--------------------------------------------------
unitTest.cpp
--------------------------------------------------
Usage : 
	$ ./unitTest   <intBits>   <floatBits>   <[ne|other]>  <[f|other]>
e.g.
$ ./ieee754 2 2  a f
1. generate from  a string list from "00.00f" ~ "99.99f"
2. pick each element in the list , and do a convert from    string-float    =>    float
3. set the atof(element)  into  type union intFloatNum's  f_num  or  union llDoubleNum's f_num ( it depend on whether has the "f" suffix )
4. compare the  cvt.doConvert(...) result   with    union's i_num field   , equal or not



--------------------------------------------------
genTools.cpp
--------------------------------------------------
 *   $ ./genTools   
 *		  --datatype <float/double>  
 *		  --intBit     <number>        // a number >=1
 *		  --floatBit   <number>        // a number >=1
 *		  (--negativeSign)

=>   it will generate a file named "genCode.cpp"
Then use  make genCode the build a project named "genCode"






