==================================================
Usage : 
    <programName> --option=<IntegerNumber>  --flag=<IntegerNumber>   <sourceCode>
==================================================



option :       | x | x | x | x | x | x | x | x |
                                             1   : print parser detail every step



flag  :       | x | x | x | x | x | x | x | x |
                                            1    : bit-1 =>  print operatorStack List All
                                         1       : bit-2 =>  print suffix List before 
                                     1           : bit-3 =>  print suffix List after 
                                1                : bit-4 =>  trace operatorStack change
                             1                   : bit-5 =>  trace suffixExpression change
                         1                       : bit-6 =>  trace +/- property change
