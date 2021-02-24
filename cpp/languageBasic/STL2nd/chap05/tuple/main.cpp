#include <iostream>
#include <string>
#include <tuple>	// for use tuple<T1,T2,T3, ... , TN>
#include <functional>	// for use   ref(...)
#include <complex>	// for use   ref(...)
using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Help Function for print arbitrary

template<int IDX,int MAX, typename... Args>
struct PRINT_TUPLE {
	static void print(ostream& strm, const tuple<Args...>& t) {
		strm << get<IDX>(t) << (IDX+1==MAX ? "" : " , ");
		PRINT_TUPLE<IDX+1,MAX,Args...>::print(strm,t);
	}
};


// partial specialization to end the recursion
// Recursive condition End
template<int MAX, typename... Args>
struct PRINT_TUPLE<MAX,MAX,Args...> {
	static void print(ostream& strm, const tuple<Args...>& t) {
		// strm << " [END] " << endl;
	}
};


// output operator for tuples
template<typename... Args>
ostream& operator << (ostream& strm, const tuple<Args...>& t)
{
	strm << " [ ";
	PRINT_TUPLE<0, sizeof...(Args), Args...>::print(strm,t);
	return strm << " ] ";
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int main(int argc, char* argv[], char* env[])
{
	tuple<string,int,int,complex<double>> t;

	tuple<int,float,string> t1(41,6.3,"nice");

	cout << "Before : " << endl;
	cout << get<0>(t1) << " ";
	cout << get<1>(t1) << " ";	// yield 6.3
	cout << get<2>(t1) << " ";
	cout << endl;

	// t2 ==> std::tuple<int, int, const char*>
	auto t2 = make_tuple(22,44,"nice");
	// float 6.3f <=  int 44
	get<1>(t1) = get<1>(t2);

	cout << "After : " << endl;
	cout << get<0>(t1) << " ";
	cout << get<1>(t1) << " ";	// now the line yield 44
	cout << get<2>(t1) << " ";
	cout << endl;

	// cout << "====================================================================================================" << endl;
	cout << "====================================================================================================" << endl << endl;

	string s1("aaa");
	tuple<string&> t3(s1);	// 1 size  tuple , first element refers to s
	get<0>(t3) = "bbb";	// assign "bbb" to s
	cout << "s1 = " << s1 << endl;


	
	// for ( auto i = 0; i < 3; ++i ) {
	//		Compile-Error : get<Idx>(tuple_varible) , Idx must be a constexpr at compile time
	// 	cout << i << ". " << get<i>(t1) << endl;
	// }
	
	const auto IDX1 = 0;
	cout << "element0 = " << get<IDX1>(t1) << endl;	// Compile-Successful   yield 41
	constexpr auto IDX2 = IDX1 + 2; 
	cout << "element2 = " << get<IDX2>(t1) << endl;	// Compile-Successful   yield "nice"


	cout << "====================================================================================================" << endl << endl;
	string ss = "aaa";
	auto x = make_tuple(ss);	// x is type tuple<string>
	get<0>(x) = "my value";	// modifies x but not s
	cout << "1. ss = " << ss << endl;	// yield "aaa"

	auto y = make_tuple( ref(ss) );	// y is of type tuple<string&>, thus y refers to s
	get<0>(y) = "my value";	// modifies ss via y
	cout << "2. ss = " << ss << endl;	// yield "my value"


	cout << "====================================================================================================" << endl << endl;
	tuple<int,float,string> t4(77,1.1,"more light");
	int ii;
	float ff;
	string strret;
	make_tuple( ref(ii), ref(ff), ref(strret) ) = t4;
	cout << "ii = " << ii << endl;	// 77
	cout << "ff = " << ff << endl;	// 1.1
	cout << "strret = " << strret << endl;	// more light

	ii += 3;
	cout << "get<0>(t4) = " << get<0>(t4)  << endl;	// yield 77 no 80

	cout << "====================================================================================================" << endl << endl;
	tie(ii,ff,strret) = t4;
	cout << "ii = " << ii << endl;	// 77
	cout << "ff = " << ff << endl;	// 1.1
	cout << "strret = " << strret << endl;	// more light


	// Additional Tuple Features
	// tuple<string,int,int,complex<double>> t;
	cout << "tuple<string,int,int,complex<double>>.size = " << tuple_size< decltype(t)>::value << endl;	// yield 4
	tuple_element<1, decltype(t)>::type num = 123;
	cout << "int num = " << num << endl;

	double dnum = 5.7;
	//                                                 tie(var) ==> create a lvalue reference of a varible
	//          连接多个tuple , 组合成一个完整的tuple
	auto t5 = tuple_cat( make_tuple(42, 7.7, "hello"), tie(num), tie(dnum) );
	cout << "Before | get<4>(t5) = " << get<4>(t5) << " | "<< dnum << endl;	// dnum = 5.7
	get<4>(t5) = 6.8;	// set dnum via exp get<4>(t5)
	cout << "After  | get<4>(t5) = " << get<4>(t5) << " | "<< dnum << endl;	// !!!Notes!!! dnum changes from 5.7 to 6.8

	// io : [ 42 , 77 ,  hello , 123 , 6.8 ]
	cout << "io : " << t5 << endl;


	return 0;
}
