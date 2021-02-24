#include <iostream>
#include <vector>
using namespace std;


//////////////////////////////////////////////////////////////////////
//
// keyword : noexcept
//
//////////////////////////////////////////////////////////////////////



int getIdxNumber_normal(vector<int>& vec,int idx)
{
	return vec.at(idx);
}


// keyword : noexcept  is provided since c++11 
// It can be used to specify that a function cannot throw -- or is not prepared to throw
int getIdxNumber_withKeyWord(vector<int>& vec,int idx) noexcept
{
	return vec.at(idx);
}


int main(int argc, char* argv[], char* env[])
{
	if ( argc != 2 ) {
		cerr << "You must add 1 arg whose value is 1 or 2 " << endl;
		return -1;
	}

	// vec has only 3 elements, so its index range from [0,2]
	// index 3 is invalid
	vector<int> vec { 1,2,3 };

	int choice = atoi(argv[1]);
	if ( choice == 1 ) {
		try {
			// 3 is an invalid idx for vector<int> vec;
			getIdxNumber_normal( vec, 3 );
		}
		catch ( ... ) {
			cout << "Catch exception @ getIdxNumber_normal(3) " << endl;
		}
	} else if ( choice == 2 )  {
		try {
			// 3 is an invalid idx for vector<int> vec;
			// But , try-catch block make no sense 
			getIdxNumber_withKeyWord( vec, 3 );
		}
		catch ( ... ) {
			cout << "Catch exception @ getIdxNumber_normal(3) " << endl;
		}
	} else {
		cerr << "arg[1] : " << choice << " is invalid,  whose value must be 1 or 2 " << endl;
	}



	return 0;
}


