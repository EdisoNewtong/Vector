#pragma once


namespace v1
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Head Version
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////
	void quickSortHead_AscendOrder(int* ary,  int begIdx, int endIdx, int arySz);
	void quickSortHead_AscendOrder_Safe(int* ary,  int begIdx, int endIdx, int arySz);
	void quickSortHead_DescendOrder(int* ary, int begIdx, int endIdx, int arySz);
	void quickSortHead_DescendOrder_Safe(int* ary, int begIdx, int endIdx, int arySz);


	////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Tail Version
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////
	void quickSortTail_AscendOrder(int* ary,  int begIdx, int endIdx, int arySz);
	void quickSortTail_AscendOrder_Safe(int* ary,  int begIdx, int endIdx, int arySz);
	void quickSortTail_DescendOrder(int* ary, int begIdx, int endIdx, int arySz);
	void quickSortTail_DescendOrder_Safe(int* ary, int begIdx, int endIdx, int arySz);

} // end namespace v1



