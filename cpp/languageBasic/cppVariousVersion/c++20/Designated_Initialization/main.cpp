#include <iostream>
#include <string>
using namespace std;

union u { int a; const char* b; };

struct PositionWithName
{
    int w,x,y,z;
    string name;
};


void test1()
{
    PositionWithName p1 { .w = 1, 
                          /*   .x = 0, */
                          .y = 2
                          /*   .z = 0, */
                          /*   .name = "", */
                        };

    cout << "p1.w = " << p1.w << endl;
    cout << "p1.x = " << p1.x << endl;
    cout << "p1.y = " << p1.y << endl;
    cout << "p1.z = " << p1.z << endl;
    cout << "p1.name = \"" << p1.name << "\"" << endl << endl;


    /****************************************************************************************************
      [WARNING] :
      main.cpp:44:32: warning: ISO C++ requires field designators to be specified in declaration order;
      field 'x' will be initialized after field 'w' [-Wreorder-init-list]
   44 |                           .w = 2
      |                           ~~~~~^
main.cpp:43:32: note: previous initialization for field 'x' is here
   43 |                           .x = 1,
      |                                ^

    ****************************************************************************************************/

    PositionWithName p2 { 
                          .x = 1,   // <= initialized order can't be reversed , because w is defined before x
                          .w = 2,   // <= initialized order can't be reversed
                          /*   .z = 0, */
                          /*   .name = "", */
                        };

    cout << "p2.w = " << p2.w << endl; // w = 2,   correct by g++ ( other compiler's behavior may be undecided )
    cout << "p2.x = " << p2.x << endl; // x = 1,   correct by g++ ( other compiler's behavior may be undecided )
    cout << "p2.y = " << p2.y << endl;
    cout << "p2.z = " << p2.z << endl;
    cout << "p2.name = \"" << p2.name << "\"" << endl;
    
}

void test2()
{
    /****************************************************************************************************
      [ERROR]
      main.cpp:72:20: error: initializer partially overrides prior initialization of this subobject
      [-Winitializer-overrides]
   72 |     u g = {.a = 1, .b = "asdf"}; // Error, only one initializer may be provided
      |                    ^~~~~~~~~~~
main.cpp:72:17: note: previous initialization is here
   72 |     u g = {.a = 1, .b = "asdf"}; // Error, only one initializer may be provided
      |                 ^

    ****************************************************************************************************/
    u f = {.b = "asdf"};         // OK, active member of the union is b
    cout << "f.b = " << f.b << endl;
    // cout << "f.a = " << std::dec << f.a << " = 0x" << std::hex << f.a << endl;
    u g = {.a = 1, .b = "asdf"}; // Error, only one initializer may be provided 
    cout << "g.a = " << g.b << endl; 
}


int main(int argc, char* argv[], char* env[])
{
    test1();
    cout << endl;
    test2();


    return 0;
}






