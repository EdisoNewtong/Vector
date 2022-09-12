#include "common.h"


bool isArray_AscendOrdered(int* ary, int sz)
{
	bool isWellSorted = true;
	for( int i = 0; i < sz-1; ++i )
	{
		if ( ary[i] > ary[i+1] ) {
			isWellSorted = false;
			break;
		}
	}
	return isWellSorted;
}

bool isArray_DescendOrdered(int* ary, int sz)
{
	bool isWellSorted = true;
	for( int i = 0; i < sz-1; ++i )
	{
		if ( ary[i] < ary[i+1] ) {
			isWellSorted = false;
			break;
		}
	}
	return isWellSorted;
}

int getElementSafely(int* ary,int idx, int arySz)
{
	if ( !(idx>=0 && idx<arySz) ) {
		string errMsg("idx(");
		errMsg += (to_string(idx) + ") is out of range(" + to_string(arySz) + ")");
		myIdxOutOfRangeException e(errMsg);
		throw e;
	}

	return ary[idx];
}


void swap2Elements(int* ary, int idx1, int idx2)
{
	int tmp = ary[idx1];
	ary[idx1] = ary[idx2];
	ary[idx2] = tmp;
}


void swap2Elements_Safe(int* ary, int idx1, int idx2, int arySz)
{
	if ( !(idx1>=0 && idx1<arySz) ) {
		string errMsg("idx-1(");
		errMsg += (to_string(idx1) + ") is out of range(" + to_string(arySz) + ")");
		myIdxOutOfRangeException e(errMsg);
		throw e;
	}

	if ( !(idx2>=0 && idx2<arySz) ) {
		string errMsg("idx-2(");
		errMsg += (to_string(idx2) + ") is out of range(" + to_string(arySz) + ")");
		myIdxOutOfRangeException e(errMsg);
		throw e;
	}

	int tmp = ary[idx1];
	ary[idx1] = ary[idx2];
	ary[idx2] = tmp;
}



