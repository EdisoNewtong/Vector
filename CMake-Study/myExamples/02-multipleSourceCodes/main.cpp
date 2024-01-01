#include <iostream>
using namespace std;

#include "AAA.h"

int main(int argc, char* argv[])
{
    const double number = 144;
    AAA* pObject = new AAA(number);
    cout << "number : " << number << " 's square root is " <<  pObject->getSquareRoot() << endl;
    delete pObject; pObject = nullptr; 

    return 0;
}


