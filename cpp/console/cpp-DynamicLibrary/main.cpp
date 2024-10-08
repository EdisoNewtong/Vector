#include <iostream>
#include "util.h"
using namespace std;

int main(int argc, char* argv[], char* env[])
{
    Util u(2);
    auto sin = 1;
    auto cos = 100;
    auto sum = u.calculateSum(sin,cos);
    cout << "Sum from " << sin << " ~ " << cos << " = " << sum << endl;

    return 0;
}


