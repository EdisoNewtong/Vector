#include <iostream>
#include <utility>	// for use pair<T1,T2>
#include <tuple>	// for use tuple<T1,T2,T3, ... , TN>
#include <functional>	// for use   ref(...)
using namespace std;


class Foo {
public:
	Foo(tuple<int,float>) {
		cout << "Foo::Foo(tuple)" << endl;
	}

	template<typename... Args>
	Foo(Args... args) {
		cout << "Foo::Foo(args...)" << endl;
	}
};

int main(int argc, char* argv[], char* env[])
{
	pair<int,float> p(1,3.14f);
	cout << "1st element = "  << get<0>(p) << endl;
	cout << "2nd element = "  << get<1>(p) << endl;

	// get the tuple element's size
	cout << "tuple_size of p = " << tuple_size< decltype(p) >::value << endl;

	// tuple_element<elementIdx, tupleType>::type   ==> get the nth varible type of tupleType
	// tuple_element<0, tupleType>::type   ==> int
	// tuple_element<1, tupleType>::type   ==> float
	tuple_element<0, decltype(p) >::type  int_varible =  123;
	tuple_element<1, decltype(p) >::type  float_varible =  444.5f;
	cout << "int_varible = " << int_varible << endl;
	cout << "float_varible = " << float_varible << endl;

	cout << "====================================================================================================" << endl;

	tuple<int,float> t(1,2.22f);
	pair<int,Foo> p1(42,t);	// call constructor of Foo(tuple)

	// template<typename... Args1, typename... Args2>
	// pair(piecewise_construct, 
	//	    tuple<Args1...> first_args,
	//	    tuple<Args2...> second_args);
	pair<int,Foo> p2(piecewise_construct, make_tuple(42), t);	// call template constructor of Foo(Args... args)


	cout << "====================================================================================================" << endl;

	// convenience Function make_pair(...)
	auto p3 = make_pair(42,'@');	// pair<int,char> p = make_pair(42,'@');
	(void)p3;

	int i = 0;
	// ref is declared in  header file <functional>
	auto p4 = make_pair( ref(i), ref(i) );	// create  pair<int&,int&>
	++p4.first;
	++p4.first;

	cout << "i = " << i << endl;
	cout << "p4.first = "  << p4.first  << endl;	// first is 2
	cout << "p4.second = " << p4.second  << endl;	// second is always the same value of p4.first

	auto p5 = make_pair('x','y');
	char ch1 = 0;
	char ch2 = 0;
	// tie is declared in header file <tuple>
	tie(ch1, ch2) = p5;	
	cout << "Old | ch1 = " << ch1 << endl;	// ch1 = x
	cout << "Old | ch2 = " << ch2 << endl;	// ch2 = y

	p5.second = '@';
	// extract second value into c ( ignore the first one)
	// tie && ignore is declared in header file <tuple>
	tie(ignore, ch2) = p5;	// set new second value to   ch2
	cout << "New | ch1 = " << ch1 << endl;	// ch1 = x
	cout << "New | ch2 = " << ch2 << endl;	// ch2 = @


	return 0;
}
