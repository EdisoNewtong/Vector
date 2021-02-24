#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

void swap2AryNumbers(int* pAry, int idx1, int idx2, int sz)
{
    if ( pAry == nullptr ) {
        cout << "[ERROR] , swap-array is nullptr" << endl;
        return;
    }

    if ( idx1 == idx2 ) {
        return;
    }
    
    if (   !(idx1 >= 0    &&   idx1 < sz) ) {
        cout << "[ERROR] , swap-idx1 is out of range , = " << idx1 << endl;
        return;
    }

    if (   !(idx2 >= 0    &&   idx2 < sz) ) {
        cout << "[ERROR] , swap-idx2 is out of range , = " << idx2 << endl;
        return;
    }

    int tmp = pAry[idx1];
    pAry[idx1] = pAry[idx2];
    pAry[idx2] = tmp;
}

/*
    先说一下，用程序实现快速排序的基本思路

    1. 给定一个1维数组，    任何选取其中的一个数字 作为 中枢元素
    2. 使其他数字，进行归位 , 归位需要同时满足以下3个条件 
        1. 比 中枢元素<小>的，   放置在 中枢元素的左侧
        2. 比 中枢元素<大>的，   放置在 中枢元素的右侧
        3. 与 中枢元素<相等>的， 放置在 左侧/右侧  都可以
    3. 再对 左侧的 子数组, 右侧的 子数组进行排序


这里让数组按 升序 进行排序，即 从左往右，按从小到大的 顺序进行排序
       
----------------------------------------------------------------------------------------------------

Step1  首先来解决 中枢元素的选取问题 : 令 数组(或者 子数组) 的第1个元素(首元素) 作为 中枢元素 , 当然也可以选择 最后一个元素, 最好不要选  中间的元素，因为程序不容易实现, 后续会进行解释 QQQ-1
Step2. 先设置从左向右的索引游标 i , 用于标识比中枢元素大的索引位 ,  -->  逐个比较 ，直到找到 比 中枢元素大的元素，停止
Step3. 然后地从右向左的索引游标 j , 用于标识比中枢元素小的索引位 ,  <--  逐个比较 ，直到找到 比 中枢元素小的元素，停止 ( 事实上Step2 与 Step3 可以进行先后位置的交换 )
Step4. 交换，Step2 与 Step3 中的2个数的 swap    ary[i]  <--> ary[j]  , 使得 小数在左,    (中枢元素稍后才归位)    大数在右
  Step4.5   i向右移动1格，j向左移动1格
Step5. 重复 Step2 和 Step3 , 直到 i 与 j 相遇或者交错(即 i 跑到了 j 的右侧) , 然后根据实际情况，选择需要与  中枢元素 进行交换的元素的位置

         Condition-1 : 如果是相遇，那么比较相遇索引所在的数字与 中枢元素的大小关系，
                       1-1 : 比中枢元素小的，则交换相遇索引位置处的数 与 中枢元素的位置，  中枢元素此时，才是真正位于  中枢的位置处，而不是数组的首部
                       1-2 : 否则，比中枢元素大或者相等的的，那么相遇索引向左回退1格的元素( 此元素 一定 <= 中枢元素  ) , 交换这2个位置的数字 

         Condition-2 : 如果是交错了，那么 j 的位置走到了 i的左侧， j 的位置停在了 比 中枢元素小的那一片区域, 交换 j 与 中枢元素 , 就完成了 中枢元素的归位

下面用^ 符号表示下方的数字就是最终的交换位(最终中枢数所处的位置) , 用 & 符号下方的数字， 表示与 原中枢数交换前后位置变化

Step6. 再对 已经归位的中枢元素的  左侧子数组 ， 右侧子数组进行排序 ( 返回Step1 ，对新的子数组进行新一轮的排序 )

e.g.
------------------------------------------------------------------------------------------------------------------------------------------------------
    Case 1:
                                   i        j
                         原: 6     9  8  1  2
        Step1:               6 被选为中枢元素
        Step2:            9 > 6 ,  i 停住
        Step3:            2 < 6 ,  j 停住
        Step4:       交换9<-->2 ,  2  8  1  9
          Step4.5         i向右移动1格， j向左移动1格
        ReDo-Step2:       8 > 6 ,  i 停住
        ReDo-Step3:       1 < 6 ,  j 停住
        ReDo-Step4:  交换8<-->1 ,  2  1  8  9
          Step4.5         i向右移动1格， j向左移动1格 , 此时 i,j 已经交错了，因为 原本位置处于 左侧的游标i 已经移动到了 原本右侧的游标j    右侧了,  满足条件 : Condition-2
                          同时，你也可以发现 (2 1)      (8 9) 的中间如果插入 Step1中的 6 ,那么就完成了预期的目标了， 使左侧的数组中所有的数都小于  中枢元素， 而右侧的数组中所有的数都 大于 中枢元素

                        i j
                  6   2 1 8 9
                        
                  &     ^
                        j i
                  6   2 1 8 9

                   &      ^
                  (2 1)  [6]    (8 9)
        Step6 ,对 (2 1)    与 (8 9) 按上述流程的方式, 进行排序

------------------------------------------------------------------------------------------------------------------------------------------------------
    Case 2:
                                   i           j
                         原: 6     9  8  7  1  2
        Step1:               6 被选为中枢元素
        Step2:            9 > 6 ,  i 停住
        Step3:            2 < 6 ,  j 停住
        Step4:       交换9<-->2 ,  2  8  7  1  9
          Step4.5         i向右移动1格， j向左移动1格
        ReDo-Step2:       8 > 6 ,  i 停住
        ReDo-Step3:       1 < 6 ,  j 停住
        ReDo-Step4:  交换8<-->1 ,  2  1  7  8  9
          Step4.5         i向右移动1格， j向左移动1格 , 此时 i,j 已经碰头, 都指向了这个数 , 又因为 7 > 6 , 因此 ，不应该让7与6进行位置的交换，而应该让7前面的那个数 1 与 6进行位置交换
                          
                              j
                              i                   
                   6    2  1  7  8  9

                   &       ^
                   6    2  1  7  8  9

                   &      ^
                  (1 2)  [6]  (7 8 9)
        Step6 ,对 (2 1)    与 (7 8 9) 按上述流程的方式, 进行排序

------------------------------------------------------------------------------------------------------------------------------------------------------
    Case 3:
                                   i           j
                         原: 6     9  8  5  1  2
        Step1:               6 被选为中枢元素
        Step2:            9 > 6 ,  i 停住
        Step3:            2 < 6 ,  j 停住
        Step4:       交换9<-->2 ,  2  8  5  1  9
          Step4.5         i向右移动1格， j向左移动1格
        ReDo-Step2:       8 > 6 ,  i 停住
        ReDo-Step3:       1 < 6 ,  j 停住
        ReDo-Step4:  交换8<-->1 ,  2  1  5  8  9
          Step4.5         i向右移动1格， j向左移动1格 , 此时 i,j 已经碰头, 都指向了这个数 , 又因为 5 < 6 , 因此 ，应该让5与6进行位置的交换，就能使 中枢元素6进行归位了

                            j
                            i
                6     2  1  5  8  9
                                
                &           ^
                6     2  1  5  8  9
                   &        ^
                  (5 2 1)  [6]  (8 9)
        Step6 ,对 (5 2 1)  与   (8 9) 按上述流程的方式, 进行排序
------------------------------------------------------------------------------------------------------------------------------------------------------

    Case 4 ~ N ... 

        当然了，在实际的排序过程中，可能会碰到一些排序是很奇怪的数组 , 无法同时找到   对 大于中枢数与小于中枢数

                                    i        j  
        Special Case-1 : 原: 6     10  9  8  7   
                         Step2 : 找到了比6大的10 
                         Step3 : 一直无法找到比6小的数， j一直往左移动，i与j 在   10这个数的位置处 , 碰头
                         因为 10 > 6, 因此，应该让10前面的那个数，与中枢数6进行位置交换，但很明显，10前面的那个数，就是中枢数6本身，因此，无需做任何的交换动作

                                     这个例子也很明显，因为原始的数组 ，就已经天生地满足将比6大的数字都放在了6的右侧
                                     但是 , 有一个十分重要的事情，需要你去注意: 虽然没有对这个数组，没有做任何2个数字的交换，但是通过遍历整个原中枢元素右侧的数组后，新的 中枢元素的位置必须明确，下面用^ 符号表示下方的数字就是归位后的中枢数 ， 它就在指向原数组的首部,6这个数字所在的位置
                                     因为，需要针对6左侧的数组(此处为空数组) ，和6右侧的数组进行再次排序(因为，虽然这些数字都>6, 但是他们本身并没有完成排序, 这一次排序后如下图

                                       j
                                       i
                                 6     10  9  8  7   

                                 &
                                 ^     
                                 6     10  9  8  7   


                                 &
                                 ^
                       (空)     [6]  (10 9 8 7)
                         

                                   i     j
        Special Case-2 : 原: 6     1 2 3 4
                         Step2 : 没有找到比6大的数字,一直向前步进,直到碰到了j所在的位置4 (j还没有向左移动的机会，就被i给撞上了)
                         因为 4 < 6, 因此，4和6进行位置交换, 这一次排序后如下图

                                   j
                                   i
                        6    1 2 3 4

                         &           ^
                         6    1 2 3  4

                         &          ^
                        (4 1 2 3)  [6]


        Special Case-3 : 原: 6     7 6 6 6
                         Step2 : 找到比6大的数字7 , i 停止
                         Step3 : 没有找到比6小的数字 , 但 j 与 i 在第2个数字7处，碰头
                         因为 7 > 6 , 因此需要找 7前面的那个数字 6, 与原数组首部的中枢数6进行交换，但2者位置重叠了，但不作交换

                                    j
                                    i
                              6     7 6 6 6

                              &
                              ^
                              6     7 6 6 6

                              &
                              ^    依然能保证 中枢元素右侧的 子数组都 >=6
                             [6]   (7 6 6 6)


        Special Case-4 : 原: 6     4 6 6 6
                         Step2 : 从左往右，没有找到比6大的数字, 一直向前步进,直到碰到了j所在的位置6 (j还没有向左移动的机会，就被i给撞上了)
                         因为 尾部的6 不比 中枢数6 小， 因此找 他前面的那个6，进行位置交换
                         
                                     j
                                     i
                         6     4 6 6 6
                                     
                         &          ^  
                         6     4 6  6  6


                         &          ^
                         6     4 6 [6] 6

                        (6   4  6)  [6] (6)



        Special Case-5 : 原: 6     6 6 6 6

                这是一个最[不]需要排序的数组，因为无论你如果计算，确定交换位置的双方，无认你如果交换哪2个数， 都还是那个数组

                         Step2 : 从左往右，没有找到比6大的数字, 一直向前步进,直到碰到了j所在的位置6 (j还没有向左移动的机会，就被i给撞上了)
                         因为 尾部的6 不比 中枢数6 小， 因此找 他前面的那个6，进行位置交换

                                     j
                                     i
                         6     6 6 6 6
                                     
                         &          ^  
                         6     6 6  6  6


                         &          ^
                         6     6 6 [6] 6

                        (6   6  6)  [6] (6)


------------------------------------------------------------------------------------------------------------------------------------------------------
下面来解答一下 QQQ-1 ，为什么不取数组中的中间元素作为 中枢元素呢？

    1. 为了交换能变得简单
    2. 为了不通过额外的随机算法，取到某个数组中 中间任意随机位置的 中枢元素
    3. 以上的算法，对于位置在 中间区域的中枢元素而言，不一定都正确

e.g-1
          i
        1 8   [5] 7 9
          j

    从左向右，找到了 8 > 5
    从右向左，在 j > i 的前提下，没有找到 < 5 的数字， 再向左步进1格， i == j
    中枢数的归位处 : 应该与定格在 8 处(即5<-->8 ), 进行交换 , 
    但是上述的算法则是要求 与8向左1格的1进行   
    错误的数组 [5] (8 1 7 9) 是一个[不]满足条件的 中枢数排序数组
    而正确的   (1) [5] (8,7,9)


                 
e.g-2
       i           
       9 [5] 7 8 10
       j
        
    从左向右，找到了 9 > 5
    从右到左，在 j > i 的前提下，没有找到 < 5 的数字， 再向左步进1格， i == j
    那么中框元素的归位处，应该与 定格在 9 处的(即 5 <--> 9 ) , 进行交换
    但是上述的算法则是要求 与9向左1格的1进行   (9已经是第1个数,无法再向左1格了)
    错误的数组 因为 9 前面没有数字了，因此无法确定
    而正确的   [5] (9 7 8 10)
        
e.g-3

     i
     9 1 [5] 6 7 8
       j

     j i
     1 9 [5] 6 7 8
     
     此时，应该是让 9 与 5 进行交换，而不是 1 与 5 进行交换
     Error   : [5] (9   1 6 7 8)
     Collect : (1) [5] (9 6 7 8)  

e.g-4

    i
    9 [5] 1 7 2
              j

            i
    2 [5] 1 7 9
            j

   (2 1) [5] 7 9

从以上的e.g.3 与 e.g.4 ，这两个例子来进行对比看，发现，当i与j相遇时， 
 !!!!! 相遇的位置在 中枢元素的左侧还是右侧 !!!!! ，会直接影响到  如果影响到 元素以何种方式进行 位置的交换



------------------------------------------------------------------------------------------------------------------------------------------------------

    

下面来看具体的代码，并附加了许多详细的注释

*/

