#include <stdio.h>
#include <stdlib.h> /* system function malloc(...)  /  free(...) required */



/*
    P72 Q8 :
          假设一个顺序表L中的所有元素为整数，设计一个算法调整该顺序表，使其中所有小于0的元素 放在 所有大于0的元素的 前面
*/




int main(int argc, char* argv[])
{
    /* 
         All the elements in the ary should be a positive number or negative number except 0
    */
    int ary[] = { 3, -1, 4, -5,  -5 , 4,6,  -1, -3, 4, 5, -4, 8 };
    int len = (sizeof(ary) / sizeof(ary[0]));
    for( int idx = 0; idx < len; ++idx) {
        printf("%d ", ary[idx]);
    }
    printf("\n");

    int i = 0;
    int j = len - 1;

    while ( i < j ) {
        while ( i < j &&  ary[j] > 0 )  { --j; }
        while ( i < j &&  ary[i] < 0 )  { ++i; }
        // if ( i < j ) {
            // swap
            int tmp = ary[j];
            ary[j] = ary[i];
            ary[i] = tmp;
        // }
    }

    for( int idx = 0; idx < len; ++idx) {
        printf("%d ", ary[idx]);
    }
    printf("\n");

    return 0;
}
