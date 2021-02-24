#include <iostream>
#include <string>
#include <map>
using namespace std;

//////////////////////////////////////////////////////////////////////
//
// keyword : decltype
//
//////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[], char* env[])
{
	map<string,float> coll;
	// value_type	:     std::pair<const Key, T>
	/*
	 * By using the new decltype keyword, you can let the compiler find out the type of an expression.
	 * This is the realization of the often requested "typeof" feature. However, 
	 * the existing typeof implementations were inconsitent and incomplete, so C++11 introduced a new keyword  : decltype
	 *
	 *
	 * 1. On application of decltype is to declare return types
	 * 2. Another is to use it in metaprogramming or to pass the type of a lambda
	*/
	decltype(coll)::value_type elem;	//	std::pair<const string, float>  elem;

	return 0;
}


