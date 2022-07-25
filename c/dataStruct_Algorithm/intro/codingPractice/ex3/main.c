#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/**********************************************************************

求素数的个数

目的 : 通过对比同一问题不同解法的绝对执行时间体会如何设计"好"的算法
内容 : 编写一个程序 ， 求 1 ~ n 的素数个数。给出两种解法， 对于相同的n ，给出这两种解法的结果和求解时间，并用相关数据进行测试

***********************************************************************/


int prime1(long n)
{
    for( long i = 2; i < n; ++i ) {
        if ( n % i == 0 ) {
            return 0;
        }
    }
    return 1;
    
}


int prime2(long n)
{
    /*         must be <=                        */
    for( long i = 2; i <= ( (int)sqrt(n) ); ++i ) {
        if ( n % i == 0 ) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char* argv[])
{
    if ( argc != 2 ) {
        printf("[ERROR] : Missing argument, require 1 positive number as the argument \n");
        return -1;
    }

    int num = atoi( argv[1] );   // atoi(...) require header file  :    <stdlib.h>
    time_t t1,t2,t3,t4;
    int cnt1 = 0;
    int cnt2 = 0;

    t1 = clock();  // clock() require header file : time.h
    for( long i = 2; i <= num; ++i ) {
        if ( prime1( i ) ) {
            ++cnt1;
        }
    }
    t2 = clock();
    time_t delta1 = t2 - t1;
    double sec1 = delta1 * 1.0 / CLOCKS_PER_SEC;

    t3 = clock();
    for( long i = 2; i <= num; ++i ) {
        if ( prime2( i ) ) {
            ++cnt2;
        }
    }
    t4 = clock();
    time_t delta2 = t4 - t3;
    double sec2 = delta2 * 1.0 / CLOCKS_PER_SEC;

    printf("prime1(...) =>  1 ~ %d , %d prime(s),  time elapsed : %ld msec(s) = %f sec(s)\n",  num, cnt1, delta1, sec1 );
    printf("prime2(...) =>  1 ~ %d , %d prime(s),  time elapsed : %ld msec(s) = %f sec(s)\n",  num, cnt2, delta2, sec2 );
    
    return 0;
}


