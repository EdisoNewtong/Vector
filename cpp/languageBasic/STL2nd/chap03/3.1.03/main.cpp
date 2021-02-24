#include <iostream>
#include <vector>
using namespace std;


int main(int argc, char* argv[], char* env[])
{
	/*
	    C++11 之前，变量初始化有很多种表达式
		class ClassPerson {
		public:
			ClassPerson(const std::string& n,int a) : name(n), age(a) { }
		protected:
			std::string name;
			int         age;
		};

		struct structPerson {
			std::string name;
			int         age;
		};

	    圆括号  : ClassPerson cp1("Tom",23);  
		花括号  : structPerson sp1 = { "Tom", "23" };
		=  赋值 : int x = 5;





		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			C++11开始，可以用统一的 {}  花括号进行初始化

		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		e.g.
			int values[] { 1,2,3 };
			std::vector<int>  v { 1,3,5,7,9 };
			std::vector<string>  cities {
				"Berlin", "New York", "London", "Braunschweig", "Cairo", Cologne"
			};


			int i{};	// the same meaning as   int i = 0;
			int* p{};	// the same meaning as   int* p = nullptr;

			ClassPerson cp1{"Tom",23};  // ==>  ClassPerson cp1("Tom",23)

	*/

	
	return 0;
}

