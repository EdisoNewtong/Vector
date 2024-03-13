import math;

import <iostream>;
using namespace std;

int main()
{
	auto a{ Math::lerchZeta(1, 2, 3) };
	auto b{ Math::superLog(1, 2) };
    cout << "a = " << a << endl;
    cout << "b = " << b << endl;
    return 0;
}


