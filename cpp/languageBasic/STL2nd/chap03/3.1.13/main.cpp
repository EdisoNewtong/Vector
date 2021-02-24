#include <iostream>
using namespace std;


//////////////////////////////////////////////////////////////////////
//
// Scoped Enumerations  :   enum class XXXNames : int-Like-Type
//
//////////////////////////////////////////////////////////////////////
/*
 * C++11 allows the defination of scoped enumerations -- also called strong enumerations, or enumeration classes
 * which are a cleaner implemetation of enumeration values(enumerations) in C++.
 * e.g.
 *
 *		enum class Salutation : char { mr, ms, co, none };
*/


enum DaysNonCpp11
{
	E_Mon = 1,
	E_Tues = 2,
	E_Wednes = 3,
	E_Thurs = 4,
	E_Fri = 5,
	E_Satur = 6,
	E_Sun = 7
};


// enum  class  Enumeration-Name   :   Type ( if type is not specified , int is the default , )
// Type can be   (unsigned) char , (unsigned) short , (unsigned) int , (unsigned) long , (unsigned) long long
enum class DaysCpp11 : int
{
	E11_Mon = 1,
	E11_Tue = 2,
	E11_Wed = 3,
	E11_Thur = 4,
	E11_Fri = 5,
	E11_Sat = 6,
	E11_Sun = 7
};


int main(int argc, char* argv[], char* env[])
{
	string full_days[] = {
		"Unknown",		// 0
		"Monday",		// 1
		"Tuesday",		// 2
		"Wednesday",	// 3
		"Thursday",		// 4
		"Friday",		// 5
		"Saturday",		// 6
		"Sunday"		// 7
	};

	cout << full_days[E_Mon]   << endl;
	/* 
	 * main.cpp: 在函数‘int main(int, char**, char**)’中:
	 * main.cpp:62:20: 错误：‘E11_Mon’在此作用域中尚未声明
	 *   cout << full_days[E11_Mon] << endl;

		1. Implicit conversion to and from int are not possible
		2. Values like E11_Mon are not part of the scope where the enumeration id declared. You have to use DaysCpp11::E11_Mon
		3. You can explicitly define the underlying type (char here) and have a guaranteed size (if you skip " : char" here,  int is the default 
		4. forward declaration of the enumeration type are possible

	*/
	

	// cout << full_days[E11_Mon] << endl;						// Compile Error 
	// cout << full_days[static_cast<size_t>(E11_Mon)] << endl;	// Compile Error
	// cout << full_days[DaysCpp11::E11_Mon] << endl;			// Compile Error
	cout << full_days[static_cast<size_t>(DaysCpp11::E11_Mon)] << endl;	// Compile OK

	return 0;
}


