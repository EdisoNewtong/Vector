#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/**********************************************************************

实验1 : 求 1 ~ n 的连续整数之和
目的  : 通过对比同一问题不同解法的绝对执行时间，体会不同算法的优劣

***********************************************************************/


/* 
     无脑，累计算法
*/
int sumBrainess(int lastNum)
{
    int sum = 0;
    for( int i = 1; i<=lastNum; ++i ) { sum += i; }
    return sum;
}

/* 
    高斯 :  等差数列求和 算法
*/
int sumGaussian(int lastNum)
{
    int sum = (1 + lastNum) * lastNum / 2;
    return sum;
}




int main(int argc, char* argv[])
{
    if ( argc != 2) {
        printf("[ERROR] : Missing argument, require 1 positive number as the argument \n");
        return -1;
    }

    int num = atoi( argv[1] );   // atoi(...) require header file  :    <stdlib.h>
    time_t t1,t2,t3,t4;

    t1 = clock();  // clock() require header file : time.h
    sumBrainess( num );
    t2 = clock();
    time_t delta1 = t2 - t1;
    double sec1 = delta1 * 1.0 / CLOCKS_PER_SEC;

    t3 = clock();
    sumGaussian( num );
    t4 = clock();
    time_t delta2 = t4 - t3;
    double sec2 = delta2 * 1.0 / CLOCKS_PER_SEC;

    printf("sumBrainess(%d) time elapsed : %ld msec(s) = %f sec(s)\n",  num,  delta1, sec1 );
    printf("sumGaussian(%d) time elapsed : %ld msec(s) = %f sec(s)\n", num,  delta2, sec2 );
    return 0;
}


