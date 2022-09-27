#include <iostream>
#include <iomanip>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>  // for function   iota(...)

#include "Modifying.h"
using namespace std;

static const char G_TAB = '\t';

namespace Modifying
{
    


void testCaseAll()
{
    cout << "######################### Modifying Algorithm Begin #########################" << endl;
    copyCase();
    moveCase();
    transformCase();
    swapRangeCase();
    fillCase();
    generateCase();
    iotaCase();
    replaceCase();
    replace_copyCase();
    cout << "######################### Modifying Algorithm End   #########################" << endl;
}


void copyCase()
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Core Core Core :
    //                   You must make sure  destination Container has enough space to be copied
    //    
    //      OutputIterator         copy(InputIterator srcBeg, InputIterator srcEnd, OutputIterator destBeg);
    //      OutputIterator         copy_if(InputIterator srcBeg, InputIterator srcEnd, OutputIterator destBeg, UnaryPredicate op);
    //      OutputIterator         copy_n(InputIterator srcBeg, Size num, OutputIterator destBeg);
    // 
    //                                                                                                                          *****  End Iterator *****
    //      BidirectionalIterator  copy_backward(BidirectionalIterator1 srcBeg, BidirectionalIterator1 srcEnd, BidirectionalIterator2 destEnd);
    cout << G_TAB << "--------------- copy(...) / copy_if(...) / copy_n(...) / copy_backward(...) --------------- " << endl;
    vector<string> v1{ "Hello", "this", "is", "an", "example" };
    list<string>   v2;

    cout << G_TAB << "Original vector v1                      : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<string>(cout, " ") );
    cout << " ] " << endl;
    

    /************************************************************************************************************************

    // Core Core Core :
    // ERROR   ERROR   ERROR   ERROR   ERROR    
    v2.resize( v1.size() );  // otherwise the program will raise a <Run-Time>   !!! [Error] !!!

    *************************************************************************************************************************/
    copy( v1.begin(), v1.end(), back_inserter(v2) );   // or    front_inserter(v2)
    cout << G_TAB << "AfterCopy vector v2(back_inserter)      : [ ";
    copy( v2.begin(), v2.end(), ostream_iterator<string>(cout, " ") );
    cout << " ] " << endl;

    v2.clear();
    copy( v1.begin(), v1.end(), front_inserter(v2) );   // or    back_inserter(v2)
    cout << G_TAB << "AfterCopy vector v2(front_inserter)     : [ ";
    copy( v2.begin(), v2.end(), ostream_iterator<string>(cout, " ") );
    cout << " ] " << endl;

    copy( v1.begin(), v1.end(), v2.begin() );
    cout << G_TAB << "AfterCopy vector v2(override existed)   : [ ";
    copy( v2.begin(), v2.end(), ostream_iterator<string>(cout, " ") );
    cout << " ] " << endl;


