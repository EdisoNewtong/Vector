import person;

import <iostream>;
import <string>;


using namespace std;

int main(int argc, char* argv[], char* env[])
{
    MyPerson* p1 = new MyPerson{  "Edison", "Tomas" };
    cout << "p1.name is \"" << p1->getLastName() << " " << p1->getFirstName() <<  "\" . " << endl;

    MyPerson* p2 = new MyPersonWithAge{  "Newton", "Issac",  12 };
    cout << "p2.name is \"" << p2->getLastName() << " " << p2->getFirstName() <<  "\" , age = " << dynamic_cast<MyPersonWithAge*>(p2)->getAge() << endl;

    /*
        main.cpp: In function ‘int main(int, char**, char**)’:
        main.cpp:37:12: 错误：对非静态成员函数‘virtual MyPersonWithAge@person::~MyPersonWithAge()’的使用无效
           37 |     delete p2;
              |            ^~
        In module person, imported at main.cpp:1:
        personDeclear.cpp:26:14: 附注：在此声明
           26 | export class MyPersonWithAge : public MyPerson
              |              ^~~~~~~~~~~~~~~
        main.cpp:37：不能从先前的错误中恢复，退出
        make: *** [Makefile:65：main.o] 错误 1
    */
    // MyPersonWithAge* p2 = new MyPersonWithAge{  "Newton", "Issac",  12 };
    // cout << "p2.name is \"" << p2->getLastName() << " " << p2->getFirstName() <<  "\" , age = " << p2->getAge() << endl;

    delete p1; 
    p1 = nullptr;

    delete p2; 
    p2 = nullptr;

    return 0;
}

