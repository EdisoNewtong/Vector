#include <iostream>
using namespace std;


// it is a header file which is written by C language , 
// so if c++ program require to include this header file
// extern "C" is a must  modifiers 
// and it also generate a shared libary  called  "libMyAdd.so"
#include "myAdd.hpp"
extern int my_add(int a,int b);


int main(int argc, char* argv[])
{
	int ret = my_add(3, 4);
	cout << "3 + 4 = " << ret << endl;
	return 0;
}



