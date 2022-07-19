#ifndef SORT_UTIL_H
#define SORT_UTIL_H

#define getEle(ary,idx,arysz)    ( !G_checkArrayBoundFlag   ?  ary[(idx)]  :  safeGetElementInArray(ary,(idx),arysz) )
#define checkBound(idx,arysz)    ( (idx) >= 0   &&  (idx) < arysz ) 



extern bool G_checkArrayBoundFlag;
extern unsigned long long G_indexOutBoundCnt;

extern bool G_quickSortRangeCheck;
extern bool G_quickSortOutOfRange_return;
extern unsigned long long G_quickSortRangesOutOfBoundCnt;




extern int safeGetElementInArray(int ary[], int idx, int arySz);
extern void swap2numbers(int ary[], int idx1, int idx2, int arySz);
extern bool isArrayWellSorted(int ary[], int sz, bool isAsc);
extern void quickSortHead(int ary[], int begIdx, int endIdx, int arySz, bool isAsc);
extern void quickSortTail(int ary[], int begIdx, int endIdx, int arySz, bool isAsc);

#endif
