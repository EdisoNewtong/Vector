#include <iostream>
// #include <string>
// #include <wstring>
using namespace std;

//////////////////////////////////////////////////////////////////////
//
// @ P23 , New String Literals
//
//////////////////////////////////////////////////////////////////////

class A
{
public:
	A(int a) : m_a(a) {
	}

	virtual ~A() {
	}

	int getA() {
		return m_a;
	}
protected:
	int m_a;
};


void func1(A* pObj) {
	(void)pObj;
}






int main(int argc, char* argv[], char* env[])
{
	//----------------------------------------------------------------------------------------------------
	// @ P23 , New String Literals
	//----------------------------------------------------------------------------------------------------
	//
	// 2 new string literal style for raw string ( non-escaple version )
	//----------------------------------------------------------------------------------------------------
	//
	//		1. R"( ... )";
	//		2. R"delim( ... )delim";  // to enable   )"  inside the raw string literal , the delim can be any sequences of at most 16 characters except the backsplash, whitespace and parentheses
	//
	//----------------------------------------------------------------------------------------------------
	/********************************************************************************************************************************************
	 *
	 *  Raw String literals are especially useful when defining regular expressions  (RE)
	 *
	 *******************************************************************************************************************************************************************************************************************
	 */

	auto s1 = "\\\\n";
	auto s2 = R"(\\n)";
	cout << "s1 = \"" << s1 << "\"" << endl;
	cout << "s2 = \"" << s2 << "\"" << endl;

	auto s3 = "a\\\n          b\\nc()\"\n       ";
	auto s4 = R"nc(a\
          b\nc()"
       )nc";
	cout << "s3 = " << endl 
		 << "==================================================" << endl
		 << s3 << endl
		 << "==================================================" << endl
		 << endl;

	cout << "s4 = " << endl 
		 << "==================================================" << endl
		 << s4 << endl
		 << "==================================================" << endl
		 << endl;
	



	//
	//  Notes that the C++ comment inside the Raw String is treated as a common string rather than comment
	//
//----------------------------------------------------------------------------------------------------
// Start of the named varible "s5" defination
//----------------------------------------------------------------------------------------------------

	auto s5 = R"(aaa
// int aaa = 5;
   // int  bbb = 7;
       /*
  int func_add(int a, int b) 
  {
    /*
	return a + b;
    */
  }
*/  bbb)";

//----------------------------------------------------------------------------------------------------
// End of the named varible "s5" defination
//----------------------------------------------------------------------------------------------------
	cout << "s5 = \"" << s5 << "\"" << endl;


	//////////////////////////////////////////////////////////////////////
	//
	// Encoding
	//
	//////////////////////////////////////////////////////////////////////
	/*
	auto s6 = u8"我们";	// s6's type is   const char*
	auto s7 = u"你们";	// s7's type is   const char16_t*  with small u
	auto s8 = U"他们";	// s8's type is   const char32_6*  with capital U
	auto s9 = L"她们";	// s9's type is   const wchar_t* 

	func1(s6);	// error :  can't cast from     const char*   -->     A*
	func1(s7);  // error :  can't cast from     const char16_t*   -->     A*
	func1(s8);	// error :  can't cast from     const char32_t*   -->     A*
	func1(s9);	// error :  can't cast from     const wchar_t*   -->     A*
	*/


	return 0;
}


