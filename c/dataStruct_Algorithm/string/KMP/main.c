#include <stdio.h>
#include <stdlib.h>

/* 
Test Case :
 
   abcFwwwabcF!
   abcabaa

   abd345abc6788abd345abd!
   abd345abc6788abd345aba!
   abd345abc6788abd345abw!


*/

/*
**************************************************
  BF algorithm
--------------------------------------------------
arguments :
    s       : search  string
    pattern : pattern string
--------------------------------------------------
return :
         the 1st position that pattern matches inside the search string (the returned index starts from 0) 
         if not found , return -1

**************************************************
*/
int BruteForceSearch(const char* src, const char* pattern)
{
    int retIdx = -1; // assign as not found flag first
    int i = 0;
    int j = 0;
    
    while ( src[i] != '\0'   &&   pattern[j] != '\0' )
    {
        if ( src[i] == pattern[j] ) {
            ++i;
            ++j;
        } else {
            // (i - j)        :  the position which pattern's 1st character is align with search's match head
            // (i - j) + 1    :  the position next to    the head of the matched src
            i = ( i - j ) + 1;
            // jump back to the head pointer of the pattern string
            j = 0; 
        }
    }


    /******************************************************************************************************
	 *
	     这里要特别说明一下，一些特殊的情况，如 目标串 或 模式串中有 空串

 --------------------------------------------------------------------------------------------------------------------------------------------------
    |                              |      数学中，有一个约定:                         |  逻辑与左侧的情况有一定的类似，属于一种特例 :
    |                              | (空集)是任何(非空集合)的<子集>                   |       A集合 包含于 A集合 (自已包含自己， A是A的子集) : 在任何时刻，<永远>是<成立>的。
	|                              | Or ( See the following Logic )                   |  因此 返回的结果为 i-j = (0 - 0) = 0;
	|                              |                                                  |
    |                              |       if ( p[j] == '\0' ) {                      |  if ( p[j] == '\0' ) {               
	|                              |       {                                          |  {                                   
	|                              |            // i = 0 && j = 0;                    |     // i = 0 && j = 0;               
	|                              |            // Match up loop logic                |     // Match up loop logic           
	|							   |            //     has <No Chance> to execute     |     //     has <No Chance> to execute
	|                              |            foundIdx = (i-j);                     |     foundIdx = (i-j);                
	|                              |       }                                          |  }                                   
 ---------------------------------------------------------------------------------------------------------------------------------------------------
	|	 target  = ""              |   target  = "aaa"                                |   target  = ""
	|	 pattern = "aaa"           |   pattern = ""                                   |   pattern = ""
	|	 "".find("aaa")   = -1;    |   "aaa".find("") = 0                             |   "".find("") = 0
	|                              |                                                  |
	|                              |                                                  |
	*******************************************************************************************************/
    if ( pattern[j] == '\0' ) {
        retIdx = i - j;
    }

    return retIdx;
}













/***************************************************
*                                                 *
*                                                 *
*  KMP (Knuth-Morris-Pratt) algorithm             *
*                                                 *
*                                                 *
***************************************************/
void calcNextArray(const char* pattern, int next[], int aryLen)
{
    int i = 0;
    int k = -1;
    next[0] = -1;

	/*
    while ( i <  aryLen    )    // in calc NextValue Array function , the final travelsal length is (aryLen) rather than (aryLen-1)    */
    while ( i < (aryLen-1) )
    {
        if ( k == -1 || pattern[i] == pattern[k] )
        {
            ++i;
            ++k;
            next[i] = k;
        }
        else
        {
            k = next[k];
        }
    }

}

void calcNextValArray(const char* pattern, int nextval[], int aryLen)
{
    int i = 0;
    int k = -1;
    nextval[0] = -1;


	/******************************************************************************************************
	 
	   这里有一个Bug  while 的条件表达式不能是  i < aryLen

		e.g.

			初始条件为 : 	pattern = "a";  i = 0; k = -1; nextval[0] = -1;  aryLen = 1;
			那么 while ( i < aryLen ) 条件成立, 进入循环体内 ( 但是 nextval[0] 已经被设置了 )
			           ( 0 < 1 )

			++i; ( => i=1 )     ++k;  ( => k=0 ) , 此时，bug 就出现了， pattern[1] 是 !!!!<一种数组越界的访问> !!!!  ( Bug-1 Here )

			而 nextval 这个数组的长度应该是 1， 因此无论最终 if 条件是否成立， nextval[1] = xxx;   都要被改写  !!!!< 一种数组越界的写操作 > !!!! ( Bug-2 Here , Bug-2 由 Bug-1 导致 )

			如果 pattern 是一个以 '\0' 结尾的C样式的字符串，那么 pattern[k] != '\0' ，一定成立， nextval[1] = 0; (数组越界操作， 非法写入)





			但是 : 如果 pattern 是一个结构体的成员的话，那么就有可能 非法访问 pattern的后续的内存空间
		e.g.
			typedef struct WrapPattern {
			    //            0123456
				// pattern = "abc-abc"    
				//    if    other[0] = '-'    nextval[7] = 0 ( illegal access   other[0] by exp : pattern[7] , illegal write nextval[7] )
				// else if  other[0] = 'w'    nextval[7] = 3 ( illegal access   other[0] by exp : pattern[7] , illegal write nextval[7] )
				//
				char pattern[7];           
				char otherString[4];
			};
				
			条件 (i < aryLen)  被 ++i; 这个语句给破坏之前的逻辑关系，因此非法读取  pattern[ aryLen ] 引发了 nextval[ aryLen ] 的写法



              可以把  while ( i < (aryLen  ) )
		      修改成  while ( i < (aryLen-1) )

		或者添加 i 的越界条件判定 

			   ++i; ++k;
			+ if ( i >= aryLen ) {
			+    break;
			+ }

	******************************************************************************************************/


    // while ( i <  aryLen    )  // It is a bug (无论 pattern 是以 "\0" 结尾的，或者是一个结构体的数据成员 ，都会诱发 memory bug )
	
    /*  
	while ( i <  (aryLen-1) )     // in calc NextValue Array function , the final travelsal length is  (aryLen-1)  rather than (aryLen) 
	while ( i <   aryLen    )     // It is a bug (无论 pattern 是以 "\0" 结尾的，或者是一个结构体的数据成员 ，都会诱发 memory bug )        */
    while ( i <  (aryLen-1) )
    {
        if ( k == -1 || pattern[i] == pattern[k] )
        {
            ++i;
            ++k;
            if ( pattern[k] != pattern[i] ) {
                nextval[i] = k;
            } else {
                nextval[i] = nextval[k];
            }
        }
        else
        {
            k = nextval[k];
        }
    }

}


