#include <iostream>
#include "core/util.h"
#include "math/my_pow.h"
using namespace std;

int main(int argc, char* argv[], char* env[])
{
    if ( argc < 3 ) {
        cout << "[ERROR] At least 2 args are required but missed ! " << endl;
        return -1;
    }

    int n_a = atoi(argv[1]);
    int n_b = atoi(argv[2]);
    int ret = util_Pow(n_a, n_b);
    cout << "inside main function , the return value for util_Pow(a,b) = " << ret << endl;

    n_a = 2;
    n_b = 10;
    ret = my_pow(n_a, n_b); // 1024
    cout << "the return value for util_Pow( " << n_a << "," << n_b << " ) = " << ret << endl;

    return 0;
}




