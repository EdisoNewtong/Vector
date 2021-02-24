#include <iostream>
#include <string>
#include <vector>
#include <memory>
using namespace std;











//
// P77-78 , The first glance at   shared_ptr<T> ...
//
void case1()
{
	/*



	   shared_ptr<T> varName( new T(...) ); // OK
	   shared_ptr<T> varName = make_shared<T>(args ...); // OK
	   e.g. 
			shared_ptr<Person> varName = make_shared<Person>(string("Tom"), 10); // OK


	   shared_ptr<T> varName = new T();  // Compile Error , int*’转换到非标量类型‘std::shared_ptr<int>’ 

	   e.g. 
			shared_ptr<int> pNumber = new int(10); // Compile Error , int*’转换到非标量类型‘std::shared_ptr<int>’ 



		Summary of the construct of an shared_ptr<T>


		shared_ptr<T> ptr;		// ptr's value is  nullptr
		shared_ptr<T> ptr(T*);	// ptr's value is  nullptr
		shared_ptr<T> ptr(nullptr);

		shared_ptr<T> ptr(sp2);
		shared_ptr<T> ptr( move(sp2) ); // sp2 is empty afterward


	*/

	
	// shared_ptr<Person> varName = make_shared<Person>(string("Tom"), 10); // OK


	// two shared pointers representing two persons by their name
	shared_ptr<string> pNico(new string("nico"));   // pNico reference-count  = 0+1 = 1
	shared_ptr<string> pJutta(new string("jutta")); // pJutta reference-count = 0+1 = 1

	// capitalize persion names
	(*pNico)[0] = 'N';
	pJutta->replace(0,1,"J");

	// put them muliple times in a container
	vector<shared_ptr<string>> whoMadeCoffee;
	whoMadeCoffee.push_back(pJutta); // pJutta.rc  += 1   => 2
	whoMadeCoffee.push_back(pJutta); // pJutta.rc  += 1   => 3
	whoMadeCoffee.push_back(pNico);  // pNico.rc += 1     => 2
	whoMadeCoffee.push_back(pJutta); // pJutta.rc += 1    => 4
	whoMadeCoffee.push_back(pNico);  // pNico.rc += 1     => 3  

	// print all elements
	for ( auto ptr : whoMadeCoffee ) {
		cout << *ptr << " ";
	}
	cout << endl;

	// override a name again
	*pNico = "Nicolai";


	// print all elements again
	for ( auto ptr : whoMadeCoffee ) {
		cout << *ptr << " ";
	}
	cout << endl;

	cout << "use_count of element[0](pJutta) = " << whoMadeCoffee[0].use_count() << endl; // pJutta
	cout << "use_count of element[2](pNico)  = " << whoMadeCoffee[2].use_count() << endl; // pNico
}


class Person {
public:
	Person(const string& name, int age) : m_name(name), m_age(age) { cout<< "Person create " << endl; }
	virtual ~Person() {  cout << "Person Delete" << endl; }
	void pt() {
		cout << "\tp.name = " <<  m_name << endl;
		cout << "\tp.age = "  <<  m_age << endl;
	}
protected:
	string m_name;
	int    m_age;
};

void case2()
{
   shared_ptr<Person> p1 = make_shared<Person>(string("Tom"), 10);
   p1->pt();
   cout << "p1.rc = " << p1.use_count() << endl;



   // Core Code , by move-semantic
   // thinking the deference between    move(p1)   and   without move version
   //      shared_ptr<Person> p2(move(p1) );  // after that p1 has been no longer use
   //      shared_ptr<Person> p2(     p1 );   // p1 is valid if without function
   shared_ptr<Person> p2(move(p1) ); // thinking the deference between    move(p1)   and   p1(
   // shared_ptr<Person> p2(p1 );
   cout << "After move-construct , p1.rc = " << p1.use_count() << endl;
   cout << "p2.rc = " << p2.use_count() << endl;


   //
   if ( p1 ) {
		cout << "now p1 is valid" << endl;
		cout << "p1->call()" << endl;
		p1->pt();
   } else {
	   cout << "!!! Sorry p1 has been no longer use !!!" << endl;
   }

   cout << "p2->call()" << endl;
   p2->pt();
}


int main(int argc, char* argv[], char* env[])
{
	// case1();
	
	case2();
	cout << "After case2() ==== " << endl;


	return 0;
}