int KMPSearch_NextArray(const char* src, const char* pattern)
{
    int retIdx = -1; // assign as not found flag first
    int patternLen = 0;
    while ( pattern[patternLen] != '\0' ) {
        ++patternLen;
    }

    int* nextAry = (int*)malloc( sizeof(int) * patternLen);
    calcNextArray(pattern, nextAry, patternLen);

    int i = 0;
    int j = 0;

    while ( src[i] != '\0'   &&   pattern[j] != '\0' )
    {
        if ( j == -1    ||  src[i] == pattern[j] ) {
            ++i;
            ++j;
        } else {
            j = nextAry[j]; 
        }
    }

    // travesal till the terminate character ('\0') of the pattern string
    if ( pattern[j] == '\0' ) {
        retIdx = i - j;
    }

    free(nextAry);
    nextAry = NULL;
    return retIdx;
}


int KMPSearch_NextValue_Array(const char* src, const char* pattern)
{
    int retIdx = -1; // assign as not found flag first
    int patternLen = 0;
    while ( pattern[patternLen] != '\0' ) {
        ++patternLen;
    }

    int* nextValue_Array = (int*)malloc( sizeof(int) * patternLen);
    calcNextValArray(pattern, nextValue_Array, patternLen);
    // for( int idx = 0; idx < patternLen; ++idx ) {
    //     printf("%d. '%c' -> nextval = %d\n",idx+1, pattern[idx], nextValue_Array[idx] );
    // }

    int i = 0;
    int j = 0;

    while ( src[i] != '\0'   &&   pattern[j] != '\0' )
    {
        if ( j==-1  ||  src[i] == pattern[j] ) {
            ++i;
            ++j;
        } else {
            // jump back to the head pointer of the pattern string
            j = nextValue_Array[j]; 
        }
    }

    // travesal till the terminate character ('\0') of the pattern string
    if ( pattern[j] == '\0' ) {
        retIdx = i - j;
    }

    free(nextValue_Array);
    nextValue_Array = NULL;
    return retIdx;
}


int main(int argc, char* argv[])
{
    if ( argc != 3 ) {
        printf("[ERROR] : Missing 2 arguments. \n");
        return -1;
    }

    const char* searchString  = argv[1];
    const char* patternString = argv[2];
	// if ( searchString[0]  == 'x' )  { searchString = "";  }
	// if ( patternString[0] == 'x' ) { patternString = ""; }

    int foundPos = 0;   // BruteForce_1(searchString, patternString);
    
    /*
    printf("[TEST] : foundPos = %d , found \"\" from \"\"\n", BruteForceSearch("",""));

    printf("[TEST] : foundPos = %d , found \"a\" from \"\"\n", BruteForceSearch("","a"));
    printf("[TEST] : foundPos = %d , found \"ab\" from \"\"\n", BruteForceSearch("","ab"));

    printf("[TEST] : foundPos = %d , found \"\" from \"a\"\n", BruteForceSearch("a",""));
    printf("[TEST] : foundPos = %d , found \"\" from \"ab\"\n", BruteForceSearch("ab",""));

    printf("[TEST] : foundPos = %d , found \"abc\" from \"ab\"\n", BruteForceSearch("ab","abc"));
    printf("[TEST] : foundPos = %d , found \"abc\" from \"adacab\"\n\n", BruteForceSearch("adacab","abc"));

    foundPos = BruteForceSearch(searchString, patternString);
    */


/*  foundPos = BruteForceSearch(searchString, patternString);     */
/*  foundPos = KMPSearch_NextArray(searchString, patternString);  */
    foundPos = KMPSearch_NextValue_Array(searchString, patternString);
    if ( foundPos == -1 ) {
        printf("[INFO] : Can't found \"%s\" inside \"%s\"\n", patternString, searchString );
    } else {
        printf("[INFO] : Found pattern \"%s\" inside \"%s\" at position [%d]\n", patternString, searchString, foundPos );
    }

    return 0;
}
