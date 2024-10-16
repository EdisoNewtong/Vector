#include <iostream>
#include <string>

// #include <format>
#include <fmt/format-inl.h>
using namespace std;



void test1()
{
    int age = 32;
    string strAge = "age";

    // auto formated_strError = std::format("{} = {}", strAge, age);

    // formated_str is a 'std::string'   type
    auto formated_str = fmt::format("{} = {}", strAge, age);
    cout << formated_str << endl;
}

void test2()
{
    string firstName = "Zhang";
    string lastName  = "ChaoHua";

    //
    // The Base index is following the C Language convention ==>  [0]  ( Zero-Base )
    //

    //                                firstName  lastName
    //                                      [0] [1]
    cout << fmt::format("My Chinese name is {0}-{1}",  firstName, lastName) << endl;

    //                                 lastName  firstName
    //                                      [1] [0]
    cout << fmt::format("My English name is {1}-{0}",  firstName, lastName) << endl;

}


void test3()
{
    int a = 12;
    int b = 345;

    cout << fmt::format("\"{0:<3}\" < \"{1:<3}\"",  a, b) << endl;
    cout << fmt::format("\"{0:>3}\" < \"{1:<3}\"",  a, b) << endl;

    cout << fmt::format("\"{1:<3}\" > \"{0:<3}\"",  a, b) << endl;

    cout << endl;
    cout << fmt::format("\"{:^6}\"", a) << endl;
    cout << fmt::format("\"{:*^6}\"", a) << endl;

    int iNum = 43; // 101011(binary) , 53(octal),  2B(hex)
    cout << fmt::format("lNum = {}",   iNum) << endl;
    cout << fmt::format("lNum = {:b}", iNum) << endl;   // binary format : 1100....
    cout << fmt::format("lNum = {:#b}", iNum) << endl;  // binary format : 0b.....
    cout << fmt::format("lNum = {:#B}", iNum) << endl;  // binary format : 0B.....
    cout << fmt::format("lNum = \"{:d}\"", iNum) << endl;   // "43"
    cout << fmt::format("lNum = \"{:5d}\"", iNum) << endl;   // "   43"  ( begin with 3 spaces )
    cout << fmt::format("lNum = \"{:>5d}\"", iNum) << endl;  // "   43"  ( begin with 3 spaces )
    cout << fmt::format("lNum = \"{:<5d}\"", iNum) << endl;  // "43   "  ( end   with 3 spaces )
    cout << fmt::format("lNum = {:0>5}", iNum) << endl; // 43 -->  00043
    cout << fmt::format("lNum = {:0<5}", iNum) << endl; // 43 -->  43000
    cout << fmt::format("lNum = {:#o}", iNum) << endl;  // '#' means add prefix of  ???,   'o' means format the number in octal form   (  0.... )
    cout << fmt::format("lNum = {:#x}", iNum) << endl;  // '#' means add prefix of  ???,   'x' means format the number in hex form     ( 0x... )   use lower case hex code : abcdef
    cout << fmt::format("lNum = {:#X}", iNum) << endl;  // '#' means add prefix of  ???,   'X' means format the number in hex form     ( 0X... )   use upper case hex code : ABCDEF

    //
    // float number "Format"
    //
    float fNum = 123.45f;
    int nWidthAfterDot = 4;
    string nameOftheType = "float";

    cout << fmt::format("\"1234567890123456\"") << endl;

    //
    //                                      "123"   "."   "450000"
    // total 13 width   :   3 space(s)  +      (3 +  1   +  6)    =  13
    cout << fmt::format("\"{:13.6f}\" is a {} number", fNum, nameOftheType) << endl;

    cout << fmt::format("\"{:.{}f}\" is a {} number", fNum, nWidthAfterDot, nameOftheType) << endl;

}

void test4(const string& fmt, const string& a,const string& b)
{
    cout << fmt::vformat(fmt.c_str(), fmt::make_format_args(a,b)  ) << endl;
}


int main(int argc, char* argv[], char* env[])
{
    test1();
    test2();
    test3();

    // Run-Time dynamic format string
    test4( std::string(argv[1]), argv[2], argv[3] );

    return 0;
}
