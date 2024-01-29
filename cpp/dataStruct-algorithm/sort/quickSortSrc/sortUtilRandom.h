#pragma once

namespace vRandom
{
	void quickSortRandom_AscendOrder(int* ary,  int begIdx, int endIdx, int arySz);
	void quickSortRandom_AscendOrder_Safe(int* ary,  int begIdx, int endIdx, int arySz);

	void quickSortRandom_DescendOrder(int* ary, int begIdx, int endIdx, int arySz);
	void quickSortRandom_DescendOrder_Safe(int* ary, int begIdx, int endIdx, int arySz);
}

