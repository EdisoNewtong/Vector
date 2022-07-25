#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**********************************************************************

求连续整数阶乘的和

目的 : 体会如何设计"好"的算法
内容 : 编写一个程序， 对于给定的正整数 n, 求 1! + 2! + 3! + 4! + ... + n! , 给出一种时间复杂度为 O(n) 的解法

***********************************************************************/


unsigned long long sum_factorial(int n)
{
    unsigned long long sum = 0ull;
    unsigned long long step = 1ull;
    for( int i = 1; i <= n; ++i )
    {
        step *= i;
        sum += step;
    }

    return sum;
}


int main(int argc, char* argv[])
{
    if ( argc != 2 ) {
        printf("[ERROR] : Missing argument, require 1 positive number as the argument \n");
        return -1;
    }

    int n = atoi( argv[1] );   // atoi(...) require header file  :    <stdlib.h>
    unsigned long long sum = sum_factorial(n);
    printf("n = %d\n",n);
    printf("1! + 2! + 3! + 4! + ... + n! = %llu\n", sum);

    return 0;
}



