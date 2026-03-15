#include <iostream>
#include "my_pow.h"

int util_Pow(int a,int b)
{
    int ret = my_pow(a,b);
    std::cout << "Inside util_Pow(a,b) : result = " << ret << std::endl;
    return ret;
}


