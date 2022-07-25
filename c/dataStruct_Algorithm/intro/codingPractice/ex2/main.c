#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**********************************************************************

  常见算法时间函数的增长趋势分析
  目的 : 理解常见算法的时间函数的增长情况

  log2(n)  ,  sqrt(n) , n, nlog2(n), n^2,  n^3,  2^n,  n!

  对于 1 ~ n 的每一个整数n, 输出 以上表达式的值

***********************************************************************/


float al_log2(int n)
{
    return log2f(n);
}

float al_sqrt(int n)
{
    return sqrt(n);
}

float al_n(int n)
{
    return (float)n;
}

float al_nlog2(int n)
{
    return n * al_log2(n);
}


float al_n2(int n)
{
    return (float)(n * n);
}

float al_n3(int n)
{
    return (float)(n * n * n);
}

float al_2pown(int n)
{
    return powf( 2.0f, (float)n );
}

float al_factorial(int n)
{
    unsigned long long res = 1;
    for( int i = 1; i <= n; ++i ) {
        res *= (unsigned long long)i;
    }
    return (float)res;
}




int main(int argc, char* argv[])
{
    if ( argc != 2 ) {
        printf("[ERROR] : Missing argument, require 1 positive number as the argument \n");
        return -1;
    }

    int n = atoi( argv[1] );   // atoi(...) require header file  :    <stdlib.h>
    printf("====================================================================================================\n");
    printf("n   log2(n)    sqrt(n)    nlog2(n)    n^2   n^3   2^n    n!\n");
    for( int i = 1; i<=n; ++i ) {
        printf("%2d   %.2f       %.2f       %.2f        %.2f  %.2f  %.2f  %.2f\n", i, al_log2(i), al_sqrt(i), al_nlog2(i), al_n2(i), al_n3(i), al_2pown(i), al_factorial(i) );
    }
    printf("====================================================================================================\n");

    return 0;
}


