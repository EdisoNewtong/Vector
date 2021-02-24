#include <iostream>
#include <vector>
using namespace std;

//////////////////////////////////////////////////////////////////////
//
// keyword : auto
//
//////////////////////////////////////////////////////////////////////


double f()
{
	return 3.14;
}


int main(int argc, char* argv[], char* env[])
{
	auto i = 42;	// i has type int
	(void)i;

	auto d = f();   // d has type double
	(void)d;

	// Peking is the alias of Beijing
	auto word = "Peking";	// word has type const char*
	(void)word;


	vector<string> vec;
	auto pos = vec.begin();	// pos has type vector<string>::iterator
	(void)pos;


	// l has type of a lambda 
	// taking an int and returning a bool
	auto l = [] (int x) -> bool {
		(void)x;
		return true;
	};
	(void)l;

	
	return 0;
}

