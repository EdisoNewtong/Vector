# Test Case Command Line 

```bash
$ ./replaceChinesePunctuation   --input   backup/testCase/t1.txt   --output   output.txt
```


# About the data struct in this program whose purpose is to replace some chinese punctuation into english version  
The data structure is a little bit like the ***==blacklist==*** words to ban. ( 屏蔽字列表 算法 )


suppose you want to replace some chinese punctuation with   english version punctuation

The following is a **==table==** list all kinds of replacement :

| number | original |                 Utf8 Codec               | replace as  |              Comment            |
|:------:|:--------:|:----------------------------------------:|:-----------:|:-------------------------------:|
| 01 | ｀or ·       | ==EF== BD 80   C2 B7                     |  `          |                                 |
| 02 | ～           | ==EF== BD 9E                             |  ~          |                                 |
| 03 |  '　'        | E3 80 80                                 | ' '         |   This line is the <Space> key  |
| 04 | ！           | ==EF== ==BC== 81                         |  !          |                                 |
| 05 | ＠           | ==EF== ==BC== A0                         |  @          |                                 |
| 06 | ＃           | ==EF== ==BC== 83                         |  #          |                                 |
| 07 | ＄ or ￥     | ==EF== ==BC== 84   ==EF== BF A5          |  $          |                                 |
| 08 | ％           | ==EF== ==BC== 85                         |  %          |                                 |
| 09 | ＾ or ……     | ==EF== ==BC== BE   E2 80 A6              |  ^          |                                 |
| 10 | ＆           | ==EF== ==BC== 86                         |  &          |                                 |
| 11 | ＊ or ×      | ==EF== ==BC== 8A   C3 97 0A              |  *          |                                 |
| 12 | （           | ==EF== ==BC== 88                         |  (          |                                 |
| 13 | ）           | ==EF== ==BC== 89                         |  )          |                                 |
| 14 | －           | ==EF== ==BC== 8D                         |  -          |                                 |
| 15 | ＿ or ——     | ==EF== ==BC== BF   E2 80 94 + E2 80 94   |  _          |                                 |
| 16 | ＝           | ==EF== ==BC== 9D                         |  =          |                                 |
| 17 | ＋           | ==EF== ==BC== 8B                         |  +          |                                 |
| 18 | ［ or 【     | ==EF== ==BC== BB   E3 80 90              |  [          |                                 |
| 19 | ｛           | ==EF== BD 9B                             |  {          |                                 |
| 20 | ］ or 】     | ==EF== ==BC== BD   E3 80 91              |  ]          |                                 |
| 21 | ｝           | ==EF== BD 9D                             |  }          |                                 |
| 22 | ＼ or 、     | ==EF== ==BC== BC   E3 80 81              |  \          |                                 |
| 23 | ｜           | ==EF== BD 9C                             |  \|         |                                 |
| 24 | ；           | ==EF== ==BC== 9B                         |  ;          |                                 |
| 25 | ：           | ==EF== ==BC== 9A                         |  :          |                                 |
| 26 | ＇ or ‘ or ’ | ==EF== ==BC== 87   E2 80 98    E2 80 99  |  '          |                                 |
| 27 | ＂ or “ or ” | ==EF== ==BC== 82   E2 80 9C    E2 80 9D  |  "          |                                 |
| 28 | ，           | ==EF== ==BC== 8C                         |  ,          |                                 |
| 29 | ＜ or 《     | ==EF== ==BC== 9C   E3 80 8A              |  <          |                                 |
| 30 | ． or 。     | ==EF== ==BC== 8E   E3 80 82              |  .          |                                 |
| 31 | ＞ or 》     | ==EF== ==BC== 9E   E3 80 8B              |  >          |                                 |
| 32 | ／           | ==EF== ==BC== 8F                         |  /          |                                 |
| 33 | ？           | ==EF== ==BC== 9F                         |  ?          |                                 |
|    |              |                                  |             |                                 |


Obviousely , You can find that almost all the characters which is made from two or more bytes have some same leader bytes 
** Start With byte  =='EF'== **
** And the 2nd byte =='BC'== **


So the data structure would be like the following style

----------------------------------------------------------------------------------------------------

C3
	97   replace  ==> '*' 

E3
	80
		8B   replace  ==> '>' 
		82   replace  ==> '.' 
		8A   replace  ==> '<' 
		81   replace  ==> '\' 
		91   replace  ==> ']' 
		90   replace  ==> '[' 
		80   replace  ==> ' ' 


C2
	B7   replace  ==> '`' 