void quickSortAry_1(int* pAry, int startIdx, int endIdx,int sz)
{
    // 首先，计算出传入数组的元素的总数， 如果数组中只有1个元素，那么就没有必要进行排序了
    // 应该立即有人会问，那么如果是只有2个元素呢,是不是可以跳过上面那么一长段的复杂的算法过程，直接简单地比较这2个数字, 如果原数组中，大数在小数的前面，那么交换2者的位置，使小数在前，大数在后, 否则，不作任何处理，保持原样
    // 别急，后续会有针对数组只有2个元素的情况下的，有针对性的具体说明
    if ( startIdx == endIdx ) {
        // only 1 number
        return;
    }

    //////////////////////////////////////////////////////////////////////////////////
    //
    // 以下是针对，数组只有2个元素时的代码，但是被注释了，不会在运行期间内被执行, 因为针对2个数，后面的代码能满足排序只有2个数的数组，因此为了不要有太多的冗余代码，这里进行注释
    //
    //////////////////////////////////////////////////////////////////////////////////
    //if ( startIdx+1 == endIdx ) {
    //    // only 2 numbers
    //    if ( pAry[startIdx] > pAry[endIdx] ) {
    //        swap2AryNumbers(pAry,startIdx,endIdx,sz);
    //    }
    //    return;   
    //}


    // 令转入数组的首元素为  中枢元素， 但不能简单粗暴地写成   int povitNum = pAry[0];      因为传入的数组可能是原始数组的子数组
    // e.g.     
    //      首次排序后整个数组被分割成了如右所示的2段      [ 0 ~ 3 ]    #4   [ 5 ~ 10 ] , 那么当你要排序 #4 右侧的数组时，startIdx = 5, endIdx = 10
    //      针对这一数组排序时， 中枢元素就是  pAry[5]  , 而[不]是 pAry[0]
    int povitNum = pAry[startIdx]; // 保存 中枢元素的值 到 povitNum这个变量中
    int i = startIdx + 1;   // 定义从左向右移动的游标，i
    int j = endIdx;         // 定义从右向左的游标 , j
    int sepIdx = 0;         // 定义最终归位时， 中枢元素的游标
    while ( true ) {
        // ------>
        // from head to tail , first the 1st number whose value is no less than povitNum
        //------------------------------------------------------------------------------------------------------------------------------------------------------
        // 这个for 循环表示，在 i < j 的前提下，
        // 如果 在index == i 的位置处的数字 <= povitNum ， 那么， i 游标 向右步进1格
        // 否则， i停止移动, i 指向 从左往右找到某一个比 povitNum 大的数的位置处
        //------------------------------------------------------------------------------------------------------------------------------------------------------
        //      也就是说 : for 循环的终止条件是 !(i < j && pAry[i] <= povitNum)
        // 即   i>=j    ||   pAry[i] > povitNum
        // 再说得精确一些， 可以出现以下2种条件之一
        //   1. i 停在了 == j 的位置处,  即!!!!  没有找到 >= povitNum 的数的索引 !!!!
        //   2. 已经找到了 >= povitNum 的数的索引，而且 i < j
        //------------------------------------------------------------------------------------------------------------------------------------------------------
        //
        // 然后再来看一些特殊的情况:
        //  1.  初次 ( 即没有进行过任何1次的 左大右小 数的交换 ) , 进入 while 循环时， 按从左向右的方向，就没能找到比 povitNum , 直到 i 停在了数组最末位的index的位置处
        //       那么，这样就说明了在 [i , j-1] 中，所有的数字，都是满足 pAry[index] <= povitNum 的条件的
        //             至于 index == j 时 pAry[j] 与 povitNum 的大小关系 , 我们现在就来分析下， 当 i == j 时，第2个for循环，直接不执行，跳出
        //             此时一定满足 后续的 i == j 的条件 : 
        //                 那么, 如果 pAry[j] < povitNum , 则根据上述的条件 : [i , j-1] 中，所有的数字，都是满足 pAry[index] <= povitNum , 所有 [startIdx+1, endIdx] 都 <= povitNum
        //                     ，此时，可以将 j 处的数 与 startIdx 处的数字进行交换: 交换后的数组如右图所示  [ exchangeNumber1 , otherNumber1, otherNumber2, otherNumber3,  ... , otherNumberN  ]          [povitNum]
        //                 否则, pAry[j] >= povitNum , 但无论是 pAry[j] > povitNum , 还是 pAry[j] == povitNum , j左侧的数字，一定 <= povitNum, 因些交换 j-1 与 startIdx 2个位置处的数后，一定为如下的大小排布
        //                       [ exchangeNumber(j-1),  otherNumber1, otherNumber2, otherNumber3, ..., otherNumberN <= povitNum ]     [povitNum]    [ 1 number >= povitNum ]
        // 
        //  2.  已经进行过了 1~n 次的左大右小数的交换了 , 但后续找不到 > povitNum, i 与 j 碰头
        //          那么 第2个for循环，不走，直接跳出 (因为此时 j > i 为false , 因为 i == j ) , 进入了满足 if ( i == j ) 的后续代码块
        //                那么 i 停止向右移动时， 当前的数组的样式如下的可能的情况 : 
        // 
        //                                                                                              j
        //                                                                                              i
        //                    Case1 : 1 time exchange  : povitNum    [exchange-Small-Number1, ... ]   [ ... , exchange-Big-Number1]
        //    第1个方括号中 :  [ ... ] 中的 , ... 表示的数,  都在第1个for 循环中被遍历过了, 而且都<= povitNum , 现在来判断 i == j 时， i处的数字与 povitNum 的大小关系情况
        //          如果: i处数字 < povitNum ,  那么交换这2个数字， povitNum 归位于 i 处，整个传入的数据已经被分割成了  [ pAry[i], ... , ... , <= povitNum] [povitNum]  [ ... nums > povitNum ]
        //          否则: i处的数字>=povitNum , 因此不能让 i 与 povitNum 进行交换, 不然交换的果然，就是pAry[i] 明明 >= povitNum , 却还被交换到了 povitNum 的左侧 :  
        //                但是, ... 处的数字均 <= povitNum (第1个for循环保证了这一点),  那么 i-1处的数字一定 <= povitNum, 交换这2个数， 最终 (i-1) 就是 中枢数的归位处 
        //
        //                                                                                                                                                j
        //                                                                                                                                                i
        //                    Case2 : n times exchange : povitNum    [exchange-Small-Number1, exchange-Small-Number2, ... , exchange-Small-NumberN ]   [ ... , exchange-Small-NumberN, ... , exchange-Big-Number2, exchange-Big-Number1]
        //    可以参考上述的 Case1 的数字大小排布的分析，这里不再详述
        //------------------------------------------------------------------------------------------------------------------------------------------------------
        for ( ; i < j && pAry[i] <= povitNum; ++i  ) ;

        // <------
        // from tail to head , first the 1st number whose value is no greater than povitNum
        // j的从右向左移动，与  上面的以变量i为主体的  for循环 类似
        //------------------------------------------------------------------------------------------------------------------------------------------------------
        //   同样，来看特别情况
        //  1. 初次进入while循环， 经过了遍历(无论i是否已经向右移动过, 因为可能初次， startIdx+1 处，就已经是一个 > povitNum 的数了，因此i无需移动, 或者向右移动了n格后，停在了一个 > povitNum 的索引处
        //        但 j 在向左移动的过程中，却没有找到 < povitNum 的数，但与 i 碰头了。
        //
        //    Case 1 : i 没有作过任何一次移动， j 就与 i 碰头了
        //                                          j   <--
        //                                          i                         
        //                  povitNum   [ Big-Number-to-be-exchange,     ]  [  pAry[j] >= povitNum , ... ,   ... ]
        //         同样，判断 pAry[i] 是否 < povitNum , 从上述的图示来看，i与j 碰头处，是比 povitNum 大的一个数，因此，将中枢元素的归位出向左移动一格，即 与 startIdx 重合 , 因此可以不用再任何的交换，原数组已经被分割成了   [0 element] [povitNum]   [Big-Number-to-be-exchange, otherNumber >= povitNum ]
        //  
        //    Case 2 : i 作了一些移动后，找到了 > povitNum
        //                                                                                                                                      j  <--
        //                                                                                                                                      i
        //                  povitNum   [ number0 <= povitNum , number1 <= povitNum, number2 <= povitNum, ... , numberN <= povitNum , Big-Number-to-be-exchange, ]  [  pAry[j] >= povitNum , ... ,   ... ]
        //         同样，判断 pAry[i] 是否 < povitNum , 从上述的图示来看，i与j 碰头处，是比 povitNum 大的一个数，因此，将中枢元素的归位出向左移动一格的位置处，一定是一个 小于 <= povitNum 的数，这个位置就是 povitNum的归位索引处
        //          [numberN<=povitNum , number0 <= povitNum, ... numberN-1 <=povitNum]     [povitNum]   [Big-Number-to-be-exchange, pAry[j] >= povitNum , ... , ... ]
        //------------------------------------------------------------------------------------------------------------------------------------------------------
        for ( ; j > i && pAry[j] >= povitNum; --j  ) ;

        //------------------------------------------------------------------------------------------------------------------------------------------------------
        // 下面先来看最为特殊的情况之一: 有且仅有2个数的数组的情况下，那么 i 与 j 一定在初次赋值时, 就已经数值相等了
        // 此时 , 上述2处的for循环，在判定 i < j  与  j > i 的情况下就已经停止了
        // 进入了下面的 j == i 的判定 , 而且一定满足 j == i , 开始执行 if 语句的花括号内 的语句
        // 如果在有且仅有2个元素的数组的情况下 : 
        //         后者 比 前者(povitNum) 小的话  ==>  pAry[i] < povitNum  , 则交换这两个数的位置, 且交换后中枢元素已经归位 ,位置在 在i与j相等(碰头处)
        //         否则 :  后者 比 前者  大， 此时中枢数已经归位, 而且中枢元素的位置在  i==j 的左侧1格，即 swapIdx = i - 1 ,同时在有且仅有2个元素的时候下  swapIdx == startIdx ; 不需要进行位置的交换 
        //         同时，退出 外侧的大的 while 循环
        if ( j == i ) {
            if ( pAry[i] < povitNum ) {
                swap2AryNumbers(pAry,startIdx,i,sz);
                sepIdx = i;
            } else {
                // pAry[i] >= povitNum
                int swapIdx = i - 1;
                if ( swapIdx != startIdx )  {
                    swap2AryNumbers(pAry,startIdx,swapIdx,sz);
                }
                sepIdx = swapIdx;
            }
            break;
        }

        // 否则 , 在 i 与j 还没有碰头时， 交换2个数的位置，这里不用担心，i 与 j 是否相等，即指向同一个元素，对同一位置的元素，进行位置交换是没有意义的, 当然，即使进行了交换后，也无妨, 因为不起作用 ， 如对 i == 3 , 与 j == 3 的2个数进行交换，则交换前后，不影响数组的大小排布
        // swap
        swap2AryNumbers(pAry,i,j,sz);
        // 这里的步进是必须的。因为经过了某一次交换后, 需要进行后续的流程，找出下一组的  大数小数(当然，可能找到，也可能没有找到，也可能只找到大数，没有找到小数)，然后进行位置交换
        //  因为假定一种最为特殊的情况，   5,   5,5,5, 5,5,5
        // 如果有人写前2个for循环时  ，都不加 等于的情况下 ，
        //           原代码 : pAry[i] <= povitNum ,   pAry[j] >= povitNum
        //           现代码 : 
        //  for ( ; i < j &&  pAry[i] < povitNum; ++i ) ;
        //  for ( ; j > i &&  pAry[j] > povitNum; ++j ) ;
        // 那么， i,j 就永远停在原地，while就是死循环
        ++i;
        --j;

        // 另外，这里有一个核心需要判定的地方，就是 i 与 j 是否交错了，一定是跳出while的时机
        // 但不用担心 ++i; --j; 后，i与j相等的情况，因为如果相等，下一次的while循环中，2个for循环都不执行，直接跳过，进行 i==j 的判定逻辑代码块中
        if ( i > j ) {
            // 因为在上面的代码中，已经进行了位置的交换, 保证了左小右大的情况，那么 ++i; --j; 错位后， j 所在的就是交换之前i所在的位置，交换后i处的数字一定 < povitNum
            // 因此: 此时的j 就是 , 中枢数的归位索引处, 无须判定 j 是否 与 startIdx 相等 ,    
            // if ( j == startIdx ) { swap2AryNumbers(pAry,startIdx,j,sz); } 多此一举, 也不可能 出现 j == startIdx 的情况
            swap2AryNumbers(pAry,startIdx,j,sz);
            sepIdx = j;
            break;
        }
    }

    // Sort <Left> part , Range : [startIdx,sepIdx-1]
    if ( sepIdx-1 > startIdx ) {
        quickSortAry_1(pAry, startIdx, sepIdx-1,sz);
    }

    // Sort <Right> part , Range : [sepIdx+1, endIdx]
    if ( sepIdx+1 < endIdx ) {
        quickSortAry_1(pAry, sepIdx+1, endIdx,sz);
    }
}

