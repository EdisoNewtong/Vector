#include <iostream>
#include <vector>
using namespace std;

//////////////////////////////////////////////////////////////////////
//
// keyword : constexpr
//
//////////////////////////////////////////////////////////////////////

int square_v1(int x)
{
	return x*x;
}

// constexpr means calcute at compile time rather than run-time
constexpr int square_v2(int x)
{
	return x*x;
}

int main(int argc, char* argv[], char* env[])
{

/*
main.cpp:29:31: 错误：可变大小的对象‘ary1’不能被初始化
	  int ary1[square_v1(3)] = { 0 };
*/
	// int ary1[square_v1(3)] = { 0 };	
	int ary2[square_v2(3)] = { 0 };	// OK , since C++11 : ary2 has 9 elements

	// (void)ary1;
	(void)ary2;


	return 0;
}


