#include <iostream>
#include <string>
#include <limits>

using namespace std;


int main(int argc, char* argv[], char* env[])
{
	// numeric_limits<int>::
	// judge whether char is  unsigned or signed
	cout << "numeric_limits<char>::is_signed = " << (numeric_limits<char>::is_signed ? "true" : "false") << endl;
	cout << endl;

	cout << "numeric_limits<char>::is_integer = " << (numeric_limits<char>::is_integer ? "true" : "false") << endl;
	cout << "numeric_limits<float>::is_integer = " << (numeric_limits<float>::is_integer ? "true" : "false") << endl;
	cout << endl;

	cout << "numeric_limits<unsigned long long>::min() = " << numeric_limits<unsigned long long>::min() << endl;
	cout << "numeric_limits<unsigned long long>::max() = " << numeric_limits<unsigned long long>::max() << endl;
	cout << endl;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// 1. char             | unsigned char              | signed char
	// 2. short            | unsigned short             | signed short
	// 3. int              | unsigned int               | signed int
	// 4. long int         | unsigned long int          | signed long int
	// 5. long long int    | unsigned long long int     | signed long long int
	// 6. float    
	// 7. double    
	// 8. long double    
	//
	//////////////////////////////////////////////////////////////
	//
	//			string is not a numeric type , only  
	//
	//////////////////////////////////////////////////////////////
	cout << "numeric_limits<string>::is_specialized = " << (numeric_limits<string>::is_specialized  ? "true" : "false") << endl;



	return 0;
}