void quickSortAry_2(int* pAry, int startIdx, int endIdx,int sz)
{
    if ( startIdx == endIdx ) {
        // only 1 number , do need to sort
        return;
    }

    // if ( startIdx+1 == endIdx ) {
    //     // only 2 numbers , just swap numbers if neccessary
    //     if ( pAry[startIdx] > pAry[endIdx] ) {
    //         swap2AryNumbers(pAry, startIdx, endIdx, sz);
    //     }
    //     return;
    // }

    int povitNum = pAry[startIdx];
    int i = startIdx+1;
    int j = endIdx;
    int sepIdx = 0;
    while ( true ) 
    {
        // <------
        // from tail to head , first the 1st number whose value is less than povitNum
        for ( ; j > i && pAry[j] >= povitNum; --j ) ;

        // ------>
        // from head to tail , first the 1st number whose value is less than povitNum
        for ( ; i < j && pAry[i] <= povitNum; ++i ) ;

        if ( i == j ) {
            if ( pAry[i] < povitNum ) {
                swap2AryNumbers(pAry, startIdx,i,sz);
                sepIdx = i;
            } else {
                // pAry[i] >= povitNum
                int swapIdx = i - 1;
                if ( swapIdx != startIdx ) {
                    swap2AryNumbers(pAry, startIdx,swapIdx,sz);
                }
                sepIdx = swapIdx;
            }
            break;
        }

        swap2AryNumbers(pAry, i,j, sz);
        ++i;
        --j;
        if ( i > j ) {
            if ( j != startIdx ) {
                swap2AryNumbers(pAry, j,startIdx, sz);
            }
            sepIdx = j;
            break;
        }
    }

    // Sort New <Left> Part , Range : [startIdx,sepIdx-1]
    if ( sepIdx-1 > startIdx ) {
        quickSortAry_2(pAry,startIdx, sepIdx-1, sz);
    }

    // Sort New <Right> Part , Range : [sepIdx+1,endIdx]
    if ( sepIdx+1 < endIdx ) {
        quickSortAry_2(pAry,sepIdx+1, endIdx, sz);
    }
}

