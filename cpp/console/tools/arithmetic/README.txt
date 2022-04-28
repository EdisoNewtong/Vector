==================================================
Usage : 
    <programName> --debugOption=<IntegerNumber>  --flag=<IntegerNumber>   <sourceCode>
==================================================



option : 
     | x  | x | x | x | x | x | x | x | x | x |
                                            1    : bit-1  =>  trace compile/runtime warnings 
                                        1        : bit-2  =>  print operatorStack List
                                    1            : bit-3  =>  print suffix List before build
                                1                : bit-4  =>  print suffix List after  build
                            1                    : bit-5  =>  print suffix List after  evaluate
                        1                        : bit-6  =>  trace operator Stack change
                    1                            : bit-7  =>  trace suffix Expression change
                1                                : bit-8  =>  trace change trace +/- property 
            1                                    : bit-9  =>  trace tmp expression evaluation step by step
       1                                         : bit-10 =>  trace parse time process step by step











flag  :   | x | x | x | x | x | x | x | x | x |
                                            1    : bit-1 => print all varible switcher (base on 10)
                                        1        : bit-2 => print all varible with 16 (hex)     format
                                     1           : bit-3 => print all varible with 2  (binary)  format
                                 1               : bit-4 => print all varible with 8  (octal)   format
