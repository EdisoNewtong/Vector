#include <iostream>
#include "sortUtil.h"
using namespace std;




int safeGetElementInArray(int ary[], int idx, int arySz)
{
    if ( !( checkBound(idx, arySz) ) ) {
        ++G_indexOutBoundCnt;
        cout << "[ERROR] : Out of array's bound , index -> " << idx << " , array.size = " << arySz << endl;
        return -1;
    }

    return ary[idx];
}


void swap2numbers(int ary[], int idx1, int idx2, int arySz)
{
    //
    // for array bounding check Only
    //
    int v1 = getEle(ary, idx1, arySz);
    int v2 = getEle(ary, idx2, arySz);
    (void)v2;

    int tmp = v1;
    ary[idx1] = ary[idx2];
    ary[idx2] = tmp;
}


bool isArrayWellSorted(int ary[], int sz, bool isAsc)
{
    bool isSorted = true;
    for( int i = 0; i < sz-1; ++i)
    {
        int leftNum = getEle(ary, i, sz);
        int rightNum = getEle(ary, (i+1), sz);
        bool isBadOrdered = ( isAsc ?   (leftNum > rightNum) 
                                    :   (leftNum < rightNum) );

        if ( isBadOrdered  ) {
            isSorted = false;
            break;
        }
    }

    return isSorted;
}



void quickSortHead(int ary[], int begIdx, int endIdx, int arySz, bool isAsc)
{
    if ( G_quickSortRangeCheck ) {
        if ( checkBound(begIdx, arySz) ) {
            ++G_quickSortRangesOutOfBoundCnt;
            cout << "[ERROR] : in quickSortHead(...) , begIdx is out of bound" << endl;
            if ( G_quickSortOutOfRange_return ) { 
                return;
            }
        }

        if ( checkBound(endIdx, arySz) ) {
            ++G_quickSortRangesOutOfBoundCnt;
            cout << "[ERROR] : in quickSortHead(...) , endIdx is out of bound" << endl;
            if ( G_quickSortOutOfRange_return ) { 
                return;
            }
        }
    }

    //////////////////////////////////////////////////
    //      Real code execute from here
    if (  begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = begIdx; // set povit number as the head number
    int pivotNum = getEle(ary, pivotIdx, arySz);

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
        while (      i < j    
                &&  ( isAsc ?  ( getEle(ary, j, arySz) > pivotNum )  :  ( getEle(ary, j, arySz) < pivotNum ) ) )
        { --j; } 

        // 2. try to find the most left element whose value is greater than pivot number
        while (      i < j    
                &&  ( isAsc ?  ( getEle(ary, i, arySz) <= pivotNum )   :  ( getEle(ary, i, arySz) >= pivotNum ) ) )
        { ++i; }

        swap2numbers(ary, i, j, arySz);
    }

    // swap   pivotNum <--> i ( the 2nd movement pointer is i )
    swap2numbers(ary, pivotIdx, i, arySz);
    quickSortHead(ary, begIdx, i-1, arySz, isAsc);   // qSort on left side  part of the pivot number
    quickSortHead(ary, i+1, endIdx, arySz, isAsc);   // qSort on right side part of the pivot number
}


void quickSortTail(int ary[], int begIdx, int endIdx, int arySz, bool isAsc)
{
    if ( G_quickSortRangeCheck ) {
        if ( checkBound(begIdx, arySz) ) {
            ++G_quickSortRangesOutOfBoundCnt;
            cout << "[WARNING] : in quickSortTail(...) , begIdx is out of bound" << endl;
            if ( G_quickSortOutOfRange_return ) { 
                return;
            }
        }

        if ( checkBound(endIdx, arySz) ) {
            ++G_quickSortRangesOutOfBoundCnt;
            cout << "[WARNING] : in quickSortTail(...) , endIdx is out of bound" << endl;
            if ( G_quickSortOutOfRange_return ) { 
                return;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //       Real code execute from here
    if ( begIdx >= endIdx  ) {
        return;
    }

    int pivotIdx = endIdx;
    int pivotNum = getEle(ary, pivotIdx, arySz);

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
                    
                     1. ----------> i
                     2. <---------- j

                if you pick ary[last] as the pivot :

                    Left-Pointer move first , then Right-Pointer .   if you pick ary[last] as the pivot

              !!! <Never> <Never> <Never> !!! exchange the order, otherwise the bug will appear
        */

        // 1. try to find the most left element whose value is greater than pivot number
        while (      i < j    
                &&  ( isAsc ?  ( getEle(ary, i, arySz) < pivotNum )   :  ( getEle(ary, i, arySz) > pivotNum ) ) )
        { ++i; }

        // 2. try to find the most right element whose value is smaller than pivot number
        while (      i < j    
                &&  ( isAsc ?  ( getEle(ary, j, arySz) >= pivotNum )  :  ( getEle(ary, j, arySz) <= pivotNum ) ) )
        { --j; }

        swap2numbers(ary, i, j, arySz);
    }

    // swap   pivotNum <--> j ( the 2nd movement pointer is j )
    swap2numbers(ary, pivotIdx, j, arySz);
    quickSortTail(ary, begIdx, j-1, arySz, isAsc);   // qSort on left side  part of the pivot number
    quickSortTail(ary, j+1, endIdx, arySz, isAsc);   // qSort on right side part of the pivot number

}