void insertSort(int* pAry, int sz)
{
    for ( int i = 1; i < sz; ++i ) {
        int targetNum = pAry[i];
        int j = i - 1;
        for ( ; j>=0 && targetNum < pAry[j]; --j ) {
            pAry[j+1] = pAry[j];
        }
        pAry[j] = targetNum;
    }
}


void quickSortAry_3(int* pAry, int startIdx, int endIdx,int sz)
{
    if ( startIdx == endIdx ) {
        // only 1 number
        return;
    }



    int povitNum = pAry[startIdx]; // 保存 中枢元素的值 到 povitNum这个变量中
    int i = startIdx + 1;   // 定义从左向右移动的游标，i
    int j = endIdx;         // 定义从右向左的游标 , j
    int sepIdx = 0;         // 定义最终归位时， 中枢元素的游标
    while ( true ) {
        for ( ; i < j && pAry[i] >= povitNum; ++i  ) ;

        for ( ; j > i && pAry[j] <= povitNum; --j  ) ;

        if ( j == i ) {
            if ( pAry[i] > povitNum ) {
                swap2AryNumbers(pAry,startIdx,i,sz);
                sepIdx = i;
            } else {
                // pAry[i] >= povitNum
                int swapIdx = i - 1;
                if ( swapIdx != startIdx )  {
                    swap2AryNumbers(pAry,startIdx,swapIdx,sz);
                }
                sepIdx = swapIdx;
            }
            break;
        }

        // 否则 , 在 i 与j 还没有碰头时， 交换2个数的位置，这里不用担心，i 与 j 是否相等，即指向同一个元素，对同一位置的元素，进行位置交换是没有意义的, 当然，即使进行了交换后，也无妨, 因为不起作用 ， 如对 i == 3 , 与 j == 3 的2个数进行交换，则交换前后，不影响数组的大小排布
        // swap
        swap2AryNumbers(pAry,i,j,sz);
        ++i;
        --j;

        if ( i > j ) {
            swap2AryNumbers(pAry,startIdx,j,sz);
            sepIdx = j;
            break;
        }
    }

    // Sort <Left> part , Range : [startIdx,sepIdx-1]
    if ( sepIdx-1 > startIdx ) {
        quickSortAry_3(pAry, startIdx, sepIdx-1,sz);
    }

    // Sort <Right> part , Range : [sepIdx+1, endIdx]
    if ( sepIdx+1 < endIdx ) {
        quickSortAry_3(pAry, sepIdx+1, endIdx,sz);
    }
}



