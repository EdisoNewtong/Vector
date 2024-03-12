import <iostream>;
import <string>;

import person;

using namespace std;

int main(int argc, char* argv[], char* env[])
{
    MyPerson* p1 = new MyPerson{  "Edison", "Tomas" };
    cout << "p1.name is \"" << p1->getLastName() << " " << p1->getFirstName() <<  "\" . " << endl;

    delete p1; p1 = nullptr;

    return 0;
}