    cout << G_TAB << "********************************************************************************" << endl;
    vector<int> v3{ 1, 2, 3, 4 };
    vector<int> v4;
    cout << G_TAB << "Original vector v3                         : [ ";
    copy( v3.begin(), v3.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    auto criterionFunc = [](int ele) {
        return ele % 2 == 1;
    };

    copy_if( v3.begin(), v3.end(), back_inserter(v4), criterionFunc );
    cout << G_TAB << "AfterCopy vector v4(back_inserter) OddFunc : [ ";
    copy( v4.begin(), v4.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    v4.clear();
    v4.resize( 3 );
    cout << G_TAB << "AfterCopy vector v4( copy_n 3 elements )   : [ ";
    copy_n( v3.begin(), 3, v4.begin() );
    copy( v4.begin(), v4.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;


    v4 = { -1, -1, -1, -1, -1, -1, -1, -1 };
    cout << G_TAB << "  assign vector v4                           : [ ";
    copy( v4.begin(), v4.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    auto itEnd = v4.end();
    --itEnd;

    copy_backward( v3.begin(), v3.end(), itEnd );
    cout << G_TAB << "  AfterCopy vector v4( copy_backward @end-1) : [ ";
    copy( v4.begin(), v4.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    copy_backward( v3.rbegin(), v3.rend(), itEnd );
    cout << G_TAB << "  AfterCopy vector v4( backward rbeg~rend @end-1) : [ ";
    copy( v4.begin(), v4.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;

}



void moveCase()
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Core Core Core :
    //       the move function do not really move element out of container : 
    //                                                        Instead  use :   *destElem = std:move(*sourceElem);
    //
    cout << G_TAB << "--------------- move(...) / move_backward(...) --------------- " << endl;
    vector<int> v1{ 0, 1, 2, 3, 4, 5 };
    vector<int> v2;

    cout << G_TAB << "Original vector v1                      : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    v2.resize( v1.size() );
    cout << G_TAB << "Original vector v2                      : [ ";
    copy( v2.begin(), v2.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    auto v1beg = v1.begin();   
    ++v1beg;

    auto v1end = v1.end();     
    --v1end;

    move(v1beg, v1end, v2.begin() ); 
    cout << G_TAB << "After Move vector v1                    : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    cout << G_TAB << "After Move vector v2                    : [ ";
    copy( v2.begin(), v2.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}





void transformCase()
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // OutputInterator  transform(InputIterator srcBeg, InputIterator srcEnd,
    //                            OutputIterator destBeg,
    //                            UnaryFunc op);
    // 
    //---------------------------------------------------------------------------------------------------- 
    // 
    //                                Op(element)
    //
    //     <Original Element>      ===================>       <Transformed Element>
    // 
    //
    //
    //####################################################################################################
    //
    //
    //
    // OutputInterator  transform(InputIterator1 src1Beg, InputIterator1 src1End,
    //                            InputIterator2 src2Beg,
    //                            OutputIterator destBeg,
    //                            BinaryFunc op);
    // 
    //---------------------------------------------------------------------------------------------------- 
    // 
    //     <Original Element 1>         Op(ele1, ele2)
    //                               ===================>       <Transformed Element>
    //     <Original Element 2>                                 
    // 
    cout << G_TAB << "--------------- transform(...)  --------------- " << endl;
    vector<int> v1{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    list<int>   v2;

    cout << G_TAB << "Original vector v1                  : [ ";
    // print original
    for( const auto& e : v1 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;


    transform( v1.begin(), v1.end(), v1.begin(),  negate<int>() );
    cout << G_TAB << "Negate          v1                  : [ ";
    // print original
    for( const auto& e : v1 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;


    transform( v1.begin(), v1.end(), back_inserter(v2),  bind(multiplies<int>(), std::placeholders::_1, 10 ) );
    cout << G_TAB << "Multipies * 10 : vector v2          : [ ";
    // print original
    for( const auto& e : v2 ) {
        cout << setw(2) << e << " ";
    }
    cout << " ] " << endl;



    cout << G_TAB << "Back output * (-1)   : vector v2    : [ ";
    transform( v2.crbegin(), v2.crend(), ostream_iterator<int>(cout, " "),
         [](int ele) {
           return -ele;
         }
    );
    cout << " ] " << endl;

    cout << G_TAB << "********************************************************************************" << endl;

    vector<int> v3{ 100, 200, 300, 400, 500, 600, 700, 800, 900 };
    vector<int> v4{   5,  10,  15,  20,  25,  30,  35,  40,  45 };

    cout << G_TAB << "Original vector v3                  : [ ";
    // print original
    for( const auto& e : v3 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;

    cout << G_TAB << "Original vector v4                  : [ ";
    // print original
    for( const auto& e : v4 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;

    v2.clear();
    v2.resize( v3.size() );
    transform( v3.begin(), v3.end(), // src     container   1
               v4.begin(),           // src     container   2
               v2.begin(),           // dest    container
               [](int e1, int e2) {
                    return e1-e2;
               }
    );

    cout << G_TAB << "(v3 - v4) =>  vector v2             : [ ";
    // print original
    for( const auto& e : v2 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}



void swapRangeCase()
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // 
    cout << G_TAB << "--------------- swap_ranges(...)  --------------- " << endl;
    vector<int>  v1;
    deque<int>   v2;
    for( int n = 1;  n <= 9; ++n ) { v1.push_back(n); }
    for( int n = 11; n <=23; ++n ) { v2.push_back(n); }

    cout << G_TAB << "Original vector v1                  : [ ";
    // print original
    for( const auto& e : v1 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;

    cout << G_TAB << "Original vector v2                  : [ ";
    // print original
    for( const auto& e : v2 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;
    
    deque<int>::iterator pos;
    pos = swap_ranges( v1.begin(), v1.end(), v2.begin() );
    cout << G_TAB << "AfterSwap vector v1                 : [ ";
    // print original
    for( const auto& e : v1 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;

    cout << G_TAB << "AfterSwap vector v2                 : [ ";
    // print original
    for( const auto& e : v2 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;

    if ( pos != v2.end() ) {
        cout << G_TAB << " first element not modified : " << *pos << endl;
    }


    cout << G_TAB << "After Mirror 3 elements vector v2   : [ ";
    // mirror frist 3 with last 3 elements in  v2
    swap_ranges( v2.begin(), v2.begin() + 3,   v2.rbegin() );
    // print original
    for( const auto& e : v2 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;

}



void fillCase()
{
    cout << G_TAB << "--------------- fill(...) / fill_n(...) --------------- " << endl;
    list<string> v1;

    cout << G_TAB;
    fill_n( ostream_iterator<float>(cout, "   "),
            10,                                 // Count : 7 times
            7.7f);                              // fill 7.7f every time  for 7 times
    cout << endl;

    fill_n( back_inserter(v1),
            9,
            "hello");

    cout << G_TAB << "After fill 9 * \"hello\" v1                       : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<string>(cout, " ") );
    cout << " ] " << endl;

    // override all elements in the container : v1    with  "again"
    fill( v1.begin(), v1.end(), "again");
    cout << G_TAB << "After override all elements v1 with again       : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<string>(cout, " ") );
    cout << " ] " << endl;
    
    // replace all but last two elements with "hi"
    fill_n( v1.begin(), v1.size()-2, "hi");
    cout << G_TAB << "After fill all elements but two v1 with hi      : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<string>(cout, " ") );
    cout << " ] " << endl;

    list<string>::iterator pos1;
    list<string>::iterator pos2;
    pos1 = v1.begin();
    pos2 = v1.end();

    fill( ++pos1, --pos2, "hmmm");
    cout << G_TAB << "After all elements without head/tail v1 with hi : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<string>(cout, " ") );
    cout << " ] " << endl;


    // cout << G_TAB << "********************************************************************************" << endl;

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;

}





void generateCase()
{
    // Core Core Core : the selfDefinedFunc is like    
    //--------------------------------------------------
    //      T   func(void) {
    //           T  result;
    //                ...
    //           return result;
    //      }
    //--------------------------------------------------
    cout << G_TAB << "--------------- generate(...) / generate_n(...) --------------- " << endl;
    list<int> v1;
    
    // insert five random numbers
    generate_n( back_inserter(v1),
                5,
                rand);
    cout << G_TAB << "After generate 5 random numbers, v1                       : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    // override with 5 new random numbers
    generate( v1.begin(),
              v1.end(),
                rand);
    cout << G_TAB << "After override 5 random numbers, v1                       : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    // cout << G_TAB << "********************************************************************************" << endl;

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}



void iotaCase()
{
    /* iota(...) // #include <numeric>
       assign all the elements with the following :  

                startValue,  
                startValue + 1, 
                startValue + 2, 
                startValue + 3,
                     ...
                startValue + ( nElements-1),
    */
    cout << G_TAB << "--------------- iota(...)  --------------- " << endl;
    list<int> v1;
    v1.resize( 10 ); // alloc a 10 space container
    
    // insert five random numbers
    iota( v1.begin(), v1.end(), 1);
    cout << G_TAB << "After assign all elements step + 1 , v1               : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}



void replaceCase()
{
    // void replace_if( ForwardIterator beg, ForwardIterator end, UnaryPredicate op, const T& newValue);
    cout << G_TAB << "--------------- replace(...) / replace_if(...) --------------- " << endl;
    list<int> v1;
    for( int n = 2; n <= 7; ++n ) { v1.push_back(n); }
    for( int n = 4; n <= 9; ++n ) { v1.push_back(n); }
    cout << G_TAB << "Original vector v1                  : [ ";
    // print original
    for( const auto& e : v1 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;

    // replace  ( all elements whose value is 6 )    with 42
    replace( v1.begin(), v1.end(), 6,  42);
    // print  v1 after replaced
    cout << G_TAB << "replace  vector v1 ( 6 <-- 42 )     : [ ";
    for( const auto& e : v1 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;




    replace_if( v1.begin(), v1.end(), 
                [](int ele) -> bool {
                    return  ele < 5;
                },
                0);
    // print  v1 after replace_if
    cout << G_TAB << "replace_if vector v1 ((e<5) <--  0) : [ ";
    for( const auto& e : v1 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;


    cout << G_TAB << endl;
}

void replace_copyCase()
{
    cout << G_TAB << "--------------- replace_copy(...) / replace_copy_if(...) --------------- " << endl;

    list<int> v1;
    for( int n = 2; n <= 6; ++n ) { v1.push_back(n); }
    for( int n = 4; n <= 9; ++n ) { v1.push_back(n); }
    cout << G_TAB << "Original vector v1                  : [ ";
    // print original
    for( const auto& e : v1 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;


    cout << G_TAB << "Replace all 5 with 55               : [ ";
    replace_copy( v1.begin(), v1.end(), v1.begin(), 5, 55);
    // print original
    for( const auto& e : v1 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;



    replace_copy_if( v1.begin(), v1.end(), v1.begin(), 
            bind( less<int>(), std::placeholders::_1, 5),  // ele < 5
            42);
    cout << G_TAB << "Replace all elements < 5 with 42    : [ ";
    // print original
    for( const auto& e : v1 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;



    replace_copy_if( v1.begin(), v1.end(), v1.begin(), 
            [](int elem) -> bool {
                return elem % 2 == 1;
            },
            0);
    cout << G_TAB << "Replace all odd elements  with 0    : [ ";
    // print original
    for( const auto& e : v1 ) {
        cout << setw(3) << e << " ";
    }
    cout << " ] " << endl;

    cout << G_TAB << endl;
}


}


