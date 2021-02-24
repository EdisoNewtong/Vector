#include <iostream>
#include <string>
#include <map>
using namespace std;

//////////////////////////////////////////////////////////////////////
//
// keyword : decltype  (New Function Declartion Syntax)
//
//////////////////////////////////////////////////////////////////////

/*
 * Sometimes the return type of a function depends on an expression processed with the arguments
 * However, something like

template<typename T1, typename T2>
decltype(x+y)   add(T1 x, T2 y);

is not possible before C++11, because the compile can't get the defination of x and y before     add(T1 x, T2 y)

*/


//
// Compile OK
//
template<typename T1, typename T2>
auto add(T1 x, T2 y) -> decltype(x+y)
{
	return x+y;
}



int main(int argc, char* argv[], char* env[])
{
	auto ret = add(3,5);
	cout << "ret = " << ret << endl;

	return 0;
}


