#include <iostream>
#include <vector>
#include <string>
using namespace std;


class AAA
{
protected:
	string* pStr;
	char buf[1024];
};

void my_func(AAA* pstr) {
	(void)pstr;
}


int main(int argc, char* argv[], char* env[])
{
	//
	// Range-Based for Loops
	//
	/*
	for ( decl : coll ) {
		statement
	}
	*/


	/*
	   for ( auto _pos = begin(coll), _end=end(coll); _pos!=end; ++_pos ) {
			decl = *pos;
			statement
	   }
	*/
	int i = 0;
	int* pi = &i;
	pi += 0;
	int ary1d[] = { 1,3,5 };
	for ( int ele : ary1d ) {
		cout << ++i << ". " << ele << endl;
	}
	cout << "==================================================" << endl;


	/*
	 *
	 *
	 *

	    注意了，2维 ~ n维的数组，要进行range-base for循环遍历时，除了最内层的1维数组不要用 auto& 
		其它都要用 auto& 

	 *
	 *
	 *
	 */
	int ary2d[5][3] = {
		{ 1,3,5 },
		{ 2,4,6 },
		{ 3,5,7 },
		{ 4,6,8 },
		{ 5,7,9 }
	};

	// i = 0;
	// for ( auto e1Val : ary2d ) {
	// 	 e1Val + pi; // e1Val ==>     ary2d[i] ==> &ary2d[i][0]  ==>   int*
	// }
	//
	
	// auto var1 = ary2d[0];
	// auto& var2 = ary2d[0];
	// my_func(var1);
	// my_func(var2);

	i = 0;
	for ( auto& e1Ref : ary2d ) {
		// e1Ref + pi;	
		for ( auto ele : e1Ref ) {	// e1Ref ==>  *(ary2d[i]) ==> int ary1d[3]
			cout << ++i << ". " << ele << endl;
		}
	}

	int ary3d[2][3][4] = {
		{
			{ 1,2,3,4 },
			{ 2,3,4,5 },
			{ 3,4,5,6 }
		},
		{
			{ 4,5,6,7 },
			{ 5,6,7,8 },
			{ 6,7,8,9 }
		},
	};


	cout << "==================================================" << endl;
	i = 0;
	for ( auto& ele2d : ary3d ) {
		for ( auto& ele1d : ele2d ) {
			for ( auto ele : ele1d ) {
				cout << ++i << ". " << ele << endl;
			}
		}
	}


	/*
	   for ( auto _pos = coll.begin(), _end = coll.end(); _pos != end; ++pos ) {
			decl = *pos;
			statement
	   }
	*/

	cout << "==================================================" << endl;
	i = 0;
	vector<int> vec{1,11,111};
	for ( auto ele : vec ) {
		cout << ++i << ". " << ele << endl;
	}

	
	return 0;
}

