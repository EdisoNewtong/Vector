#include "sortUtil.h"

/* value can be                  1  or  2      */
static const int G_METHOD_TYPE = 1;


void swap2Elements(int ary[], int idx1, int idx2)
{
    int tmp = ary[idx1];
    ary[idx1] = ary[idx2];
    ary[idx2] = tmp;
}

bool isArray_AscendOrdered(int ary[], int sz)
{
    bool isSorted = true;
    for( int i = 0; i < sz-1; ++i)
    {
        if ( ary[i] > ary[i+1] ) {
            isSorted = false;
            break;
        }
    }

    return isSorted;
}

bool isArray_DescendOrdered(int ary[], int sz)
{
    bool isSorted = true;
    for( int i = 0; i < sz-1; ++i)
    {
        if ( ary[i] < ary[i+1] ) {
            isSorted = false;
            break;
        }
    }

    return isSorted;
}


/********************************************************************************
 
     快速排序法的步骤 (细节阐述)

   Step 1. 
             任取数组中的某一个位置的数字，以它作为中枢元素
             将 除中枢元素以外的 其他元素 分成2组， A,B
                1. A组的数值均小于等于 中枢元素
                2. B组的数值均大于等于 中枢元素
      
            然后将中枢元素 放置于 A,B 2 组的中间，( A,B 两组的相对顺序需要根据需求而改变 ) 构成了一个新的数组 
                                
                                     Group A   ,   Pivot   ,  Group B    ( Ascending  Order )
                               or    Group B   ,   Pivot   ,  Group A    ( Descending Order )

            (有2种排序的可能)
                1. Ascending  Order (从左到右，按从 小 到 大的顺序  : 升序)
                2. Descending Order (从左到右，按从 大 到 小的顺序  : 降序)

            如下图 :


    Step 2.
             以 从小到大的排序准则为例

             此时，以 pivot  为分界元素， 左侧 均比 pivot 小
                                          右侧 均比 pivot 大
           
             然后分别对 ，先<左>侧  后<右>侧的数组   重复进行 Step1 ~ Step2 的处理 ( 递归过程 ) ( <Recursively> apply  quick-sort algorithm for a certain group )

             或           先<右>侧  后<左>侧的  ( 左侧分组 与 右侧分组  的先后执行 ， 可以任意，不影响排序结果的正确性 )
      
      
-------------------------------------------------------------------------------------------------------------------------------------------
Ascending Order :

                 Group A                                              Group B
  _______________________________________               _____________________________________
  |                                     |               |                                    |
  |   ( all number <= pivotNumber )     |  PivotNumber  |  ( all number >= pivotNumber )     |
  |_____________________________________|               |____________________________________|

-------------------------------------------------------------------------------------------------------------------------------------------

Descending Order :

                 Group A                                              Group B
  _______________________________________               _____________________________________
  |                                     |               |                                    |
  |   ( all number >= pivotNumber )     |  PivotNumber  |  ( all number <= pivotNumber )     |
  |_____________________________________|               |____________________________________|

-------------------------------------------------------------------------------------------------------------------------------------------



代码实现的细节 : 
  
     那么如何处理一个数组，使 除中枢元素以外的 其他元素，分组成1组 <= pivotNumber , 另一组 >= pivotNumber 呢
代码实现上，为了方便代码处理，也为了可读性更好，一般取 数组的  第1个元素作为 中枢元素，从第2个起到最后一个元素， 进行AB分组
                                                  或者 数组的最后1个元素作为 中枢元素，从第1个起到倒数第2个元素，进行AB分组


下面，以取第1个元素作为中枢元素为例 () ，  具体说明一下, 如何进行AB分组
      
      设置2个 索引 指针， i,j ， i 指向第1个元素( 为什么不指向第2个元素 ( 虽然 除去第1个元素后，剩余的数组为为是从第2个元素 ~ 最后一个元素 )， 但是这么做会让程序在特殊的情况下，工作不正常 )
                                 j 指向最后一个元素

      i 从 头 --> 尾                   ， 进行遍历， 始终指向 比 pivot 大的数字
      j 从 头 <-- 尾  ( 即 尾 --> 头 ) ， 进行遍历， 始终指向 比 pivot 小的数字

      **********
      ** 但是 ** ， 哪个指针先移动，哪个指针后移动，尤为 !!! 关键 !!!
      **********

      在 i 与 j 没有交错(交错 : 意为 i == j or  i > j ) 的前提下( i < j )

           找到 左侧第1个比 pivot 大的数 ， 找到右侧 倒着数 第1个比 pivot 小的数，
           大数 与 小数， 交换位置

    继续 : 找到 左侧第2个比 pivot 大的数 ， 找到右侧 倒着数 第2个比 pivot 小的数，
           大数 与 小数， 交换位置

    直到 , i 与 j 碰头或交错 ，此时，整个区域都已经被遍历完了1趟
 
----------------------------------------------------------------------------------------------------
Round 0 :
            pivot    i                       j
 init      :  10    17   15   13     7   8   9
             ----
               ^
            pivot   i                        j
 swap      :  10    9    15   13     7   8   17
             ----
               ^
----------------------------------------------------------------------------------------------------
Round 1 :
    
            pivot         i              j
 move i,j  :  10    9    15   13     7   8   17
             ----
               ^
            pivot         i              j
 swap      :  10    9     8   13     7  15   17
             ----
               ^
----------------------------------------------------------------------------------------------------
Round 2 :
            pivot              i     j
 move i,j  :  10    9     8   13     7  15   17
             ----
               ^
            pivot              i     j
 swap      :  10    9     8    7    13  15   17
             ----

----------------------------------------------------------------------------------------------------
Round 3 :
                               j  <- j'
             pivot             i
 move i,j  :  10    9     8    7     13  15   17
             ----
               ^

----------------------------------------------------------------------------------------------------
Final :
               i              pivot
                             ------
swap i,pivot : 7    9     8  | 10 |    13  15   17
                             ------
                                ^

然后，再对 pivot 左侧和右侧的数组进行再次的 QuickSort 算法


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Core Core Core :
4 Question(s) : 
                                                                                                   pivot        i    j
                                                                                                    10     9 8 13    7  15 17

                                                                                              
Q-1      : 最后， 中枢元素 与 小数交换时 ，用 i 还是用 j ?  用 i 指针 ? 
Answer-1 : 一定要用 i 指针

    首先, i 设计的初衷 用于指向比 pivot 大的数字 , 而 j 指针指向比 pivot 小的数字， 最终 pivot 需要 和 小数进行交换 ， 是 j 吗？
    因为，在最后一次 i,j 交换后 , i 指向的数字 与 最初 设计阶段的初衷,发生了反转 , ( 初衷是用于 指向比 pivot 大的数 )
            最后一次 i <--> j 交换后， i 指向了比 pivot 小的数字 ， 而 j 指向了比 pivot 大的数字

因此，需要将 pivotIndex 与 i ( 比 pivot小的数 ) 进行交换，而非 j ( 此时j 指向的数字 已经比 pivot 大了 )


e.g.
                                      pivot < ary[i] | ary[j] < pivot
                                                  i        j
                            pivot             
                           ------
before swap i,j          : | 10 |  9     8       13        7  15  17      
                           ------
                              ^

                            pivot                 
                           ------
after swap i<->j         : | 10 |   9     8       7       13  15  17    
                           ------
                              ^

                             i  <------------> pivot
So :                                          ------
Final swap i <-> pivot  :    7     9     8    | 10 |     13  15   17
                                              ------



Q-2      : i, j   这2个指针的搜索顺序能否颠倒 
         ( 原始顺序 : [先](j)从后往前找到比 pivot 小的数，[后]从前往后找到比 pivot 大的数 )
         ( 颠倒顺序 : [先](i)从前往后找到比 pivot 大的数，[后]从后往前找到比 pivot 小的数 )
Answer-2 : 不能 ( 如果是以第1个数作为中枢元素的话，必须先 j 移动，再 i 移动 )


stop:                     i        j
           10      9  8  13        7  15  17
          -----
            ^
swap:                     i        j
           10      9  8   7       13  15  17
          -----
            ^
move:                     i'  -->  i
                                   j
           10      9  8   7       13  15  17
          -----
            ^
          
           13      9  8   7       10  15  17    
    //   [ERROR]

    // 这一个数组不满足条件了，   10 按照算法的需求，左侧的子数组都比中枢元素(10) 要小，但是， 13 比 10 大， 
    // 算法 !!![无法]!!! 进行进行下去了。



Q-3      : 正向，逆向 查找比 pivot 大/小的数字时，< <=   >= >  ， 是否能滥用 ? 
Answer-3 : 不能

被允许的2元组，由以下2种
1. 
   ary[j] >  pivot
   ary[i] <= pivot

2. 
   ary[j] >= pivot
   ary[i] <= pivot



[不]被允许的2元组，由以下2种
3. 
   ary[j] >= pivot        
   ary[i] <  pivot

         i     j  
 e.g.    10   15  
        pivot

               i
         j
         10   15

          i   
         15   10               // [ERROR] ，期望 pivot 左边的子数组在都比 pivot 要小，但是 15 比 10 要大
             pivot


4. 
   ary[j] >  pivot
   ary[i] <  pivot


          i    j  
 e.g.    15   15              // [ERROR]   infinite loop , i,j never move 
        pivot
        




         

Q-4      : 初始化 i,j 时， i 能否赋值化为 (pivotIdx + 1) ?
Answer-4 : 不能 ( 可能有些人会想，假定ary[0]设置为 pivotNumber, 那么，需要针对 ary[ 1 ... ary.size-1 ] 这个子数组，进行以中枢元素为准则的左右分组 排序

         但是 : 如果 pivot 指向 ary[1] ，会有反例的情况发生

e.g.     
      ary[0]     i=0+1
     -------
     |  7  |      10
     -------
      pivot      j = 1
          


      此时，  i 与 j 已经相等了。  i,j 无法进入再向前/向后 移动
      直接 ary[i or j] <-->  ary[0] 交换 ( 因为 i,j 是相等了，无所谓用i , 还是j )

         
       
        10        7     // [ERROR] : 交换完后，中枢元素左侧的子数组，有比 7 大的数 ，不满足条件了。
                pivot

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


*********************************************************************************/





