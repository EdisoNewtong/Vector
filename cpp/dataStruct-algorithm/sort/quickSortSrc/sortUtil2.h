
#ifndef SORT_UTIL_2_H
#define SORT_UTIL_2_H

#define getEle(ary,idx,arysz)    ( !G_checkArrayBoundFlag   ?  ary[(idx)]  :  safeGetElementInArray(ary,(idx),arysz) )
#define checkBound(idx,arysz)    ( (idx) >= 0   &&  (idx) < arysz ) 



extern bool G_checkArrayBoundFlag;
extern unsigned long long G_indexOutBoundCnt;

extern bool G_quickSortRangeCheck;
extern bool G_quickSortOutOfRange_return;
extern unsigned long long G_quickSortRangesOutOfBoundCnt;





extern void swap2Elements(int ary[], int idx1, int idx2);

extern bool isArray_AscendOrdered(int ary[], int sz);
extern bool isArray_DescendOrdered(int ary[], int sz);

extern void quickSortHead_AscendOrder(int ary[],  int begIdx, int endIdx, int arySz);
extern void quickSortHead_DescendOrder(int ary[], int begIdx, int endIdx, int arySz);

extern void quickSortTail_AscendOrder(int ary[],  int begIdx, int endIdx, int arySz);
extern void quickSortTail_DescendOrder(int ary[], int begIdx, int endIdx, int arySz);



#endif

