#include <iostream>
#include <string>
using namespace std;



class A
{
public:
	A(int a) : m_a(a) { 
		cout << "A::create(), m_a = " << m_a << "\t" << this << endl;
	}
	virtual ~A() {
		cout << "A::delete(), m_a = " << m_a << "\t" << this << endl;
	}

	A(const A& ra) : m_a(ra.m_a) {
		cout << "A::copy() , ra = " << ra.m_a << endl;
	}

	A(A&& ra) : m_a(ra.m_a) {
		cout << "A::move() , ra = " << ra.m_a << endl;
	}

	A& operator = (const A& lvalue) {
		cout << "A::assign Left-value(), la = " << lvalue.m_a << endl;
		if ( this == &lvalue ) {
			return *this;
		}

		this->m_a = lvalue.m_a;
		cout << "After Left assignment" << endl;
		return *this;
	}

	A& operator = (A&& rvalue) {
		cout << "A::assign Right-value(), ra = " << rvalue.m_a << endl;
		if ( this == &rvalue ) {
			return *this;
		}

		this->m_a = rvalue.m_a;
		cout << "After Right assignment" << endl;
		return *this;
	}

	A operator + (const A& rightValue) {
		cout << "2 A objects Added" << endl;
		A ret(*this);
		ret.m_a += rightValue.m_a;
		return ret;
	}

	int getA() const {
		return m_a;
	}

protected:
	int m_a;

};

class B
{
public:
	// void test1(A objA) {
	// 	cout << "B::test1() Object-Value , m_a = " << objA.getA() << endl;
	// }

	void test1(const A& objA) {
		cout << "B::test1() Left-Value , m_a = " << objA.getA() << endl;
	}

	void test1(A&& objA) {
		cout << "B::test1() Right-Value, m_a = " << objA.getA() << endl;
	}
};


/*

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

   Never return an rvalue reference if the return object is a local nonstatic object

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



A&& foo1()
{
	A retA(3);
	...
	return x;
}


A&& foo2()
{
	A retA(5);
	...
	return std::move(x);
}
*/




int main(int argc, char* argv[], char* env[])
{

	//
	// @ P23  :  Type&&				Right-Value Reference    Move Segmantics ( The Formal Parameter <Steal> the content of the passed real varible, and make the real passed varible unaccessible , the formal parameter is filled with the real varible's member-data  )
	//
	B objB;
	objB.test1( A(3) );	// temp object A(3) is Right-Value , and it will be destoried after the invoke of objB.test1( ... );

	cout << "==================================================" << endl;
	A objA1(5);
	A objA2(77);
	objA1 = objA2;				// call  Left-Value of assignment version , because exp objA2 is a named varible
	objA1 = A(4);				// call Right-Value of assignment version , because A(4) is temp unnamed exp
	objB.test1( objA1 );		// call  Left-Value version
	objB.test1( std::move(objA1) );	// call Right-Value version , because of the function named   std::move(...)
	objB.test1( objA1 + objA2 );	// call Right-Value version , because exp (objA1 +  objA2) is an unnamed exp

	cout << "==================================================" << endl;
	return 0;
}


