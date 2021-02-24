#include <iostream>
#include <string>
#include <functional>
using namespace std;

//////////////////////////////////////////////////////////////////////
//
// C++11  Lambda  (Core Core Core)
// 
//////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////
//
// Type of Lambdas
//
//////////////////////////////////////////////////////////////////////
//
// Type  function<int(int,int)>
//
// must  include   header file <functional>
//
function<int(int,int)> returnLambda()
{
	return [](int x, int y) {
		return (x * y);
	};
}

int main(int argc, char* argv[], char* env[])
{

	// sample lambda invoke immediately without a named varible
	[] {
		cout << "In anonymous function()" << endl;
	} ();

	/* 
	  if a Lambda has 0 args, the arguments list (...) can be ommited

	    The complete code is like the following code
		e.g.
			auto funcSample = []() {
				cout << "In funcSample()" << endl;
			};
	*/
	auto funcSample1 = [] {
		cout << "In funcSample-1()" << endl;
	};
	funcSample1();

	auto funcSample2 = [] {
		cout << "In funcSample-2()" << endl;
	};
	funcSample2();

	/*
	 
	 lambda defination style
	 1. 
		[...] { ... }
	 2. 
		[...] (...) mutable(opt)  throwSpec(opt)   ->retType(opt) { ... }

		[=] captures all below varibles by values
		[&] captures all below varibles by references


		[a,&b] ==> varible "a" by value , varible "b" by references
	*/

	int x = 0;
	int y = 42;
	auto qqq = [x,&y] {
		cout << "x = " << x << endl;
		cout << "y = " << y << endl;
		// ++x;  // Compile Error     错误：令只读变量‘x’自增
		++y;	// OK
	};

	x = y = 77;
	qqq();
	// x = 0
	// y = 77

	qqq();
	// x = 0
	// y = 78
	
	cout << "finally y : " << y << endl;
	// finally y : 79

	cout << "====================================================================================================" << endl;

	int id = 0;
	auto f = [id] () mutable {
		cout << "id = " << id << endl;
		++id;	// OK because of the keyword mutable
	};

	id = 42;

	f();
	// id = 0
	f();
	// id = 1
	f();
	// id = 2

	cout << "finally id = " << id << endl;
	// finally id = 42

	cout << "====================================================================================================" << endl;
	auto lf = returnLambda();
	cout << lf(6,7) << endl;

	return 0;
}


