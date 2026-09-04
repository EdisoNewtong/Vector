// Build with a compiler that support C++17 standard
#include <iostream>
#include <string>
#include <tuple>  // feature since c++11
using namespace std;

/*****************************************************************

  Structured Binding Declaration (since C++17)

*****************************************************************/
struct Dummy
{
    string ss;
    char   buf[3];
    int    ary[5];
    bool   flag;
};

void Compiler_ErrorReport( const Dummy* pObj )
{
    cout << pObj->ss << endl;
}

struct PositionWithName
{
    int w;
    int x;
    int y;
    int z;
    string name;
};

void test0()
{
    int ary[2] = { 1,2 };
    cout << "0.  ary[0] = " << ary[0] << endl; // output 1
    cout << "0.  ary[1] = " << ary[1] << endl; // output 2

    // x,y are 2 [*** COPY ***] values by ary
    //    x = ary[0]
    //    y = ary[1]
    auto [x,y] = ary;
    x = 123;
    y = 456;
    cout << "1.  ary[0] = " << ary[0] << endl; // output 1
    cout << "1.  ary[1] = " << ary[1] << endl; // output 2

    // x,y are 2 reference by ary
    //  int&  rx = ary[0]
    //  int&  ry = ary[1]
    /**********************************************************************************************************************************

     The change of value rx and ry will take effect on ary[0]/ ary[1]  immediately

    **********************************************************************************************************************************/
    auto& [rx,ry] = ary;
    rx = 123; ry = 456;
    cout << "2.  ary[0] = " << ary[0] << endl; // output 123
    cout << "2.  ary[1] = " << ary[1] << endl; // output 456

}

void test1()
{
    PositionWithName pos = { 10, 20, 30, 40, "Home Point" };

    cout << "0. w = "    << pos.w  << endl;   // w = 10
    cout << "0. x = "    << pos.x  << endl;   // x = 20
    cout << "0. name = " << pos.name << endl;  // name = "Home Point"
    cout << endl;

    // wVal, x_Val, y_Val, z_Val  are all <int> type ( a value copy , it's value doesn't take effect on the real of o.a )
    auto  [w_Val,x_Val,y_Val,z_Val,name] = pos;
    w_Val = 1234;
    name = "Hello";

    cout << "1. w = "    << pos.w  << endl;   // num = 10
    cout << "1. name = " << pos.name << endl;  // name = "Home Point"
    cout << endl;

    // wVal, x_Val, y_Val, z_Val  are all <int&> type ( they refers to each field w,x,y,z,   name )
    auto&  [rw_Val,rx_Val,ry_Val,rz_Val,rname] = pos;

    // pos.x from      20 -> 40
    rx_Val *= 2;
    // pos.name from   "Hello" --> "World"
    rname = "Target Point";

    cout << "2. x = "    << pos.x  << endl;   // num = 40
    cout << "2. name = " << pos.name << endl;  // name = "Target Point"
    cout << endl;

    // 
    // Notes :
    //      if you want to know what's real type of the variable defined by 'auto' keyword
    // Please uncomment the following statement like the following 
    //
    // The Compiler will report some error message to tell you the real type by mismatched variable type
    //
    /*******************************************************/
    //
    // Compiler_ErrorReport( w_Val ); 
    // Compiler_ErrorReport( name ); 
    //
    // Compiler_ErrorReport( rw_Val );
    // Compiler_ErrorReport( rname );
    //
    /*******************************************************/
}

/***********************************************************************************************************************************

( Until C++26 )
  The number of identifiers in sb-identifier-list must be equal to the structured binding size of E.

( Since C++26 )
Given the number of identifiers in sb-identifier-list as N and the structured binding size of E as S:
If there is no structured binding pack, N must be equal to S.
Otherwise, the number of non-pack elements (i.e., N - 1) must be less than or equal to S, and the number of elements of the structured binding pack is S - N + 1 (which can be zero).


// Since C++26 
// The Compiler Must Support C++26
**********************************************************************************************************************************/
void test2()
{
    PositionWithName pos = { 10, 20, 30, 40, "Home Point" };

    //
    // Compile [ERROR]
    // decomposes into 5 elements, but only 2 names were provided
    //
    /**********************************************************************
    auto  [w_Val,x_Val] = pos; 
    **********************************************************************/



    //
    // Compile [ERROR]
    // decomposes into 5 elements, but 6 names were provided
    //
    /**********************************************************************
    // auto  [w1,x1,y1,z1,name1,other1] = pos;
    **********************************************************************/

    
    /**********************************************************************
    auto [w2, ...e] = pos;
    cout << "w2     = " << w2 << endl;
    auto [...z3, pName] = pos;
    cout << "z3     = " << z3 << endl;
    // cout << "other2 = " << other2 << endl;
    **********************************************************************/
}


void tuple_test()
{
    // tObj1 is a tuple<int,const char*> type
    auto tObj1 = std::make_tuple( 123, "abc");
    (void)tObj1;

    // tObj2 is a tuple<unsigned int,const char*> type
    auto tObj2 = std::make_tuple( 123u, string("abc") );
    (void)tObj2;

    tuple<float,float> tObj3(3.141,  2.718);
    auto [pi,e] = tObj3;
    cout << "pi = " << pi << " = std::get<0>(tObj3) = " << std::get<0>(tObj3) << endl; 
    cout << "e  = " <<  e << " = std::get<1>(tObj3) = " << std::get<1>(tObj3) << endl;

    // piVal and eVal must be defined before use  tie( ... , ... )
    float piVal,eVal;
    std::tie(piVal, eVal) = tObj3;
    cout << "piVal = " << piVal << endl;
    cout << "eVal  = " << eVal  << endl;

    int x;
    // double y;
    std::string z;
    // ignore   double y = 2.0;
    std::tie(x, std::ignore, z) = std::tuple(1, 2.0, "Test");
    cout << "x = " << x << endl;
    //cout << "y = " << y << endl;
    cout << "z = " << z  << endl;

}




int main(int argc, char* argv[], char* env[])
{
    test0();
    cout << endl;
    test1();
    cout << endl;
    test2();
    cout << endl;
    tuple_test();

    return 0;
}




