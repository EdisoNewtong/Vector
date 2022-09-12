#include "common.h"
#include "sortUtil3.h"


namespace v3
{

void quickSortHead_AscendOrder(int ary[], int begIdx, int endIdx, int arySz)
{
    if (  begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = begIdx; // set pivot number as the head number
    int pivotNum = ary[pivotIdx]; 

    int i = begIdx;
    for( int j = begIdx+1; j <= endIdx; ++j )
    {
        if ( ary[j] < pivotNum ) {
            ++i;
            if ( i != j ) {
                swap2Elements(ary, i,j);
            }
        }
    }
    swap2Elements(ary, i , pivotIdx);

    quickSortHead_AscendOrder(ary, begIdx, i-1, arySz);   // quickSort left  part
    quickSortHead_AscendOrder(ary, i+1, endIdx, arySz);   // quickSort right part
}

void quickSortHead_AscendOrder_Safe(int ary[], int begIdx, int endIdx, int arySz)
{
    if (  begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = begIdx; // set pivot number as the head number
    int pivotNum = getElementSafely(ary, pivotIdx, arySz); // ary[pivotIdx]; 

    int i = begIdx;
    for( int j = begIdx+1; j <= endIdx; ++j )
    {
        if ( getElementSafely(ary, j, arySz) < pivotNum ) {
            ++i;
            if ( i != j ) {
                swap2Elements_Safe(ary, i,j, arySz);
            }
        }
    }
    swap2Elements_Safe(ary, i , pivotIdx, arySz);

	quickSortHead_AscendOrder_Safe(ary, begIdx, i-1, arySz);   // quickSort left  part
    quickSortHead_AscendOrder_Safe(ary, i+1, endIdx, arySz);   // quickSort right part
}




void quickSortHead_DescendOrder(int ary[], int begIdx, int endIdx, int arySz)
{

    if (  begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = begIdx; // set pivot number as the head number
    int pivotNum = ary[pivotIdx]; 

    int i = begIdx;
    for( int j = begIdx+1; j <= endIdx; ++j )
    {
        if ( ary[j] > pivotNum ) {
            ++i;
            if ( i != j ) {
                swap2Elements(ary, i,j);
            }
        }
    }
    swap2Elements(ary, i , pivotIdx);

    quickSortHead_DescendOrder(ary, begIdx, i-1, arySz);   // quickSort left  part
    quickSortHead_DescendOrder(ary, i+1, endIdx, arySz);   // quickSort right part
}


void quickSortHead_DescendOrder_Safe(int ary[], int begIdx, int endIdx, int arySz)
{

    if (  begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = begIdx; // set pivot number as the head number
    int pivotNum = getElementSafely(ary, pivotIdx, arySz); // ary[pivotIdx]; 

    int i = begIdx;
    for( int j = begIdx+1; j <= endIdx; ++j )
    {
        if ( getElementSafely(ary, j, arySz) > pivotNum ) {
            ++i;
            if ( i != j ) {
                swap2Elements_Safe(ary, i,j, arySz);
            }
        }
    }
    swap2Elements_Safe(ary, i , pivotIdx ,arySz);

	quickSortHead_DescendOrder_Safe(ary, begIdx, i-1, arySz);   // quickSort left  part
    quickSortHead_DescendOrder_Safe(ary, i+1, endIdx, arySz);   // quickSort right part

}





void quickSortTail_AscendOrder(int ary[], int begIdx, int endIdx, int arySz)
{
    if (  begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = endIdx; // set pivot number as the head number
    int pivotNum = ary[pivotIdx]; 

    int i = begIdx-1;
    for( int j = begIdx; j < endIdx; ++j )
    {
        if ( ary[j] < pivotNum ) {
            ++i;
            if ( i != j ) {
                swap2Elements(ary, i,j);
            }
        }
    }
    ++i;
    swap2Elements(ary, i , pivotIdx);

    quickSortTail_AscendOrder(ary, begIdx, i-1, arySz);   // quickSort left  part
    quickSortTail_AscendOrder(ary, i+1, endIdx, arySz);   // quickSort right part

}

void quickSortTail_AscendOrder_Safe(int ary[], int begIdx, int endIdx, int arySz)
{
    if (  begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = endIdx; // set pivot number as the head number
    int pivotNum = getElementSafely(ary, pivotIdx, arySz); // ary[pivotIdx]; 

    int i = begIdx-1;
    for( int j = begIdx; j < endIdx; ++j )
    {
        if ( getElementSafely(ary, j, arySz)  < pivotNum ) {
            ++i;
            if ( i != j ) {
                swap2Elements_Safe(ary, i,j, arySz);
            }
        }
    }
    ++i;
    swap2Elements_Safe(ary, i , pivotIdx, arySz);

	quickSortTail_AscendOrder_Safe(ary, begIdx, i-1, arySz);   // quickSort left  part
    quickSortTail_AscendOrder_Safe(ary, i+1, endIdx, arySz);   // quickSort right part

}



void quickSortTail_DescendOrder(int ary[], int begIdx, int endIdx, int arySz)
{
    if (  begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = endIdx; // set pivot number as the head number
    int pivotNum = ary[pivotIdx]; 

    int i = begIdx-1;
    for( int j = begIdx; j < endIdx; ++j )
    {
        if ( ary[j] > pivotNum ) {
            ++i;
            if ( i != j ) {
                swap2Elements(ary, i,j);
            }
        }
    }
    ++i;
    swap2Elements(ary, i , pivotIdx);

    quickSortTail_DescendOrder(ary, begIdx, i-1, arySz);   // quickSort left  part
    quickSortTail_DescendOrder(ary, i+1, endIdx, arySz);   // quickSort right part
														   //
}

void quickSortTail_DescendOrder_Safe(int ary[], int begIdx, int endIdx, int arySz)
{
    if (  begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = endIdx; // set pivot number as the head number
    int pivotNum = getElementSafely(ary, pivotIdx, arySz); // ary[pivotIdx]; 

    int i = begIdx-1;
    for( int j = begIdx; j < endIdx; ++j )
    {
        if ( getElementSafely(ary, j, arySz) > pivotNum ) {
            ++i;
            if ( i != j ) {
                swap2Elements_Safe(ary, i,j, arySz);
            }
        }
    }
    ++i;
    swap2Elements_Safe(ary, i , pivotIdx, arySz);

	quickSortTail_DescendOrder_Safe(ary, begIdx, i-1, arySz);   // quickSort left  part
    quickSortTail_DescendOrder_Safe(ary, i+1, endIdx, arySz);   // quickSort right part
																
}





} // using namespace v3

