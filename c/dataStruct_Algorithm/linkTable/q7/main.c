#include <stdio.h>
#include <stdlib.h> /* system function malloc(...)  /  free(...) required */

#define MAX_SIZE 100

typedef int DataType;

/*
    P72 Q7 :
          设计一个算法，将元素 x 插入到一个有序(从小到大排序)顺序表的适当位置，并保持有序性
*/

typedef struct SeqTable
{
    DataType ary[MAX_SIZE];
    int      length;
} SeqTable;

void insertElement(SeqTable* pTb, int x)
{
    if ( pTb == NULL ) {
        return;
    }

    int insertIdx = 0;
    while ( insertIdx < pTb->length   &&   pTb->ary[insertIdx] < x ) { 
        ++insertIdx; 
    }

    for( int i = pTb->length-1; i >= insertIdx; --i ) {
        pTb->ary[i+1] = pTb->ary[i];
    }
    pTb->ary[insertIdx] = x;
    ++pTb->length;
}


int main(int argc, char* argv[])
{
    if ( argc != 2 ) {
        printf("[ERROR] : missing args\n");
        return -1;
    }


    SeqTable tb;
    int i;
    for( i = 0; i < 9; ++i )
    {
        if ( i <= 3 ) {
            tb.ary[i] = (i+1);
        } else {
            tb.ary[i] = (i+2);
        }
    }
    tb.length = i;

    printf("Original Array : ");
    printf("\t");
    for( int idx = 0; idx < tb.length; ++idx ) {
        printf("%d ", tb.ary[idx]);
    }
    printf("\n");

    insertElement(&tb, atoi(argv[1]) );
    printf("AfterInsert Array : ");
    printf("\t");
    for( int idx = 0; idx < tb.length; ++idx ) {
        printf("%d ", tb.ary[idx]);
    }
    printf("\n");

    return 0;
}
