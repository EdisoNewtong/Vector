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

# Compile Issue 

$ gcc --version
***==g++ (GCC) 4.8.5 20150623 (Red Hat 4.8.5-44)==***
Copyright © 2015 Free Software Foundation, Inc.
本程序是自由软件；请参看源代码的版权声明。本软件没有任何担保；
包括没有适销性和某一专用目的下的适用性担保。


 
 
***==Because for some old compiler , like link-table , the data type of a member can't be the struct itself==*** 


$ g++   -Wall   -std=c++11    -O3   -o   replaceChinesePunctuation    replaceChinesePunctuation.cpp
> g++   -Wall   -std=c++11    -O3   -o   replaceChinesePunctuation    replaceChinesePunctuation.cpp
> In file included from /usr/include/c++/4.8.2/bits/stl_algobase.h:64:0,
>                  from /usr/include/c++/4.8.2/bits/char_traits.h:39,
>                  from /usr/include/c++/4.8.2/ios:40,
>                  from /usr/include/c++/4.8.2/ostream:38,
>                  from /usr/include/c++/4.8.2/iostream:39,
>                  from replaceChinesePunctuation.cpp:1:
> /usr/include/c++/4.8.2/bits/stl_pair.h: In instantiation of ‘struct std::pair<const char, nextCandidateMap>’:
> /usr/include/c++/4.8.2/type_traits:615:28:   required from ‘struct std::__is_destructible_impl<std::pair<const char, nextCandidateMap> >’
> /usr/include/c++/4.8.2/type_traits:637:12:   required from ‘struct std::__is_destructible_safe<std::pair<const char, nextCandidateMap>, false, false>’
> /usr/include/c++/4.8.2/type_traits:652:12:   required from ‘struct std::is_destructible<std::pair<const char, nextCandidateMap> >’
> /usr/include/c++/4.8.2/type_traits:116:12:   required from ‘struct std::__and_<std::is_destructible<std::pair<const char, nextCandidateMap> >, std::__is_direct_constructible_impl<std::pair<const char, nextCandidateMap>, const std::pair<const char, nextCandidateMap>&> >’
> /usr/include/c++/4.8.2/type_traits:817:12:   required from ‘struct std::__is_direct_constructible_new_safe<std::pair<const char, nextCandidateMap>, const std::pair<const char, nextCandidateMap>&>’
> /usr/include/c++/4.8.2/type_traits:895:12:   [ skipping 4 instantiation contexts, use -ftemplate-backtrace-limit=0 to disable ]
> /usr/include/c++/4.8.2/type_traits:968:12:   required from ‘struct std::__is_copy_constructible_impl<std::pair<const char, nextCandidateMap>, false>’
> /usr/include/c++/4.8.2/type_traits:974:12:   required from ‘struct std::is_copy_constructible<std::pair<const char, nextCandidateMap> >’
> /usr/include/c++/4.8.2/bits/alloc_traits.h:540:12:   required from ‘struct std::__is_copy_insertable<std::allocator<std::pair<const char, nextCandidateMap> > >’
> /usr/include/c++/4.8.2/bits/alloc_traits.h:560:63:   required by substitution of ‘template<class _Alloc> using __check_copy_constructible = std::__allow_copy_cons<std::__is_copy_insertable<_Alloc>::value> [with _Alloc = std::allocator<std::pair<const char, nextCandidateMap> >]’
> /usr/include/c++/4.8.2/bits/unordered_map.h:97:11:   required from ‘class std::unordered_map<char, nextCandidateMap>’
> replaceChinesePunctuation.cpp:43:40:   required from here
> /usr/include/c++/4.8.2/bits/stl_pair.h:102:11: 错误：‘std::pair<_T1, _T2>::second’类型不完全
>        _T2 second;                /// @c second is a copy of the second object
>            ^
> replaceChinesePunctuation.cpp:37:8: 错误：‘struct nextCandidateMap’的前向声明
>  struct nextCandidateMap {
>         ^
> In file included from /usr/include/c++/4.8.2/bits/stl_algobase.h:64:0,
>                  from /usr/include/c++/4.8.2/bits/char_traits.h:39,
>                  from /usr/include/c++/4.8.2/ios:40,
>                  from /usr/include/c++/4.8.2/ostream:38,
>                  from /usr/include/c++/4.8.2/iostream:39,
>                  from replaceChinesePunctuation.cpp:1:
> /usr/include/c++/4.8.2/bits/stl_pair.h: In instantiation of ‘constexpr std::pair<_T1, _T2>::pair(std::pair<_U1, _U2>&&) [with _U1 = char; _U2 = nextCandidateMap; <模板形参-2-3> = void; _T1 = const char; _T2 = nextCandidateMap]’:
> /usr/include/c++/4.8.2/type_traits:801:43:   required from ‘struct std::__is_direct_constructible_impl<std::pair<const char, nextCandidateMap>, std::pair<char, nextCandidateMap>&&>’
> /usr/include/c++/4.8.2/type_traits:116:12:   required from ‘struct std::__and_<std::is_destructible<std::pair<const char, nextCandidateMap> >, std::__is_direct_constructible_impl<std::pair<const char, nextCandidateMap>, std::pair<char, nextCandidateMap>&&> >’
> /usr/include/c++/4.8.2/type_traits:817:12:   required from ‘struct std::__is_direct_constructible_new_safe<std::pair<const char, nextCandidateMap>, std::pair<char, nextCandidateMap>&&>’
> /usr/include/c++/4.8.2/type_traits:895:12:   required from ‘struct std::__is_direct_constructible_new<std::pair<const char, nextCandidateMap>, std::pair<char, nextCandidateMap>&&>’
> /usr/include/c++/4.8.2/type_traits:903:12:   required from ‘struct std::__is_direct_constructible<std::pair<const char, nextCandidateMap>, std::pair<char, nextCandidateMap>&&>’
> /usr/include/c++/4.8.2/type_traits:944:12:   required from ‘struct std::__is_constructible_impl<std::pair<const char, nextCandidateMap>, std::pair<char, nextCandidateMap>&&>’
> /usr/include/c++/4.8.2/type_traits:955:12:   required from ‘struct std::is_constructible<std::pair<const char, nextCandidateMap>, std::pair<char, nextCandidateMap>&&>’
> /usr/include/c++/4.8.2/bits/unordered_map.h:365:32:   required from here
> /usr/include/c++/4.8.2/bits/stl_pair.h:152:40: 错误：使用无效字段‘std::pair<_T1, _T2>::second’
>     second(std::forward<_U2>(__p.second)) { }
>                                         ^
> replaceChinesePunctuation.cpp: 在函数‘void insertInfoMap(const string&, NextMap*, char)’中:
> replaceChinesePunctuation.cpp:88:51: 错误：‘struct std::pair<const char, nextCandidateMap>’没有名为‘second’的成员
>       pWhichList = &( (pWhichList->find(singleCh)->second).nextMap );
>                                                    ^
> replaceChinesePunctuation.cpp:98:34: 错误：‘struct std::pair<const char, nextCandidateMap>’没有名为‘second’的成员
>       pWhichList->find(singleCh)->second.replaceWith = toReplaceWith;
>                                   ^
> replaceChinesePunctuation.cpp:106:29: 错误：‘struct std::pair<const char, nextCandidateMap>’没有名为‘second’的成员
>      pWhichList = &(foundIt->second).nextMap;
>                              ^
> replaceChinesePunctuation.cpp: 在函数‘void printSpecialDataStruct(const NextMap&, int)’中:
> replaceChinesePunctuation.cpp:219:27: 错误：‘const struct std::pair<const char, nextCandidateMap>’没有名为‘second’的成员
>    NextMap nextLayer = it->second.nextMap;
>                            ^
> replaceChinesePunctuation.cpp:229:39: 错误：‘const struct std::pair<const char, nextCandidateMap>’没有名为‘second’的成员
>     cout << "   replace  ==> '" << it->second.replaceWith << "\' " << endl;
>                                        ^
> replaceChinesePunctuation.cpp: 在函数‘void tryReplaceChinesePuncPunctuation(const processArg&, const NextMap&, fileCharInfo*&, size_t&, std::vector<fileCharInfo>&)’中:
> replaceChinesePunctuation.cpp:608:30: 错误：‘struct std::pair<const char, nextCandidateMap>’没有名为‘second’的成员
>      char repacedWithCh = it->second.replaceWith;
>                               ^
> replaceChinesePunctuation.cpp:609:18: 错误：‘struct std::pair<const char, nextCandidateMap>’没有名为‘second’的成员
>      pMap = &(it->second.nextMap);
>                   ^
> make: *** [replaceChinesePunctuation] 错误 1


