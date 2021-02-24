#include <iostream>
#include <string>
using namespace std;

//////////////////////////////////////////////////////////////////////
//
// Variadic Template  &&     Alias Templates ( Template Typedef )
//
//////////////////////////////////////////////////////////////////////



/*
 
 main.cpp: In instantiation of ‘void my_print(const T&, const Types& ...) [with T = std::basic_string<char>; Types = {}]’:
 main.cpp:21:18:   recursively required from ‘void my_print(const T&, const Types& ...) [with T = long unsigned int; Types = {char [13], bool, std::basic_string<char, std::char_traits<char>, std::allocator<char> >}]’
 main.cpp:21:18:   required from ‘void my_print(const T&, const Types& ...) [with T = int; Types = {long unsigned int, char [13], bool, std::basic_string<char, std::char_traits<char>, std::allocator<char> >}]’
 main.cpp:26:70:   required from here
 main.cpp:21:18: 错误：对‘my_print()’的调用没有匹配的函数
   my_print(args...);
                     ^
					 main.cpp:21:18: 附注：备选是：
					 main.cpp:18:6: 附注：template<class T, class ... Types> void my_print(const T&, const Types& ...)
					  void my_print(const T& firstArg, const Types&... args)
					        ^
							main.cpp:18:6: 附注：  template argument deduction/substitution failed:
							main.cpp:21:18: 附注： 备选需要 2 实参，但提供了 0 个
							  my_print(args...);
							                    ^
make: *** [all] 错误 1

*/


// This empty function named my_print is a must
// Otherwise it will raise an compile time error
// This function will be called if all the arguments is processed 
//--------------------------------------------------------------------------------------------------------------------------------------------
// my_print() is     to end of recursion, the non-template overload of the print is provided
//--------------------------------------------------------------------------------------------------------------------------------------------
void my_print()
{
	//--------------------------------------------------------------------------------------------------------------------------------------------
	// Core Core Core
	//--------------------------------------------------------------------------------------------------------------------------------------------
	// the following code will be invoked at the end of the variadic templates
	cout << "End of my_print()" << endl;
}


template <typename T, typename... Types>
void my_print(const T& firstArg, const Types&... args)
{
	cout << firstArg << endl;
	// inside variadic templates, sizeof...(args) yields the number of arguments
	my_print(args...);
}



/*

template <typename T>
void my_print(const T& arg) 
{
	cout << arg << endl;
}

template <typename T, typename... Types>
void my_print(const T& firstArg, const Types&... args)
{
	cout << firstArg << endl;
	my_print(args...);
}

*/




int main(int argc, char* argv[], char* env[])
{
	my_print(111,222UL,"Hello World ", true, string("c++ string A1b2C3"));

	/*
	template <typename T>
	using Vec = std::vector<T, MyAlloc<int>> coll;
	

	==>
	the term
		Vec<int> coll;
	is equivalent to:
		std::vector<int, MyAlloc<int>> coll;

	*/

	return 0;
}


