#include "sortUtil.h"


void swap2Elements(int ary[], int idx1, int idx2)
{
    int tmp = ary[idx1];
    ary[idx1] = ary[idx2];
    ary[idx2] = tmp;
}


bool isArray_AscendOrdered(int ary[], int sz)
{
    bool isSorted = true;
    for( int i = 0; i < sz-1; ++i)
    {
        if ( ary[i] > ary[i+1] ) {
            isSorted = false;
            break;
        }
    }

    return isSorted;
}


bool isArray_DescendOrdered(int ary[], int sz)
{
    bool isSorted = true;
    for( int i = 0; i < sz-1; ++i)
    {
        if ( ary[i] < ary[i+1] ) {
            isSorted = false;
            break;
        }
    }

    return isSorted;
}



void quickSortHead_AscendOrder(int ary[], int begIdx, int endIdx, int arySz)
{

    //////////////////////////////////////////////////
    //      Real code execute from here
    if (  begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = begIdx; // set povit number as the head number
    int pivotNum = ary[pivotIdx]; 

    /************************************************** 
        Core Core Core :
                            i must assign to   begIdx  ( Left Pointer  loop start here )
                             <Rather than>     begIdx +1               // [ ERROR ]
    */
    int i = begIdx;
    int j = endIdx;
    while ( i < j )
    {
        /************************************************** 
         
            Core Core Core :
                    
                     1. <---------- j
                     2. ----------> i

                if you pick ary[0] as the pivot :

                    Right-Pointer move first , then Left-Pointer .


              !!! <Never> <Never> <Never> !!! exchange the order, otherwise the bug will appear
        */

        // 1. try to find the most right element whose value is smaller than pivot number
        while ( i < j   &&   ary[j] > pivotNum ) { --j; } 

        // 2. try to find the most left element whose value is greater than pivot number
        while ( i < j   &&   ary[i] <= pivotNum ) { ++i; }

        swap2Elements(ary, i, j);
    }

    // swap   pivotNum <--> i ( the 2nd movement pointer is i )
    swap2Elements(ary, pivotIdx, i);
    quickSortHead_AscendOrder(ary, begIdx, i-1, arySz);   // qSort on left side  part of the pivot number
    quickSortHead_AscendOrder(ary, i+1, endIdx, arySz);   // qSort on right side part of the pivot number
}



void quickSortHead_DescendOrder(int ary[], int begIdx, int endIdx, int arySz)
{
    if (  begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = begIdx; // set povit number as the head number
    int pivotNum = ary[pivotIdx]; 

    int i = begIdx;
    int j = endIdx;
    while ( i < j )
    {
        while ( i < j   &&   ary[j] < pivotNum )  { --j; } 
        while ( i < j   &&   ary[i] >= pivotNum ) { ++i; }

        swap2Elements(ary, i, j);
    }

    swap2Elements(ary, pivotIdx, i);
    quickSortHead_DescendOrder(ary, begIdx, i-1, arySz);   // qSort on left side  part of the pivot number
    quickSortHead_DescendOrder(ary, i+1, endIdx, arySz);   // qSort on right side part of the pivot number

}




void quickSortTail_AscendOrder(int ary[], int begIdx, int endIdx, int arySz)
{
    if ( begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = endIdx;
    int pivotNum = ary[pivotIdx];

    int i = begIdx;
    int j = endIdx;
    while ( i < j )
    {
        /************************************************** 
         
            Core Core Core :
                    
                     1. ----------> i
                     2. <---------- j

                if you pick ary[last] as the pivot :

                    Left-Pointer move first , then Right-Pointer .   if you pick ary[last] as the pivot

              !!! <Never> <Never> <Never> !!! exchange the order, otherwise the bug will appear
        */

        // 1. try to find the most left element whose value is greater than pivot number
        while ( i < j   &&  ary[i]  < pivotNum ) { ++i; }

        // 2. try to find the most right element whose value is smaller than pivot number
        while ( i < j   &&  ary[j] >= pivotNum ) { --j; }

        swap2Elements(ary, i, j);
    }

    // swap   pivotNum <--> j ( the 2nd movement pointer is j )
    swap2Elements(ary, pivotIdx, j);
    quickSortTail_AscendOrder(ary, begIdx, j-1, arySz);   // qSort on left side  part of the pivot number
    quickSortTail_AscendOrder(ary, j+1, endIdx, arySz);   // qSort on right side part of the pivot number
}


void quickSortTail_DescendOrder(int ary[], int begIdx, int endIdx, int arySz)
{    
    if ( begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = endIdx;
    int pivotNum = ary[pivotIdx];

    int i = begIdx;
    int j = endIdx;
    while ( i < j )
    {
        while ( i < j   &&  ary[i] >  pivotNum ) { ++i; }
        while ( i < j   &&  ary[j] <= pivotNum ) { --j; }

        swap2Elements(ary, i, j);
    }

    swap2Elements(ary, pivotIdx, j);
    quickSortTail_DescendOrder(ary, begIdx, j-1, arySz);   // qSort on left side  part of the pivot number
    quickSortTail_DescendOrder(ary, j+1, endIdx, arySz);   // qSort on right side part of the pivot number

}