int test1(int flag)
{
    const int ARY_SIZE = 100;

    ifstream f("num100.txt", ios::in);
    if ( !f ) {
        cout << "Can't open num100.txt , Exit the program" << endl;
        return -1;
    }

    int cnt = 0;
    int* pAry = new int[ARY_SIZE];
    while( !f.eof() ) {
        int num = 0;
        f >> num;
        // cout << cnt << ".  " << num << endl;
        if ( num != 0 ) {
            if ( cnt < ARY_SIZE ) {
                pAry[cnt++] = num;
            } 
        } 
    }
    f.close();

    // random   right part's number maybe  >=81  or   <=81 ,   Number Array's size = 100  : [1 ~ 100]
    // int testAry1[ARY_SIZE] = { 81, 24, 15, 31, 61, 77, 34, 9, 36, 62, 30, 75, 98, 50, 45, 35, 19, 23, 83, 38, 11, 74, 63, 44, 14, 1, 22, 47, 70, 86, 88, 52, 2, 94, 39, 71, 57, 69, 40, 87, 79, 80, 32, 18, 46, 67, 95, 10, 41, 5, 76, 97, 85, 78, 27, 25, 21, 6, 54, 72, 20, 51, 33, 17, 92, 56, 12, 37, 73, 3, 60, 89, 66, 26, 8, 53, 49, 59, 64, 13, 28, 48, 96, 93, 16, 91, 58, 68, 43, 42, 7, 4, 65, 82, 29, 99, 100, 55, 90, 84 };

    // Special Condition 1 : right parts > 81
    // int testAry2[ARY_SIZE] = { 81, 16807, 282475249, 1622650073, 984943658, 1144108930, 470211272, 101027544, 1457850878, 1458777923, 2007237709, 823564440, 1115438165, 1784484492, 74243042, 114807987, 1137522503, 1441282327, 16531729, 823378840, 143542612, 896544303, 1474833169, 1264817709, 1998097157, 1817129560, 1131570933, 197493099, 1404280278, 893351816, 1505795335, 1954899097, 1636807826, 563613512, 101929267, 1580723810, 704877633, 1358580979, 1624379149, 2128236579, 784558821, 530511967, 2110010672, 1551901393, 1617819336, 1399125485, 156091745, 1356425228, 1899894091, 585640194, 937186357, 1646035001, 1025921153, 510616708, 590357944, 771515668, 357571490, 1044788124, 1927702196, 1952509530, 130060903, 1942727722, 1083454666, 1108728549, 685118024, 2118797801, 1060806853, 571540977, 194847408, 2035308228, 158374933, 1075260298, 824938981, 595028635, 1962408013, 1137623865, 997389814, 2020739063, 107554536, 1635339425, 1654001669, 1777724115, 269220094, 34075629, 1478446501, 1864546517, 1351934195, 1581030105, 1557810404, 2146319451, 1908194298, 500782188, 657821123, 753799505, 1102246882, 1269406752, 1816731566, 884936716, 1807130337, 578354438 };

    // Special Condition 2 : right parts < 81
    // int testAry3[ARY_SIZE] = { 81, -40, -61, -77, 20, 49, 35, 51, 80, -80, -37, 18, -44, 60, 62, -45, -56, -79, -34, 70, 1, -15, -73, -36, -11, 22, -42, -33, 45, 62, -70, -61, 2, 41, -1, 62, -55, -46, -54, -30, -37, 38, -37, 18, -11, -47, 38, -49, 69, -76, -62, -20, 79, 55, 64, 68, 66, 44, 75, -13, 70, 43, -63, 17, -44, -73, -36, 45, 6, -7, 40, -70, -43, -34, -6, 40, -68, 63, -50, -56, -24, 70, -63, -76, -65, -50, -15, 17, 24, 12, -35, -59, 16, 24, 62, 52, -6, 20, 63, -21 };

    // Special Condition 3 : all value are <Same>
    // int testAry4[ARY_SIZE] = { 81, 81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81,  81 }; 

    //for ( int i = 0; i < ARY_SIZE; ++i ) {
    //    pAry[i] = testAry1[i];
    //    pAry[i] = testAry2[i];
    //    pAry[i] = testAry3[i];
    //    pAry[i] = testAry4[i];
    //}

    if ( cnt != ARY_SIZE ) {
        cout << "[ERROR] : Collect less than 100 numbers ! Exit the program " << endl;
        return -1;
    }

    if ( flag == 1 ) {
        cout << "Use Quick Sort 1" << endl;
        quickSortAry_1(pAry,0, ARY_SIZE-1, ARY_SIZE);
    } else {
        // cout << "Use Quick Sort 2 : Insert Sort" << endl;
        // insertSort(pAry, ARY_SIZE);
        cout << "Use Quick Sort 3" << endl;
        quickSortAry_3(pAry,0, ARY_SIZE-1, ARY_SIZE);
    }
    
    // 排序准则 :   升序排序
    auto isAscendingOrder = true;
    auto meetError = false;
    // int num = 1;
    int errorIdx = 0;
    for ( int i = 0; i < ARY_SIZE-1; ++i ) {
        int j = i + 1;
        if ( isAscendingOrder ) {
            // 升序排序
            if ( pAry[i] > pAry[j] ) {
                meetError = true;
                errorIdx = i;
                break;
            }
        } else {
            // 降序排序
            if ( pAry[i] < pAry[j] ) {
                meetError = true;
                errorIdx = i;
                break;
            }
        }
    }

    if ( meetError ) {
        cout << "[ERROR] : Sort <Not> OK, errorIdx = [" << errorIdx << "] = " << pAry[errorIdx] << endl;
        cout << "idx => ";
        for ( int i = 0; i < ARY_SIZE; ++i ) {
            cout << setw(3) << i << " ";
        }
        cout << endl;
        cout << "ary => ";
        for ( int i = 0; i < ARY_SIZE; ++i ) {
            cout << setw(3) << pAry[i]  << " ";
        }
        cout << endl;
    } else {
        cout << "Sort OK" << endl;
    }

    if ( pAry != nullptr ) {
        delete [] pAry;
        pAry = nullptr;
    }

    return 0;
}



int main(int argc,char* argv[])
{
    if ( argc == 1 ) {
        cout << "Missing args" << endl;
        return -1;
    }

    int tag = atoi(argv[1]);
    test1(tag);
    return 0;
}


