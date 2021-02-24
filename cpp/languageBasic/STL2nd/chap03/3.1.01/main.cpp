#include <iostream>
using namespace std;

//////////////////////////////////////////////////////////////////////
//
// keyword : nullptr_t
//
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
void f(int)
{
	cout << "in f(int)" << endl;
}

void f(void*)
{
	cout << "in f(void*)" << endl;
}


void keyword_nullptr() 
{
	f(0); // call f(int)
	/*
	 *
		main.cpp:21:8: 错误：调用重载的‘f(NULL)’有歧义
		  f(NULL);  // calls f(int) if NULL is 0, ambiguous otherwise
				  ^
		main.cpp:21:8: 附注：备选是：
		main.cpp:7:6: 附注：void f(int)
		  void f(int)
			   ^
		main.cpp:12:6: 附注：void f(void*)
		  void f(void*)
		       ^
		make: *** [all] 错误 1

	*/
	// before C++11  calls f(int) if NULL is 0, ambiguous otherwise
	// f(NULL);



	// OK , here nullptr is a new keyword in C++ 11
	// nullptr is a type of std::nullptr_t, whose is defined in header file <csdtdef>
	f(nullptr); // call f(void*)
}
//////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[], char* env[])
{
	keyword_nullptr();
	return 0;
}