E2
	80
		9D   replace  ==> '"' 
		9C   replace  ==> '"' 
		98   replace  ==> ''' 
		94
			E2
				80
					94   replace  ==> '_' 
		99   replace  ==> ''' 
		A6
			E2
				80
					A6   replace  ==> '^' 
EF
	BF
		A5   replace  ==> '$' 
	BC
		8F   replace  ==> '/' 
		8E   replace  ==> '.' 
		9C   replace  ==> '<' 
		8C   replace  ==> ',' 
		87   replace  ==> ''' 
		9A   replace  ==> ':' 
		9B   replace  ==> ';' 
		9F   replace  ==> '?' 
		82   replace  ==> '"' 
		BC   replace  ==> '\' 
		8B   replace  ==> '+' 
		9E   replace  ==> '>' 
		BB   replace  ==> '[' 
		81   replace  ==> '!' 
		86   replace  ==> '&' 
		BD   replace  ==> ']' 
		A0   replace  ==> '@' 
		83   replace  ==> '#' 
		9D   replace  ==> '=' 
		84   replace  ==> '$' 
		BE   replace  ==> '^' 
		8A   replace  ==> '*' 
		88   replace  ==> '(' 
		89   replace  ==> ')' 
		8D   replace  ==> '-' 
		85   replace  ==> '%' 
		BF   replace  ==> '_' 
	BD
		9C   replace  ==> '|' 
		9D   replace  ==> '}' 
		9B   replace  ==> '{' 
		9E   replace  ==> '~' 
		80   replace  ==> '`'

----------------------------------------------------------------------------------------------------

The key-value pair collections ( unordered_map<key,value> ) is much suitble 
the Key is a char
What's more : the Value is also a new instance of  unordered_map<key,value> . It's a typical **==recursive==** data structure like the link-table 


unordered_map<char,    
               unordered_map<char,    
                                   unordered_map<char,
                                                       unordered_map<char,    
                                                                           ...
                                                                               >
                                                                                > 
                                                                                  >
                                                                                    >    toReplacedWordsMaped


== So the struct defination would be == : 

```c++
/**
 * Because for some old compiler , like link-table , the data type of a member can't be the struct itself
 * e.g.
 *           struct Node {
 *                            int value;
 *                            // struct Node   Next;   // Compiler Error 
 *                            struct Node*   pNext;    
 *           };
 *
 *           So the struct nextCandidateMap will need the some restriction of the above situation
 *           
 *            User pointer instead
 *
 * What's more : the recursive end-up condition is that    nextMap.empty() =>   true
 *
 * There must a root object of    nextCandidateMap
 * e.g.
 * 
 *          nextCandidateMap* pRoot = new nextCandidateMap();
 *          pRoot->nextMap.clear();

 *          //
 *          // ～ :  EF BD 9E
 *          //
 * 
 *          char ch1 = 0xEF;                      // a new empty hash-table
 *          pRoot->nextMap.insert( make_pair(ch1, new nextCandidateMap() );
 *          
 *          char ch2 = 0xBD;
 *          nextCandidateMap* nxtMap = pRoot->nextMap.begin()->second;
 *                                         // a new empty hash-table
 *          nxtMap->insert( make_pair(ch2, new nextCandidateMap() );
 *
 *          char ch3 =  0x9E
 *          nxtMap = nxtMap->begin()->second;
 *                                         // this is the final hash-table
 *          nxtMap->insert( make_pair(ch3, new nextCandidateMap() );
 *          
*/
struct nextCandidateMap {

    #ifdef USE_POINTER_VERSION_DATA_STRUCT
        // HERE :      ===>  recursive hash-table data struct   <==
    	unordered_map<char, nextCandidateMap*> nextMap;
    #else
    	unordered_map<char, nextCandidateMap>  nextMap;
    #endif

	      char replaceWith;
};

```
