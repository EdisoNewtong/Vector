# How to encrypt a given file 

```C++

    const int baseLength = 16;
    const int dummyLength[3] = { 
                             baseLength << 0,  
                             baseLength << 1,  
                             baseLength << 2  
                         };

    int exponent = random(0, 0x7FFFFFFF) % 3; // got one of [0,1,2]

    /***************************************************************************************************************************************************

-----------------
    Encryption
-----------------

       16 (0x10) =  3 + 10 + (...) +  3; 
       32 (0x20) = 11 + 10 + (...) + 11; 
       64 (0x40) = 27 + 10 + (...) + 27;

       char sp1[4] = { 0xF0, 0x9D,  0x84, 0x9E };    // 𝄞
       char sp2[4] = { 0xF0, 0x9D,  0x84, 0xA2 };    // 𝄢
       char sp3[4] = { 0xF0, 0x9F,  0x94, 0x92 };    // 🔒
       char sp4[3] = { 0xE3, 0x80,  0xA0       };    // 〠 
       char sp5[4] = { 0xF0, 0x9F,  0x9B, 0xAA };    // 🛪

        𝄞      size() = 2 QChar(s), character(s) = 1     |   '𝄞' => 2 QChars ,  utf-8 :  "\xF0\x9D\x84\x9E", u32 : U+1D11E = 119070
        𝄢      size() = 2 QChar(s), character(s) = 1     |   '𝄢' => 2 QChars ,  utf-8 :  "\xF0\x9D\x84\xA2", u32 : U+1D122 = 119074
        🔒     size() = 2 QChar(s), character(s) = 1     |   '🔒' => 2 QChars,  utf-8 : "\xF0\x9F\x94\x92", u32 : U+1F512 = 128274
        〠     size() = 1 QChar(s), character(s) = 1     |   '〠' => 1 QChar ,  utf-8 :  "\xE3\x80\xA0"    , u32 : U+3020 = 12320 
        🛪      size() = 2 QChar(s), character(s) = 1     |   '🛪' => 2 QChars ,  utf-8 :  "\xF0\x9F\x9B\xAA", u32 : U+1F6EA = 128746


    ------------------------------------------------------------------------------------------------------------------------------------------------------
        4 th    with 1   :   [ 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31  ] .    16 counts
        3 th    with 1   :   [ 8 11 12 14 15 24 25 26 27 28 29 30 31  ] .    13 counts
        2 th    with 1   :   [ 4 5 6 7 12 14 15 20 21 22 23 28 29 30 31  ] .    15 counts
        1 th    with 1   :   [ 2 3 6 7 11 14 15 18 19 22 23 26 27 30 31  ] .    15 counts
        0 th    with 1   :   [ 1 3 5 7 11 15 17 19 21 23 25 27 29 31  ] .    14 counts
    ------------------------------------------------------------------------------------------------------------------------------------------------------


 ----------------------------------------------------------------------------------------------------
1 x 5 counts :  [ 31  ] .   1 counts
1 x 4 counts :  [ 15 23 27 29 30  ] .   5 counts
1 x 3 counts :  [ 7 11 14 19 21 22 25 26 28  ] .   9 counts
1 x 2 counts :  [ 3 5 6 12 17 18 20 24  ] .   8 counts
1 x 1 counts :  [ 1 2 4 8 16  ] .   5 counts
 ----------------------------------------------------------------------------------------------------

Step by Step
 0. Read the total bytes in the given file and save it the a varible named  'iFileSize' by 1 unsigned int (4 bytes)    A1 A2 A3 A4
 1. Generate a random number inside [0 1 2] (Use expr : random_Number % 3)  maped as [2 3 4]          f(x) = x+4  [ 4 5 6 ]
     [ 
        2  // pick one element from 1 x 2        => 4     2^4 = 16 (fill 16 bytes meta bytes)
        3  // pick one element from 1 x 3        => 5     2^5 = 32 (fill 32 bytes meta bytes)
        4  // pick one element from 1 x 4        => 6     2^6 = 64 (fill 64 bytes meta bytes)
     ]

 2. Generate a random number in the range [ 1, 7 ] as the bit-shift op   and the shift  direction by 1 bit      << :0   or      >> :1
     [ 4 => 16   :     3 + 10 + (...) + 3  ]  
     [ 5 => 32   :    11 + 10 + (...) + 11 ]
     [ 6 => 64   :    27 + 10 + (...) + 27 ]

 3. Generate a random number to indicate use   Big-Endian(0) or Little-Endian(1)
 4. Write the 1st byte with value from step1 [4 5 6]   g(x) = f(x)-2   => [ 2 3 4 ]      pick one number inside the array     1 x 2(8 counts)        1 x 3(8 counts)        1 x 4(8 counts)
 5. if f(x) == 4 , the write sp4[0] and sp4[1] into  the 2nd byte and the 3rd byte , else write down the leader 2 bytes from sp1/sp2/sp3/sp5
 6. write down the key 10 bytes
     
      A4    x x x   A3    x x    A2 x   A1     for Little Endian by the random number from Step 3
      A1    x x x   A2    x x    A3 x   A4     for Big    Endian by the random number from Step 3


      6 x(s) above is about to save the bit-shift op
            1 : 1 of 6 bytes , pick 1 byte and fill it with 1 x 1 counts :  [ 1 2 4 8 16  ] .   5 counts
            2 : 1 + 1
            3 : 1 + 1 + 1
            4 : 1 + 1 + 1 + 1
            5 : 1 + 1 + 1 + 1 + 1
            6 : 1 + 1 + 1 + 1 + 1 + 1
            7 : 2 + 1 + 1 + 1 + 1 + 1   or    1 + 1 + 2 + 1 + 1 + 1

  7. Use the bit-shift op apply to the origin file content   and write down

      12 34                                           AB   CD
      b1 b2 b3 b4 ... b-middle  ...    b [-4] b[-3] b[-2] b[-1]                  

      b1  and  b[-1] ,   assume bit-shift value as 3 bits with <<   ,       12   ...  CD    => 96 68
      b2  and  b[-2] ,   assume bit-shift value as 3 bits with <<   ,       34   ...  AB    => A5 59  

      if 'iFileSize' is an odd number , then the b-middle number use the  << (3) op on itself   1234 5678  =>   4567 8123
      else           for the even number , all byte are make pairs

  8. fill the rest  3/11/27 by the following algorithm
      - 3+10 + (...) + 3

         b[-3]  b[-2]   b[-1]   

            b[-2]  write sp4[2]   // sp4 is very special
         

        otherwise
          b[-4]          b[-3]  b[-2]                                  b[-1]   
                 write last 2 bytes from sp1/sp2/sp3/sp5

      xxxx x?xx      the 2nd bit(the zero bit start at the last bit) from b[-1]        write-down    big-endian(0)   or  little-endian(1)   
      xxx? xxxx      the 4th bit(the zero bit start at the last bit) from b[-3 or -4]  write-down    bit-shift direction                   << :0   or      >> :1


       - other possibilities
            fill dummy bytes from [0~31] without \t \r \n     &&   127    to other placehold byte slot  the make    16/32/64   full




#########################################################################################################################################################
#########################################################################################################################################################
#########################################################################################################################################################

-----------------
    Decryption
-----------------

 1. Read the 1st byte , and count how many 1 bits are there in this byte.   [ count=2  => 4 ]   [ count=3  => 5 ]    [ count=4  => 6 ]     f(x) = x+2;

     ***************************************************************************************************************************************************/
     

    

```
