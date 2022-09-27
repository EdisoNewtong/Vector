#include <iostream>
#include <iomanip>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <iterator>

#include "NonModifying.h"
using namespace std;


static const char G_TAB = '\t';


namespace NonModifying
{
    
void testCaseAll()
{
    cout << "######################### NonModifying Algorithm Begin #########################" << endl;
    countOrCount_ifCase();
    minOrMaxOrMinMax();
    findOrFind_ifOrFind_if_notCase();
    search_nCase();
    searchCase();
    find_endCase();
    find_first_ofCase();
    adjacent_findCase();
    equalCase();
    is_permutationCase();
    mismatchCase();
    sortedCase();
    is_partitionedCase();
    all_any_noneCase();
    cout << "######################### NonModifying Algorithm End   #########################" << endl;
}


void countOrCount_ifCase()
{
    cout << G_TAB << "--------------- count(...) / count_if(...) --------------- " << endl;
    vector<int> v;
    // init
    for( int i = 1; i <=9; ++i ) {
        v.push_back(i);
    }

    cout << G_TAB << "Original vector                    : [ ";
    // print original
    for( const auto& e : v ) {
        cout << e << " ";
    }
    cout << " ] " << endl;

    int num = 0;
    num = count(v.cbegin(),  v.cend(), 4);
    cout << G_TAB << "number of elements equal to 4      : " << num << endl;

    //
    // count_if(InputIterator beg, InputIterator end, UnaryPredicate op);
    //   "op" should not modify the passed arguments
    num = count_if(v.cbegin(),  v.cend(), 
                   [](int ele) {
                        return ele % 2 == 0;
                   });
    cout << G_TAB << "number of elements with even value : " << num << endl;


    num = count_if(v.cbegin(),  v.cend(), 
                   [](int ele) {
                        return ele > 4;
                   });
    cout << G_TAB << "number of elements greater than 4  : " << num << endl;
    cout << G_TAB << "------------------------------------------------------------" << endl << endl;

}


void minOrMaxOrMinMax()
{
    cout << G_TAB << "--------------- min(...) / max(...) / minmax(...) --------------- " << endl;

    //
    // min_element(ForwardIterator beg, ForwardIterator end, CompFunc op);
    //
    auto absLess = [](int ele1, int ele2) -> bool {
        return abs(ele1) < abs(ele2);
    };

    deque<int> v;
    for( int n =  2; n <=6; ++n ) { v.push_back(n); }
    for( int n = -3; n <=6; ++n ) { v.push_back(n); }

    cout << G_TAB << "Original Deque                    : [ ";
    // print original
    for( const auto& e : v ) {
        cout << e << " ";
    }
    cout << " ] " << endl;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Core Core Core : 
    //     You should guarantee that the container is not empty 
    // Otherwise the min_element( ... ) will return v.end() as the result
    
    //
    // find minimum element
    //
    auto itMin = min_element( v.cbegin(), v.cend() );
    if ( itMin != v.end() ) {
        cout << G_TAB << "min = " << *itMin << endl;
    } else {
        cout << G_TAB << "min = <NOT FOUND>" << endl;
    }

    //
    // find maximum element
    //
    auto itMax = max_element( v.cbegin(), v.cend() );
    if ( itMax != v.end() ) {
        cout << G_TAB << "max = " << *itMax << endl;
    } else {
        cout << G_TAB << "max = <NOT FOUND>" << endl;
    }

    //
    // find Both <Min> And <Max> elements in one function named   minmax_element(...)
    // It yield a pair of iterator  as the result type
    auto minMaxPair = minmax_element( v.cbegin(), v.cend() );
    if ( minMaxPair.first != v.end() ) {
        cout << G_TAB << "min = " << *(minMaxPair.first) << endl;
    } else {
        cout << G_TAB << "min = <NOT FOUND>" << endl;
    }
    
    if ( minMaxPair.second != v.end() ) {
        cout << G_TAB << "max = " << *(minMaxPair.second) << endl;
    } else {
        cout << G_TAB << "max = <NOT FOUND>" << endl;
    }

    itMin = min_element( v.cbegin(), v.cend(), absLess);
    if ( itMin != v.end() ) {
        cout << G_TAB << "minimum of absolute value = " << *itMin << endl;
    } else {
        cout << G_TAB << "minimum of absolute value = <NOT FOUND>" << endl;
    }

    itMax = max_element( v.cbegin(), v.cend(), absLess);
    if ( itMax != v.end() ) {
        cout << G_TAB << "maximum of absolute value = " << *itMax << endl;
    } else {
        cout << G_TAB << "maximum of absolute value = <NOT FOUND>" << endl;
    }

    /*
    cout << G_TAB << "Original deque                    : [ ";
    // print original
    for( const auto& e : v ) {
        cout << e << " ";
    }
    cout << " ] " << endl;
    */

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}


void findOrFind_ifOrFind_if_notCase()
{
    cout << G_TAB << "--------------- find(...) / find_if(...) / find_if_not(...) --------------- " << endl;
    list<int> v;
    for( int n =  1; n <=9; ++n ) { v.push_back(n); }
    for( int n =  1; n <=9; ++n ) { v.push_back(n); }

    cout << G_TAB << "Original list                    : [ ";
    // print original
    for( const auto& e : v ) {
        cout << e << " ";
    }
    cout << " ] " << endl;

    list<int>::iterator pos1;
    // find the 1st element whose value equals to 4
    pos1 = find( v.begin(), v.end(), 4);

    list<int>::iterator pos2 = v.end();
    if ( pos1 != v.end() ) {
        list<int>::iterator pos3 = pos1;
        pos2 = find(++pos3, v.end(), 4);
    }

    if ( pos1 != v.end()   &&   pos2 != v.end()  ) {
        cout << G_TAB << "After find(...) list             : [ ";
        copy(pos1, ++pos2, ostream_iterator<int>(cout, " ") );
        cout << " ] " << endl;
    }


    cout << G_TAB << "********************************************************************************" << endl;
    v.clear();
    for( int n =  1; n <=9; ++n ) { v.push_back(n); }
    cout << G_TAB << "Original list                    : [ ";
    // print original
    for( const auto& e : v ) {
        cout << e << " ";
    }
    cout << " ] " << endl;
    //                                  find the 1st element       element > 3
    //                                                       [](int ele) { return ele > 3; }
    pos1 = find_if( v.begin(), v.end(), bind(greater<int>(), std::placeholders::_1,  3) );
    if ( pos1 != v.end() ) {
        cout << G_TAB << "The " << distance(v.begin(), pos1) + 1 << "th. element is the first greater than 3 => " << *pos1 <<  endl;
    }
    
    //                                  find the 1st element divisible by 3      
    pos1 = find_if( v.begin(), v.end(), 
                   [](int ele) {
                        return  ele % 3 == 0;
                   });
    if ( pos1 != v.end() ) {
        cout << G_TAB << "The " << distance(v.begin(), pos1) + 1 << "th. element is the first divisible by 3 => " << *pos1 << endl;
    }


    //                                      ! ( find the 1st element < 5 )
    //                                                       [](int ele) { return ele < 5; }
    pos1 = find_if_not( v.begin(), v.end(), bind( less<int>(), std::placeholders::_1, 5) );
    
    /*

       Core Core Core : 

       ****************************************************************************************************
         The first Predicate whose return value is   <false>   ( Not <true> )
       ****************************************************************************************************


    pos1 = find_if_not( v.begin(), v.end(), [](int ele) -> bool {
        static int G_CNT = 0;
        bool b = ele < 5;
        cout << ++G_CNT << ". ele = " << ele << " , b = " << (b ? "true" : "false") << endl;
        return b;
    } );
    */
    if ( pos1 != v.end() ) {
        cout << G_TAB << "The first value >= 5  => " << *pos1 << endl;
    }

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}

void search_nCase()
{
    //                            连续的，不间断的
    //  Search First n  Matching      Consecutive        Elements
    //
    // search_n(ForwardIterator beg, ForwardIterator end, Size count, const T& value);
    // 
    //                                                                             // the 4th arguments
    //                                                                    [](int ele, int value) -> { return ; }
    // search_n(ForwardIterator beg, ForwardIterator end, Size count, const T& value, BinaryPredicate op);
    //
    
    cout << G_TAB << "--------------- search_n(...) / search_n_if(...) --------------- " << endl;

    //            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,  10, 11,  12
    deque<int> v{ 1, 2, 7, 7, 6, 3, 9, 5, 7, 7,   7,  3,   6  };
    cout << G_TAB << "Original index                    :-> ";
    for( int i = 0; i < static_cast<int>(v.size()); ++i ) {
        cout << setw(2) << (i+1) << " ";
    }
    cout << " <- " << endl;
    cout << G_TAB << "Original deque                    : [ ";
    // print original
    for( const auto& e : v ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    deque<int>::iterator pos;

    //
    // 寻找连续3个7, 如果存在，返回第1个7 所在的位置
    //                                count = 3
    //                                value = 7
    pos = search_n(v.begin(), v.end(), 3, 7);
    if ( pos != v.end() ) {
        cout << G_TAB << "3 consecutive elements with value 7 "
             << "start with the " << (distance(v.begin(), pos) + 1) << "th element "
             << endl;
    } else {
        cout << G_TAB << "<No> 3 consecutive elements with value 7 " << endl;
    }

    // 
    //
    // 寻找连续2个元素 ,   他们的值 都是3的整数倍 
    //                                count = 2
    //                                value = 3
    pos = search_n(v.begin(), v.end(),
                   2,   // count
                   3,   // value
                   [](int ele, int val) {
                        // value = 3 ( the 4th arguments )
                        //     (ele % 3 == 0)
                        return ele % val == 0;
                   });


    if ( pos != v.end() ) {
        deque<int>::iterator posBack = pos;
        // [ 6, 3 ]
        cout << G_TAB << "2 consecutive (elements % 3 == 0)   =>  [ ";
        for( int i = 0; i < 2; ++i ) {
            cout << *pos << " ";
            advance(pos,1);
        }
        cout << "] " << endl;
        pos = posBack;
    } else {
        cout << G_TAB << "<No> 2 consecutive (elements % 3 == 0) ." << endl;
    }


    // 
    //
    // 寻找连续4个 奇数  
    pos = search_n(v.begin(), v.end(),
                   4,   // count
                   0,   // value
                   [](int ele, int val) {
                        (void)val; // unused argument  "val"
                        return ele % 2 == 1; // odd number
                   });


    if ( pos != v.end() ) {
        cout << G_TAB << "4 consecutive odd number   =>  [ ";
        for( int i = 0; i < 4; ++i, ++pos ) {
            cout << *pos << " ";
        }
        cout << "] " << endl;
    } else {
        cout << G_TAB << "<No> 4 consecutive odd number.  " << endl;
    }

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
    
}


void searchCase()
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // ForwardIterator   search(ForwardIterator1 mainSet_Beg, ForwardIterator1  mainSet_End,
    //                          ForwardIterator2  subSet_Beg, ForwardIterator2   subSet_End, 
    //                          BinaryPredicate op);
    cout << G_TAB << "--------------- search(...)  --------------- " << endl;

    deque<int> v1{ 1, 2, 3, 4, 5, 6, 7,   1, 2, 3, 4, 5, 6, 7 };
    list<int>  v2{       3, 4, 5, 6                           };

    cout << G_TAB << "Original index                    :-> ";
    for( int i = 0; i < static_cast<int>(v1.size()); ++i ) {
        cout << setw(2) << (i+1) << " ";
    }
    cout << " <- " << endl;
    cout << G_TAB << "Original deque                    : [ ";
    // print original deque
    for( const auto& e : v1 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;


    cout << G_TAB << "Original list                     : [       ";
    // print original list
    for( const auto& e : v2 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Core Core Core :
    //
    //       v1,v2 都是STL容器
    //       如果 v2 是 v1 的子集， 查找 v2 在 v1 的哪个位置 ?
    deque<int>::iterator pos = v1.begin();
                     //  big main search     range
    while ( ( pos = search( pos, v1.end(),                 
                     //  small            subrange
                      v2.begin(), v2.end() ) ) != v1.end()  
    )
    {
        cout << G_TAB
             << "v2 is the subset of v1 , Found the starting element at the <"
             << ( distance(v1.begin(), pos) + 1 ) << ">th element."
             << endl;
        ++pos;
    }


    cout << G_TAB << "********************************************************************************" << endl;
    vector<int> v3{ 1, 2, 3, 4, 5, 6, 7, 8 , 9 };
    cout << G_TAB << "Original vector                    : [ ";
    // print original vector
    for( const auto& e : v3 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    auto criterion = [](int ele, bool evenFlag) -> bool {
        if ( evenFlag ) {
            return ele % 2 == 0;
        } else {
            return ele % 2 == 1;
        }
    };

    bool checkEvenArgs[3] = { true, false, true };
    vector<int>::iterator pos2 = v3.begin();
                     //     big main search     range
    while ( (pos2 = search( pos2, v3.end(),
                     //      small            subrange
                           checkEvenArgs, checkEvenArgs+3,
                           criterion) ) != v3.end() 
    )
    {
        vector<int>::iterator pos2Backup = pos2;
        cout << G_TAB << "subrange match with 3 elements [ even odd even ]    : [ ";
        for( int i = 0; i < 3; ++i, ++pos2) {
            cout << *pos2 << " ";
        }
        cout << " ] " << endl;
        pos2 = ++pos2Backup;
    }

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}


void find_endCase()
{
    // ForwardIterator1  find_end(ForwardIterator1 beg,    ForwardIterator1 end,
    //                            ForwardIterator2 subBeg, ForwardIterator2 subEnd,
    //                            BinaryPredicate op);  // op(element1, subElement);
    cout << G_TAB << "--------------- find_end(...)  --------------- " << endl;
    deque<int> v1{ 1, 2, 3, 4, 5, 6, 7,   1, 2, 3, 4, 5, 6, 7 };
    list<int>  v2{       3, 4, 5, 6                           };

    // print original deque's   index
    cout << G_TAB << "Original index                    :-> ";
    for( int i = 0; i < static_cast<int>(v1.size()); ++i ) {
        cout << setw(2) << (i+1) << " ";
    }
    cout << " <- " << endl;
    cout << G_TAB << "Original deque                    : [ ";
    // print original deque
    for( const auto& e : v1 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    cout << G_TAB << "Original list                     : [       ";
    // print original list
    for( const auto& e : v2 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     Core Core Core :
    // it return the value of the 2nd arguments if not found the matched range
    /*
         vector<int> va{ 11,22,33,44,55,66 };
         vector<int> vb{ 11,22,33,44,55    };

         auto vaBeg = va.begin();
         auto vaSubEnd = vaBeg + 3; // pointer at  the number 44
         
         auto it = find_end( vaBeg, vaSubEnd,
                             vb.begin(), vb.end() );

         //
         // Core Core Core 
         //
         if ( it == vaSubEnd ) {
            // *it  ->  44
            cout << "[NOT FOUND]  it == vasubend , *it = " << *it << endl;
         }
                   
         if ( it != v1.end() ) {
            cout << "it == vasubend , *it = " << *it << endl;
         }

    */
    deque<int>::iterator pos;
    deque<int>::iterator endpos = v1.end();
                     //         big main search     range
    while ( ( pos = find_end( v1.begin(), endpos,
                     //  small                   subrange
                     v2.begin(), v2.end() ) )             != endpos
    )
    {
        cout << G_TAB
             << "v2 is the subset of v1 , <In Reverse direction> Found the starting element at the <"
             << ( distance(v1.begin(), pos) + 1 ) << ">th element."
             << endl;


        endpos = pos;
    }

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}

void find_first_ofCase()
{
    // Core Core Core : ( 注意，这里[不]需要2个区间的元素对应相等， 而是只是找到2区间中第1个与1区间相等的元素
    // range1 : [ 2, 4, 6, 5  ]
    //                     ^
    // range2 : [ 1, 7, 5, 11 ]
    //                  ^
    // return -> indexOf(range1, 5)     == (range1.begin() + 3);
    // 
    ////////////////////////////////////////////////////////////////////////////
    //     在区间1 内，寻找 区间2 中与区间1, 第一次相等的元素，返回这个元素在区间1中的位置
    //
    // InputIterator  find_first_of(InputIterator beg,    InputIterator   end,
    //                            ForwardIterator subBeg, ForwardIterator subEnd,
    //                            BinaryPredicate op);  // op(element1, subElement);
    cout << G_TAB << "--------------- find_first_of(...)  --------------- " << endl;

    vector<int> v1{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    //                          ^     *
    list<int>   v2{       33, 44, 5,  7                  };
    //                            ^   *

    // print original deque's   index
    cout << G_TAB << "Original index                    :-> ";
    for( int i = 0; i < static_cast<int>(v1.size()); ++i ) {
        cout << setw(2) << (i+1) << " ";
    }
    cout << " <- " << endl;
    cout << G_TAB << "Original vector                   : [ ";
    // print original deque
    for( const auto& e : v1 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    cout << G_TAB << "Original list                     : [       ";
    // print original list
    for( const auto& e : v2 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    vector<int>::iterator pos;
    pos = find_first_of( v1.begin(), v1.end(),
                         v2.begin(), v2.end() );
    cout << G_TAB << "first element of v2 in v1 is at the position : <"
         << ( distance(v1.begin(), pos) + 1 ) << ">th. "
         << endl;

    vector<int>::reverse_iterator rpos;
    rpos = find_first_of( v1.rbegin(), v1.rend(),
                          v2.begin(), v2.end() );
    cout << G_TAB << "last  element of v2 in v1 is at the position : <"
         << ( distance(v1.begin(), rpos.base() )  ) << ">th.   *rpos = " << *rpos
         << endl;

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}


void adjacent_findCase()
{
    // search 2 adjacent elements whose values are identical or match a given criterion 
    // ForwardIterator  adjacent_find(ForwardIterator beg, ForwardIterator end);
    // ForwardIterator  adjacent_find(ForwardIterator beg, ForwardIterator end, BinaryPredicate op);
    cout << G_TAB << "--------------- adjacent_find(...)  --------------- " << endl;
    vector<int> v{ 1, 3, 2, 4, 5, 5, 0 };
    cout << G_TAB << "Original vector                   : [ ";
    // print original deque
    for( const auto& e : v ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    vector<int>::iterator pos;
    pos = adjacent_find( v.begin(), v.end() );
    if ( pos != v.end() ) {
        vector<int>::iterator tmpIt = pos;
        cout << G_TAB << " adjacent number is " << *tmpIt << endl;
        cout << G_TAB << " previous number is " << *(--tmpIt) << endl;
        ++tmpIt; // pointer to pos
        ++tmpIt; // pointer to pos+1
        ++tmpIt; // pointer to pos+2
        cout << G_TAB << " next number is " << *tmpIt << endl;
    }

    cout << endl;
    // search first two elements for which the second has double the value of the first
    auto isDoubled = [](int e1, int e2) -> bool {
        return (e1*2 == e2);
    }; 

    pos = adjacent_find( v.begin(), v.end(), isDoubled);
    if ( pos != v.end() ) {
        cout << G_TAB << " first two elements with second value twice the first have pos <"
             << ( distance(v.begin(), pos) + 1 )
             << ">th .   *pos = " << *pos
             << endl;
    }

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}


void equalCase()
{
    ////////////////////////////////////////////////////////////////////////////
    // 
    // Core Core Core : ( 在使用这个算法之前，必须保证 compBeg 迭代器所在的容量有和 [beg, end) 至少一样多的元素 )
    //
    // Core Core Core :
    // bool equal(InputIterator1 beg, InputIterator1 end,
    //            InputIterator2 compBeg);
    cout << G_TAB << "--------------- equal(...)  --------------- " << endl;
    vector<int> v1{ 1, 2, 3, 4, 5, 6, 7 };
    list<int>   v2{ 3, 4, 5, 6, 7, 8, 9 };

    // print original vector
    cout << G_TAB << "Original vector                   : [ ";
    for( const auto& e : v1 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    // print original list
    cout << G_TAB << "Original list                     : [ ";
    for( const auto& e : v2 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;


    if ( equal(v1.begin(), v1.end(),     v2.begin() ) ) {
        cout << G_TAB << "v1 == v2 " << endl;
    } else {
        cout << G_TAB << "v1 != v2 " << endl;
    }


    if ( equal(v1.begin(), v1.end(),     v2.begin() ) ) {
        cout << G_TAB << "v1 == v2 " << endl;
    } else {
        cout << G_TAB << "v1 != v2 " << endl;
    }

    // cout << endl;

    // check for corresponding even and odd elements
    auto bothEvenOrOdd = [](int e1, int e2) {
        return (e1 % 2) == (e2 % 2);
    };

    if ( equal(v1.begin(), v1.end(),     v2.begin(), bothEvenOrOdd ) ) {
        cout << G_TAB << "even and odd elements correspond." << endl;
    } else {
        cout << G_TAB << "even and odd elements do not correspond." << endl;
    }


    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}

void is_permutationCase()
{

    ////////////////////////////////////////////////////////////////////////////
    // bool is_permutation(ForwardIterator1 beg1, ForwardIterator1 end1, 
    //                     ForwardIterator2 beg2);
    //
    // 2个区间 是否是 同一组排列数 ? 这里[不]要求，这2上区间长度相等的，2区间可以长于1区间
    // e.g.
    //                     
    //      range1 : [     1,2,3,4     ]
    //                     ^     ^
    // 
    //      range2 : [  9, 1,2,3,4, 10 ]
    //                     *     
    cout << G_TAB << "--------------- is_permutation(...)  --------------- " << endl;
    //              1,   1 ~ 9
    vector<int> v1{ 1,   1,  2,  3,  4,  5,  6,  7,  8,  9 };
    //              1,   9 ~ 1
    list<int>   v2{ 1,   9,  8,  7,  6,  5,  4,  3,  2,  1 };  // 6 odd numbers (1,   1,3,5,7,9    ) ,    4 even number (  2,  4,  6,  8)
    deque<int>  v3{ 11, 12, 13, 19, 18, 17, 16, 15, 14, 11 };  // 6 odd numbers (11, 13,19,17,15,11) ,    4 even number ( 12, 18, 16, 14)

    // print original vector
    cout << G_TAB << "Original vector                   : [ ";
    for( const auto& e : v1 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    // print original list
    cout << G_TAB << "Original list                     : [ ";
    for( const auto& e : v2 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    // print original deque
    cout << G_TAB << "Original deque                    : [ ";
    for( const auto& e : v3 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    if ( is_permutation(v1.begin(), v1.end(), v2.begin() ) ) {
        cout << G_TAB << " v1 and v2 have equal elements. " << endl;
    } else {
        cout << G_TAB << " v1 and v2 don't have equal elements. " << endl;
    }

    auto bothEvenOrOdd = [](int e1, int e2) {
        return (e1 % 2 == e2 % 2);
    };

    if ( is_permutation(v1.begin(), v1.end(), v3.begin(), bothEvenOrOdd ) ) {
        cout << G_TAB << " v1 and v3 have numbers of even and odd elements. " << endl;
    } else {
        cout << G_TAB << " v1 and v3 don't have numbers of even and odd elements. " << endl;
    }

    cout << endl;

    v1 = {    1, 2, 3, 4     };
    v2 = { 9, 4, 1, 2, 3, 11 };

    // print original vector
    cout << G_TAB << "Original vector                   : [    ";
    for( const auto& e : v1 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    // print original list
    cout << G_TAB << "Original list                     : [ ";
    for( const auto& e : v2 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    auto newBeg = v2.begin();
    if ( is_permutation(v1.begin(), v1.end(), ++newBeg  ) ) {
        cout << G_TAB << " v1 and v2 have equal elements at the given range " << endl;
    } else {
        cout << G_TAB << " v1 and v2 don't have equal elements at the given range " << endl;
    }


    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}

void mismatchCase()
{
    ////////////////////////////////////////////////////////////////////////////
    //  pair<InputIterator1, InputIterator2>
    //  mismatch(ForwardIterator1 beg, ForwardIterator1 end, 
    //           ForwardIterator2 cmpBeg);
    //
    cout << G_TAB << "--------------- mismatch(...)  --------------- " << endl;
    vector<int> v1{ 1, 2, 3, 4,  5, 6 };
    list<int>   v2{ 1, 2, 4, 8, 16, 3 };

    // print original vector
    cout << G_TAB << "Original vector                   : [ ";
    for( const auto& e : v1 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    // print original list
    cout << G_TAB << "Original list                     : [ ";
    for( const auto& e : v2 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    auto itPair = mismatch( v1.begin(), v1.end(), v2.begin() );
    if (  itPair.first == v1.end() ) {
        cout << G_TAB << "v1 and v2 has no mismatch" << endl;
    } else {
        cout << G_TAB << "v1 and v2 has at least 1 mismatch : "
             << G_TAB << *(itPair.first)  << " and "
             << *(itPair.second) << endl;
    }

    itPair = mismatch( v1.begin(), v1.end(), v2.begin() , less_equal<int>()  );
    if (  itPair.first == v1.end() ) {
        cout << G_TAB << "v1 and v2 always less-or-equal. " << endl;
    } else {
        cout << G_TAB << "v1 and v2 not always less-or-equal : "
             << G_TAB << *(itPair.first)  << " and "
                      << *(itPair.second) << endl;
    }


    cout << G_TAB << "********************************************************************************" << endl;

    v1 = {    1, 2, 3, 4, 5, 6,  55 };
    v2 = { 9, 1, 2, 3, 4, 5, 6,  18 };

    // print original vector
    cout << G_TAB << "Original vector                   : [    ";
    for( const auto& e : v1 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    // print original list
    cout << G_TAB << "Original list                     : [ ";
    for( const auto& e : v2 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    auto it1 = v1.end();
    auto it2 = v2.begin();

    itPair = mismatch( v1.begin(), (--it1), (++it2)  );
    if (  itPair.first == it1 ) {
        cout << G_TAB << "v1 and v2 has No mismatch : ";

        cout << *(itPair.first)  << " and "
             << *(itPair.second) << endl;
    } else {
        cout << G_TAB << "v1 and v2 has at least 1 mismatch : "
             << G_TAB << *(itPair.first)  << " and "
             << *(itPair.second) << endl;
    }

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}


void sortedCase()
{
    ////////////////////////////////////////////////////////////////////////////
    //  //
    //  //  Check is Ascending Order as default    (   less<int>()   )
    //  //
    //  bool is_sorted(ForwardIterator beg, ForwardIterator end);  // default
    //  
    cout << G_TAB << "--------------- is_sorted(...)  --------------- " << endl;
    vector<int> v1{ 1, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    // print original vector v1
    cout << G_TAB << "Original vector v1                 : [ ";
    for( const auto& e : v1 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    if ( is_sorted(v1.cbegin(), v1.cend()  ) ) {
        cout << G_TAB << "v1 is well sorted  (In Ascending Order )" << endl;
    } else {
        cout << G_TAB << "v1 is not sorted " << endl;
    }

    map<int,string> mp;
    mp = { 
        { 1, "Bill" },
        { 2, "Jim"  },
        { 3, "Nico" },
        { 4, "Liu"  },
        { 5, "Ai"   }
    }; 

    cout << G_TAB << "Original mp                       : [ ";
    for( auto it = mp.cbegin(); it != mp.cend(); ++it ) {
        cout << "[ " << it->first << ", " << it->second << " ]" << ( (distance(it, mp.cend() ) != 1) ? ", " : "");
    }
    cout << " ] " << endl;

    auto cmpFunc = [](const pair<int,string> e1, const pair<int,string> e2) {
        return   e1.second < e2.second;
    };

    if ( is_sorted(mp.cbegin(), mp.cend(), cmpFunc) ) {
        cout << G_TAB << " names in mp are sorted" << endl;
    } else {
        cout << G_TAB << " names in mp are not sorted" << endl;
    }

    // print first unsorted name
    auto pos = is_sorted_until( mp.cbegin(), mp.cend(), cmpFunc );
    if ( pos != mp.cend() ) { 
        cout << G_TAB << " first unsorted name : " << pos->second << endl;
    }


    vector<int> v2{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 1 };
    // print original vector v1
    cout << G_TAB << "Original vector v2                 : [ ";
    for( const auto& e : v2 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    if ( is_sorted(v2.cbegin(), v2.cend(),  greater<int>()  ) ) {
        cout << G_TAB << "v2 is well sorted  (In Descending Order )" << endl;
    } else {
        cout << G_TAB << "v2 is not sorted " << endl;
    }

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}



void is_partitionedCase()
{
    cout << G_TAB << "--------------- is_sorted(...)  --------------- " << endl;
    vector<int> v1{  -3, -2, -1,       1, 2, 3, 4, 5  }; 
    // print original vector v1
    cout << G_TAB << "Original vector v1                  : [ ";
    for( const auto& e : v1 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    auto criterionFunc1 = [](int e) -> bool {
        return e < 0;
    };

    if ( is_partitioned(v1.cbegin(), v1.cend() , criterionFunc1 ) ) {
        cout << G_TAB << " v1 is partitioned as 2 parts :    [ elements<0 ]   |   [ elements>=0 ]" << endl;
        auto pos = partition_point( v1.cbegin(), v1.cend(), criterionFunc1 );
        if ( pos != v1.end() ) { 
            cout << G_TAB << "partition_point :  pos -> " << *pos << endl;
        } else {
            cout << G_TAB << "partition_point :  pos -> end() " << endl;
        }
    } else {
        cout << G_TAB << " v1 is <Not> partitioned . " << endl;
    }

    cout << G_TAB << "********************************************************************************" << endl;

    vector<int> v2{ 5, 3, 9, 1, 3, 4, 8, 2, 6 };
    // print original vector v2
    cout << G_TAB << "Original vector v2                 : [ ";
    for( const auto& e : v2 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;

    auto criterionFunc2 = [](int e) -> bool {
        return e % 2 == 1;
    };

    if ( is_partitioned(v2.cbegin(), v2.cend() , criterionFunc2 ) ) {
        cout << G_TAB << " v2 is partitioned as 2 parts :    [ odd numbers ]   |   [ even numbers ]" << endl;
        auto pos = partition_point( v2.cbegin(), v2.cend(), criterionFunc2 );
        if ( pos != v2.end() ) { 
            cout << G_TAB << "partition_point :  pos -> " << *pos << endl;
        } else {
            cout << G_TAB << "partition_point :  pos -> end() " << endl;
        }
    } else {
        cout << G_TAB << " v2 is <Not> partitioned . " << endl;
    }



    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}


void all_any_noneCase()
{
    ////////////////////////////////////////////////////////////////////////////
    //
    //  bool  all_of(ForwardIterator beg, ForwardIterator end, UnaryPredicate op);   // if container is empty() , return true
    //  bool  any_of(ForwardIterator beg, ForwardIterator end, UnaryPredicate op);   // if container is empty() , return false
    //  bool none_of(ForwardIterator beg, ForwardIterator end, UnaryPredicate op);   // if container is empty() , return true
    //
    cout << G_TAB << "--------------- all_of(...) / any_of(...) / none_of(...) --------------- " << endl;
    vector<int> v1{  1, 2, 3, 4 };
    // print original vector v1
    cout << G_TAB << "Original vector v1                  : [ ";
    for( const auto& e : v1 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;


    auto criterionFunc1 = [](int ele) {
        return ele > 0;
    }; 

    if ( all_of(v1.begin(), v1.end(), criterionFunc1) ) {
        cout << G_TAB << " in if,  All elements of v1 are greater than 0." << endl;
    } else {
        cout << G_TAB << " in else,  <Not> elements of v1 are greater than 0." << endl;
    }

    auto criterionFunc2 = [](int ele) {
        return ele == 4;
    }; 

    if ( any_of(v1.begin(), v1.end(), criterionFunc2) ) {
        cout << G_TAB << " in if,  At least 1 element of v1 is '4'." << endl;
    } else {
        cout << G_TAB << " in else,  No '4' in the container v1. " << endl;
    }

    auto criterionFunc3 = [](int ele) {
        return ele <= 0;
    }; 

    if ( none_of(v1.begin(), v1.end(), criterionFunc3) ) {
        cout << G_TAB << " in if,  None of element is less than 0. " << endl;
    } else {
        cout << G_TAB << " in else,  At least one element is less than 0 " << endl;
    }


    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}


} // end namespace  NonModifying
