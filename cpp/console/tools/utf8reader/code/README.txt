
1. create an empty directory named 'build'  if directory 'build' is not existed
2. $ make


/*
 ****************************************************************************************************
						UTF8  Core Core Core Principle
 ****************************************************************************************************
 
UTF-8 就是使用变长字节表示,顾名思义，就是使用的字节数可变，这个变化是根据 Unicode 编号的大小有关，编号小的使用的字节就少，编号大的使用的字节就多。使用的字节个数从 1 到 4 个不等。
UTF-8 的编码规则是：
 
1. 对于1个字节(单字节)的符号:
	 字节的第一位设为 0，
	 后面的7位为这个符号的 Unicode 码，
	 因此对于英文字母，UTF-8 编码和 ASCII 码是相同的。
 
2. 对于n字节的符号 （n>1）:
   第一个字节的前 n 位都设为 1，  ( 也就是说 至少有2个11  , 即  肯定是以下3种的其中之一的情况
	<1>.    110X,XXXX | 10XX,XXXX |								(2 bytes to build an Non-Ansi Character) , Valid bits = 5 + 6 = 11		   -> 1 byte  + 3 bits (need 2 bytes) ( so unicode32 = U+XXXX   )
	<2>.    1110,XXXX | 10XX,XXXX | 10XX,XXXX |					(3 bytes to build an Non-Ansi Character) , Valid bits = 4 + 6 + 6 = 16     -> 2 bytes		   (need 2 bytes) ( so unicode32 = U+XXXX   )
	<3>.    1111,0XXX | 10XX,XXXX | 10XX,XXXX | 10XX,XXXX |		(4 bytes to build an Non-Ansi Character) , Valid bits = 3 + 6 + 6 + 6 = 21 -> 2 bytes + 5 bits (need 3 bytes) ( so unicode32 = U+XXXXXX )
 
	因为Unicode 最多使用4个字节表示一个文字 )
 
  第 n+1 位设为 0
		第2~4个字节的前两位一律设为 10
	 	剩下的没有提及的二进制位，全部为这个符号的 Unicode 码 。

Special Chinese  Character : 
-----
|𠄘 |     // string s = "\xF0\xA0\x84\x98";
-----

UTF-8      : 0xF0 0xA0 0x84 0x98    
Unicode 32 : U+20118

------
|䕫 |     // string s = "\xF0\xAF\xA6\xB2"; (拼音 : kui 4 )
------

UTF-8      : 0xF0 0xAF 0xA6 0xB2
Unicode 32 : U+2F9B2
 
 ****************************************************************************************************
*/