void quickSortHead_AscendOrder(int ary[], int begIdx, int endIdx, int arySz)
{

    //////////////////////////////////////////////////
    //      Real code execute from here
    if (  begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = begIdx; // set pivot number as the head number
    int pivotNum = ary[pivotIdx]; 

    int i = begIdx;
    int j = endIdx;
    while ( i < j )
    {
        while ( i < j  &&    ary[j] >   pivotNum ) { --j; }
        if ( G_METHOD_TYPE == 2 ) { ary[i] = ary[j]; }

        while ( i < j  &&    ary[i] <=  pivotNum ) { ++i; } 
        if ( G_METHOD_TYPE == 2 ) { ary[j] = ary[i]; }

        if ( G_METHOD_TYPE == 1 ) { swap2Elements(ary, i, j); }
    }

    if ( G_METHOD_TYPE == 2 ) {
        ary[i] = pivotNum;
    } else if ( G_METHOD_TYPE == 1 ) { 
        swap2Elements(ary, pivotIdx, i);
    }

    quickSortHead_AscendOrder(ary, begIdx, i-1, arySz);   // quickSort left  part
    quickSortHead_AscendOrder(ary, i+1, endIdx, arySz);   // quickSort right part
}



void quickSortHead_DescendOrder(int ary[], int begIdx, int endIdx, int arySz)
{
    if (  begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = begIdx; // set pivot number as the head number
    int pivotNum = ary[pivotIdx]; 

    int i = begIdx;
    int j = endIdx;
    while ( i < j )
    {
        while ( i < j  &&    ary[j] <  pivotNum ) { --j; } 
        if ( G_METHOD_TYPE == 2 ) { ary[i] = ary[j]; }

        while ( i < j  &&    ary[i] >= pivotNum ) { ++i; }
        if ( G_METHOD_TYPE == 2 ) { ary[j] = ary[i]; }
        
        if ( G_METHOD_TYPE == 1 ) { swap2Elements(ary, i, j); }
    }

    if ( G_METHOD_TYPE == 2 ) {
        ary[i] = pivotNum;
    } else if ( G_METHOD_TYPE == 1 ) {
        swap2Elements(ary, pivotIdx, i);
    }

    quickSortHead_DescendOrder(ary, begIdx, i-1, arySz);   // quickSort left  part
    quickSortHead_DescendOrder(ary, i+1, endIdx, arySz);   // quickSort right part

}




void quickSortTail_AscendOrder(int ary[], int begIdx, int endIdx, int arySz)
{
    if ( begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = endIdx; // set pivot number as the tail number
    int pivotNum = ary[pivotIdx];

    int i = begIdx;
    int j = endIdx;
    while ( i < j )
    {
        while ( i < j  &&    ary[i] <  pivotNum ) { ++i; }
        if ( G_METHOD_TYPE == 2 ) { ary[j] = ary[i]; }

        while ( i < j  &&    ary[j] >= pivotNum ) { --j; }
        if ( G_METHOD_TYPE == 2 ) { ary[i] = ary[j]; }

        if ( G_METHOD_TYPE == 1 ) {
            swap2Elements(ary, i, j);
        }
    }

    if ( G_METHOD_TYPE == 2 ) {
        ary[j] = pivotNum;
    } else if ( G_METHOD_TYPE == 1 ){
        swap2Elements(ary, pivotIdx, j);
    }

    quickSortTail_AscendOrder(ary, begIdx, j-1, arySz);   // quickSort left  part
    quickSortTail_AscendOrder(ary, j+1, endIdx, arySz);   // quickSort right part
}


void quickSortTail_DescendOrder(int ary[], int begIdx, int endIdx, int arySz)
{    
    if ( begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = endIdx; // set pivot number as the tail number
    int pivotNum = ary[pivotIdx];

    int i = begIdx;
    int j = endIdx;
    while ( i < j )
    {
        while ( i < j  &&    ary[i] >  pivotNum ) { ++i; }
        if ( G_METHOD_TYPE == 2 ) { ary[j] = ary[i]; }

        while ( i < j  &&    ary[j] <= pivotNum ) { --j; }
        if ( G_METHOD_TYPE == 2 ) { ary[i] = ary[j]; }

        if ( G_METHOD_TYPE == 1 ) { swap2Elements(ary, i, j); }
    }

    if ( G_METHOD_TYPE == 2 ) {
        ary[j] = pivotNum;
    } else if( G_METHOD_TYPE == 1 ) {
        swap2Elements(ary, pivotIdx, j);
    }
    quickSortTail_DescendOrder(ary, begIdx, j-1, arySz);   // quickSort left  part
    quickSortTail_DescendOrder(ary, j+1, endIdx, arySz);   // quickSort right part
}

