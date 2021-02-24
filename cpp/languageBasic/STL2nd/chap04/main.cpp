#include <iostream>
#include <future>
// #include <future>
#include <functional>
#include <memory>
using namespace std;

int orinaryFunc(int x, int y)
{
	cout << "1111-->222222 call orinaryFunc" << endl;
	return x + y;
}

auto l = [](int x, int y) -> int {
	return x - y;
};


class C {
public:
	C() : m_val(100) {
		cout << "C::created()\t" << this <<  endl;
	}

	virtual ~C() {
		cout << "C::delete()\t" << this <<  endl;
	}

	C(const C& other) : m_val(other.m_val) {
		cout << "C::copy()\t" << this << endl;
	}

	C(C&& rvalOther) : m_val(rvalOther.m_val) {
		cout << "C::move()\t" << this << endl;
	}

	int operator() (int x, int y) const
	{
		cout << "in operator() (x,y) "  << endl;
		return x * y;
	}

	int memfunc(int x, int y) const
	{
		cout << "in memfunc(x,y) "  << endl;
		return x / y;
	}
protected:
	int m_val;
};


int main(int argc, char* argv[], char* env[])
{
	C c;
	shared_ptr<C> sp(new C);

	//////////////////////////////////////////////////////////////////////
	//
	// !!! Core Core Core !!!
	//
	// function bind(...) is declared in header file <functional>
	//
	//
	//////////////////////////////////////////////////////////////////////
	
	// bind() use callable objects to bind arguments:
	cout << "----------------------------------------------------------------------" << endl;
	int ret1 = bind(orinaryFunc,77,33)();
	cout << "ret1 = " << ret1 << endl;
	cout << "----------------------------------------------------------------------" << endl;

	int ret2 = bind(l,77,33)();
	cout << "ret2 = " << ret2 << endl;
	cout << "----------------------------------------------------------------------" << endl;

	// 1. create a unnamed temp varible C()
	// 2. invoke move constructor when pass temp varible to formal arguments
	// 3. inside the bind call , call operator()(x,y)
	// 4. release the 2nd created object mentioned in step2
	// 5. release the 1st created object mentioned in step1
	int ret3 = bind(C(),77,33)();
	cout << "ret3 = " << ret3 << endl;
	cout << "----------------------------------------------------------------------" << endl;

	int ret4 = bind(&C::memfunc, c, 77, 33)();
	cout << "ret4 = " << ret4 << endl;
	cout << "----------------------------------------------------------------------" << endl;

	int ret5 = bind(&C::memfunc, sp, 77, 33)();	// sp->memfunc(77,33)
	cout << "ret5 = " << ret5 << endl;
	cout << "----------------------------------------------------------------------" << endl;

	cout << endl;
	cout << endl;
	cout << endl;

	//////////////////////////////////////////////////////////////////////
	//
	// !!! Core Core Core !!!
	//
	// function async(...) // is declared in header file <future>
	//
	//
	//////////////////////////////////////////////////////////////////////
	async(orinaryFunc, 77,33);
	async(l,77,33);
	async(c,77,33);
	async(&C::memfunc, &c, 77, 33);
	async(&C::memfunc, sp, 77, 33);


	return 0;
}

