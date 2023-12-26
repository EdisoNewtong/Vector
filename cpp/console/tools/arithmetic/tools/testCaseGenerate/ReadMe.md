# testCaseGenerate/main.cpp  
In order to generate test case input files which contains 2 different tokens 

Such as :
1. 123 123
1. 123 +
1. =+
   ...


If 2 successive tokens are both operator ,
  it will generate 2 files :
file-1 : 
          int var = 5;
          var +=

file-2 :
          int var = 5;
          var + =                 // put a <Space> between 2 operator , 
/*
     Because , 
          "+ ="    
    and    "+="    will be treat as different operator
*/

# testCaseGenerate/check/main.cpp  
Check if the prefix generated input file is valid or not ( require binary executable program "arith" )  

