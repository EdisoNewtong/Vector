#include "common.h"
#include "sortUtilRandom.h"
#include <cstdlib>


namespace vRandom
{


void quickSortRandom_AscendOrder(int* ary, int begIdx, int endIdx, int arySz)
{
    if (  begIdx >= endIdx  ) {
        return;
    }

    if ( begIdx+1 == endIdx ) {
        // only 2 numbers in the passed range 
        if ( ary[begIdx] > ary[endIdx] ) {
            swap2Elements(ary, begIdx, endIdx);
        }
        return;
    }

    // at least 3 elements in the range
    int lIndex = begIdx + 1;
    int rIndex = endIdx - 1;
    int lenOfCenterRange = rIndex - lIndex + 1;

    // int dt = 0;
    // if ( lenOfCenterRange == 0 ) { dt = std::abs(std::rand()) % 2; } else { dt = std::abs(std::rand()) % lenOfCenterRange; }
    // int pivotIdx = lIndex +  dt; // std::abs(std::rand()) % lenOfCenterRange; // random the pivot position at the center position of given range 

    int pivotIdx = lIndex +  std::abs(std::rand()) % lenOfCenterRange; // random the pivot position at the center position of given range 
    int pivotNum = ary[pivotIdx]; 

    int i = begIdx;
    int j = endIdx;

    while ( i < j )
    {
        int jcmp = (j > pivotIdx && i < pivotIdx) ? pivotIdx : i;
        int icmp = (i < pivotIdx && j > pivotIdx) ? pivotIdx : j;

        while ( j > jcmp  &&  ary[j] >  pivotNum ) { --j; }
        while ( i < icmp  &&  ary[i] <= pivotNum ) { ++i; }

        bool bNeedSwap = true;
        if ( j == pivotIdx ) {
            bNeedSwap = false;
            j = pivotIdx - 1;
        } else if ( i == pivotIdx ) {
            bNeedSwap = false;
            i = pivotIdx + 1;
        }

        if ( bNeedSwap &&  i < j ) {
            swap2Elements(ary, i, j);
        }
    }

    if (  j > pivotIdx ) {
        swap2Elements(ary, pivotIdx, j);
        i = j;
    } else if ( ary[i] > pivotNum ) { 
        swap2Elements(ary, pivotIdx, i);
    }

    quickSortRandom_AscendOrder(ary, begIdx, i-1, arySz);   // quickSort left  part
    quickSortRandom_AscendOrder(ary, i+1, endIdx, arySz);   // quickSort right part

}

void quickSortRandom_AscendOrder_Safe(int* ary, int begIdx, int endIdx, int arySz)
{
    if (  begIdx >= endIdx  ) {
        return;
    }

    if ( begIdx+1 == endIdx ) {
        // only 2 numbers in the passed range 
        if ( getElementSafely(ary, begIdx, arySz)  > getElementSafely(ary, endIdx, arySz) ) {
            swap2Elements_Safe(ary, begIdx, endIdx, arySz);
        }
        return;
    }

    // at least 3 elements in the range
    int lIndex = begIdx + 1;
    int rIndex = endIdx - 1;
    int lenOfCenterRange = rIndex - lIndex + 1;

    // int dt = 0;
    // if ( lenOfCenterRange == 0 ) { dt = std::abs(std::rand()) % 2; } else { dt = std::abs(std::rand()) % lenOfCenterRange; }
    // int pivotIdx = lIndex +  dt; // std::abs(std::rand()) % lenOfCenterRange; // random the pivot position at the center position of given range 

    int pivotIdx = lIndex +  std::abs(std::rand()) % lenOfCenterRange; // random the pivot position at the center position of given range 
    int pivotNum = getElementSafely(ary, pivotIdx, arySz); // ary[pivotIdx]; 

    int i = begIdx;
    int j = endIdx;

    while ( i < j )
    {
        int jcmp = (j > pivotIdx && i < pivotIdx) ? pivotIdx : i;
        int icmp = (i < pivotIdx && j > pivotIdx) ? pivotIdx : j;

        while ( j > jcmp  &&  getElementSafely(ary,j, arySz) >  pivotNum ) { --j; }
        while ( i < icmp  &&  getElementSafely(ary,i, arySz) <= pivotNum ) { ++i; }

        bool bNeedSwap = true;
        if ( j == pivotIdx ) {
            bNeedSwap = false;
            j = pivotIdx - 1;
        } else if ( i == pivotIdx ) {
            bNeedSwap = false;
            i = pivotIdx + 1;
        }

        if ( bNeedSwap &&  i < j ) {
            swap2Elements_Safe(ary, i, j, arySz);
        }
    }

    if (  j > pivotIdx ) {
		swap2Elements_Safe(ary, pivotIdx, j, arySz);
        i = j;
    } else if ( getElementSafely(ary, i, arySz) > pivotNum ) { 
		swap2Elements_Safe(ary, pivotIdx, i, arySz);
    }

    quickSortRandom_AscendOrder_Safe(ary, begIdx, i-1, arySz);   // quickSort left  part
    quickSortRandom_AscendOrder_Safe(ary, i+1, endIdx, arySz);   // quickSort right part

}



void quickSortRandom_DescendOrder(int* ary, int begIdx, int endIdx, int arySz)
{
    if (  begIdx >= endIdx  ) {
        return;
    }

    if ( begIdx+1 == endIdx ) {
        // only 2 numbers in the passed range 
        if ( ary[begIdx] < ary[endIdx] ) {
            swap2Elements(ary, begIdx, endIdx);
        }
        return;
    }

    // at least 3 elements in the range
    int lIndex = begIdx + 1;
    int rIndex = endIdx - 1;
    int lenOfCenterRange = rIndex - lIndex + 1;

    // int dt = 0;
    // if ( lenOfCenterRange == 0 ) { dt = std::abs(std::rand()) % 2; } else { dt = std::abs(std::rand()) % lenOfCenterRange; }
    // int pivotIdx = lIndex +  dt; // std::abs(std::rand()) % lenOfCenterRange; // random the pivot position at the center position of given range 

    int pivotIdx = lIndex +  std::abs(std::rand()) % lenOfCenterRange; // random the pivot position at the center position of given range 
    int pivotNum = ary[pivotIdx]; 

    int i = begIdx;
    int j = endIdx;

    while ( i < j )
    {
        int jcmp = (j > pivotIdx && i < pivotIdx) ? pivotIdx : i;
        int icmp = (i < pivotIdx && j > pivotIdx) ? pivotIdx : j;

        while ( j > jcmp  &&  ary[j] <  pivotNum ) { --j; }
        while ( i < icmp  &&  ary[i] >= pivotNum ) { ++i; }

        bool bNeedSwap = true;
        if ( j == pivotIdx ) {
            bNeedSwap = false;
            j = pivotIdx - 1;
        } else if ( i == pivotIdx ) {
            bNeedSwap = false;
            i = pivotIdx + 1;
        }

        if ( bNeedSwap &&  i < j ) {
            swap2Elements(ary, i, j);
        }
    }

    if (  j > pivotIdx ) {
        swap2Elements(ary, pivotIdx, j);
        i = j;
    } else if ( ary[i] < pivotNum ) { 
        swap2Elements(ary, pivotIdx, i);
    }

    quickSortRandom_DescendOrder(ary, begIdx, i-1, arySz);   // quickSort left  part
    quickSortRandom_DescendOrder(ary, i+1, endIdx, arySz);   // quickSort right part
}

void quickSortRandom_DescendOrder_Safe(int* ary, int begIdx, int endIdx, int arySz)
{
    if (  begIdx >= endIdx  ) {
        return;
    }

    if ( begIdx+1 == endIdx ) {
        // only 2 numbers in the passed range 
        if ( getElementSafely(ary, begIdx, arySz) < getElementSafely(ary, endIdx, arySz) ) {
            swap2Elements_Safe(ary, begIdx, endIdx, arySz);
        }
        return;
    }

    // at least 3 elements in the range
    int lIndex = begIdx + 1;
    int rIndex = endIdx - 1;
    int lenOfCenterRange = rIndex - lIndex + 1;

    // int dt = 0;
    // if ( lenOfCenterRange == 0 ) { dt = std::abs(std::rand()) % 2; } else { dt = std::abs(std::rand()) % lenOfCenterRange; }
    // int pivotIdx = lIndex +  dt; // std::abs(std::rand()) % lenOfCenterRange; // random the pivot position at the center position of given range 

    int pivotIdx = lIndex +  std::abs(std::rand()) % lenOfCenterRange; // random the pivot position at the center position of given range 
    int pivotNum = getElementSafely(ary, pivotIdx, arySz); // ary[pivotIdx]; 

    int i = begIdx;
    int j = endIdx;

    while ( i < j )
    {
        int jcmp = (j > pivotIdx && i < pivotIdx) ? pivotIdx : i;
        int icmp = (i < pivotIdx && j > pivotIdx) ? pivotIdx : j;

        while ( j > jcmp  && ( getElementSafely(ary, j, arySz)  <  pivotNum ) ) { --j; }
        while ( i < icmp  && ( getElementSafely(ary, i, arySz)  >= pivotNum ) ) { ++i; }

        bool bNeedSwap = true;
        if ( j == pivotIdx ) {
            bNeedSwap = false;
            j = pivotIdx - 1;
        } else if ( i == pivotIdx ) {
            bNeedSwap = false;
            i = pivotIdx + 1;
        }

        if ( bNeedSwap &&  i < j ) {
            swap2Elements_Safe(ary, i, j, arySz);
        }
    }

    if (  j > pivotIdx ) {
		swap2Elements_Safe(ary, pivotIdx, j, arySz);
        i = j;
    } else if ( getElementSafely(ary, i, arySz) < pivotNum ) { 
		swap2Elements_Safe(ary, pivotIdx, i, arySz);
    }

    quickSortRandom_DescendOrder_Safe(ary, begIdx, i-1, arySz);   // quickSort left  part
    quickSortRandom_DescendOrder_Safe(ary, i+1, endIdx, arySz);   // quickSort right part
}

} // end namespace vRandom
