#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;





int getElement2(int* ary, int arysz, int idx, int* pErrorCnt, int* isOK)
{
	if ( idx >= 0 && idx < arysz ) {
		if ( isOK != nullptr ) {
			*isOK = 1;
		}

		return ary[idx];
	} else {
		if ( pErrorCnt != nullptr ) {
			++(*pErrorCnt);
		}

		if ( isOK != nullptr ) {
			*isOK = 0;
		}
		cout << "idx is out of range -> idx = " << idx << "  range:[0," << arysz << ") " << endl;

		return 0;
	}
}


/************************************************** 

   Travelsal the range from [nodeIdx, arySz) 

Place the node by the given 'nodeIdx'   into a proper slot , to make the subtree whose rootNode is located by nodeIdx to be the <Max Root Heap>

----------------------------------------------------------------------------------------------------
Core Core Core :  ( the node at the nodeIdx must be the max node inside the whole subtree whose root node is 'nodeIdx')
----------------------------------------------------------------------------------------------------

***************************************************/
void shift2(int* ary, int nodeIdx, int arySz)
{
    int pIdx = nodeIdx;
    int leftIdx = pIdx * 2 + 1;
    int target = ary[pIdx];

    while ( leftIdx < arySz )
    {
        if ( (leftIdx+1) < arySz ) {
            leftIdx = ary[(leftIdx+1)] > ary[leftIdx] ? (leftIdx+1) : leftIdx;
        }

        if ( ary[leftIdx] > target ) {
            ary[pIdx] = ary[leftIdx]; // bubble the greater number to the <Parent> slot

            //
            // continue sort the <Downer> Subtree nodes
            //
            pIdx = leftIdx;
            leftIdx = pIdx * 2 + 1;
        } else {
            break;
        }
    }

    // place
    ary[pIdx] = target;
}



void shift2_WithIdxCheck(int* ary, int low, int high, int arysz, int* pErrorCnt)
{
    int bIsOK = 0;

	// int pIdx = low;
	int pIdx = low;
    // int leftIdx = pIdx * 2;
    int leftIdx = (pIdx * 2 + 1);


    // int target = ary[pIdx];
    int target = getElement2(ary, arysz, pIdx, pErrorCnt, &bIsOK );

    while ( leftIdx <= high )
    {
        if ( (leftIdx+1) <= high ) {
            // leftIdx = ( ary[leftIdx+1] > ary[leftIdx] ) ? (leftIdx+1) : leftIdx;
			leftIdx = ( getElement2(ary, arysz, (leftIdx+1) , pErrorCnt, &bIsOK ) > getElement2(ary, arysz, (leftIdx), pErrorCnt, &bIsOK ) )
					  ? (leftIdx+1) : leftIdx;
        }


		getElement2(ary, arysz, (leftIdx), pErrorCnt, &bIsOK );
        if ( ary[leftIdx]  > target ) {
            // ary[pIdx] = ary[leftIdx];
			getElement2(ary, arysz, (pIdx), pErrorCnt, &bIsOK );
			getElement2(ary, arysz, (leftIdx), pErrorCnt, &bIsOK );
			ary[pIdx] = ary[leftIdx];

            pIdx = leftIdx;
            leftIdx = (2 * pIdx + 1);
        } else {
            break;
        }

    }

	getElement2(ary, arysz, (pIdx), pErrorCnt, &bIsOK );
    ary[pIdx] = target;

}





/**************************************************
   
========== No Dummy Node : [0] ==========


             [0]
            /   \
           /     \
          /       \
        [1]       [2]
       /  \       /  \
     [3]  [4]   [5]  [6]



leftChildIdx  = (2 * parent) + 1;
rightChildIdx = (2 * parent) + 1 + 1 = (2 * parent) + 2;


========== No Dummy Node : [0] ==========

Array Range :              [0, n-1]

***************************************************/
void HeapSort2(int* ary, int arySz)
{
    ////////////////////////////////////////////////////
	//
	//
    //   Build the Max-Root-Heap Once
	//
	//
    ////////////////////////////////////////////////////
    // 因为数组的最后一个元素下标为 (arySz-1)
	//
	// 因为对于任何 parentIdx
	//    leftIdx  = parentIdx * 2 + 1 (一定是奇数)
	//    rightIdx = parentIdx * 2 + 2 (一定是偶数)
	//
	//  因此对于任何一个给定下标的 左儿子 / 右儿子 结点
	//  leftIdx/2  =  parentIdx     ( 1/2=0.5 , 被取整后，0.5被自动剔除 )
	//  rightIdx/2 = (parentIdx+1) ( 一定能整除, 但比起正确的值多了1 )
	//  而无论 leftIdx or rightIdx 的值是多少
	//  如果左儿子是数组最后一个元素，那么 leftIdx+1  = arySz  ( arySz 是偶数 )
	//  如果右儿子是数组最后一个元素，那么 rightIdx+1 = arySz  ( arySz 是奇数 )
	//  那么 表达式 (arySz-2) / 2 一定能正确地计算出 他们对应的父结点的 index
	//     
	//     假设 :
	//     最后一个元素是 <左>儿子
	//        那么  arySz-1      = parentIdx * 2 + 1
	//             (arySz-1-1)   = parentIdx * 2 + 1 - 1 = parentIdx * 2
	//              (arySz-2)/2  = (parentIdx * 2)/2
	//              (arySz-2)/2  = parentIdx 
	//
	//     最后一个元素是 <右>儿子
	//        那么  arySz-1      = parentIdx * 2 + 2
	//             (arySz-1-1)   = parentIdx * 2 + 2 - 1 = parentIdx * 2 + 1
	//              (arySz-2)   = parentIdx * 2 + 1
	//              (arySz-2)/2 = static_cast<int>( parentIdx + 0.5) = parentIdx; // 舍弃0.5 , 得到 parentIdx
	//
	// 另外，不用担心， (arySz-2)/2 是负数的风险，因为 arySz 无论是1 or 2,  条件 i>=0 可以保证下标的合法数
    for( int i = (arySz-2)/2; i>=0; --i ) {
		// 必须是 底层 至 上层的顺序，创建大根堆， 这样能使得每一颗子树都能满足大根堆的条件，从而得到整颗树以及所有子树都满足 大根堆，
		// 因此，当循环结束后， ary[0] 即 树的根结点处，一定是最大的元素
        shift2(ary, i, arySz);
    }

	// 看一个特殊的例子 : 
	// 当数组大小为2时，  [0]        [1]
	//                   MaxOne    SmallOne
	//
	// 因此需要交换2者的位置，以达成  从小到大的排序的需求
    for( int newSz = arySz; newSz >= 2; --newSz )
    {
        int lastIdx = newSz-1;
        // swap [0] <-> last
        {
            int maxElement = ary[0];
            ary[0] = ary[lastIdx];
            ary[lastIdx] = maxElement;
        }

		//
		// 使 [0] 元素归位，将 它 所在的子树的最大值 放置在 [0] 处
		//   lastIdx 为 剔除最后一个元素后的， 数组的长度
		//
        // kick  the last elment out of the sort range
        // sort at range [0, newSz-1 ]
        shift2(ary, 0, lastIdx);
    }
}




void HeapSort2_WithIdxCheck(int* ary, int n, int* errorCnt)
{
	// int errorCnt = 0;
	int bIsOK = 0;

    for(int  i = (n-2)/2; i>=0; --i ) {
        shift2_WithIdxCheck(ary, i, n,  (n),   errorCnt);
    }


    for(int  i = n; i>=2; --i )
	{
		/*   swap [0] <--> [n-1]   */
	   int maxNumber = ary[0];
	   getElement2(ary, (n),(i-1), errorCnt, &bIsOK );
	   ary[0] = ary[i-1];
	   ary[i-1] = maxNumber;

	   shift2_WithIdxCheck(ary, 0, (i-1),   (n),   errorCnt);
	}

}







////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//  Core Core Core :
//			Original Version
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////
int getElement1(int* ary, int arysz, int idx, int* pErrorCnt, int* isOK)
{
	// use   idx > 0   rather than       idx >= 0
	if ( idx > 0 && idx < arysz ) {
		if ( isOK != nullptr ) {
			*isOK = 1;
		}

		return ary[idx];
	} else {
		if ( pErrorCnt != nullptr ) {
			++(*pErrorCnt);
		}

		if ( isOK != nullptr ) {
			*isOK = 0;
		}
		cout << "idx is out of range -> idx = " << idx << "  range:[0," << arysz << ") " << endl;

		return 0;
	}
}



void shift_Original(int* ary, int low, int high)
{
	int pIdx = low; // pIdx => parent index
    int leftIdx = pIdx * 2;

    int target = ary[pIdx];

    while ( leftIdx <= high )
    {
        if ( (leftIdx+1) <= high ) {
            leftIdx = ( ary[leftIdx+1] > ary[leftIdx] ) ? (leftIdx+1) : leftIdx;
        }


        if ( ary[leftIdx]  > target ) {
			ary[pIdx] = ary[leftIdx];

            pIdx = leftIdx;
            leftIdx = 2 * pIdx;
        } else {
            break;
        }
    }

    ary[pIdx] = target;
}


void shift_OriginalWithIndexCheck(int* ary, int low, int high, int arysz, int* pErrorCnt)
{
    int bIsOK = 0;

	int pIdx = low; // pIdx => parent index
    int leftIdx = pIdx * 2;

    int target = getElement1(ary, arysz, pIdx, pErrorCnt, &bIsOK );

    while ( leftIdx <= high )
    {
        if ( (leftIdx+1) <= high ) {
			leftIdx = ( getElement1(ary, arysz, (leftIdx+1) , pErrorCnt, &bIsOK ) > getElement1(ary, arysz, (leftIdx), pErrorCnt, &bIsOK ) )
					  ? (leftIdx+1) : leftIdx;
        }


		getElement1(ary, arysz, (leftIdx), pErrorCnt, &bIsOK );
        if ( ary[leftIdx]  > target ) {
			getElement1(ary, arysz, (pIdx), pErrorCnt, &bIsOK );
			getElement1(ary, arysz, (leftIdx), pErrorCnt, &bIsOK );
			ary[pIdx] = ary[leftIdx];

            pIdx = leftIdx;
            leftIdx = 2 * pIdx;
        } else {
            break;
        }

    }

	getElement1(ary, arysz, (pIdx), pErrorCnt, &bIsOK );
    ary[pIdx] = target;
}






/**************************************************


Typical original sample function   from   <<Algorithm Book>>


--------------------------------------------------
Array Range   :  [0,n]
--------------------------------------------------
	Dummy-Node    :  [0] 
	Real Elements :  [1, n]



             [1]
            /   \
           /     \
          /       \
        [2]       [3]
       /  \       /  \
     [4]  [5]   [6]  [7]



leftChildIdx  = (2 * parent)
rightChildIdx = (2 * parent) + 1;


***************************************************/
void HeapSort_Original(int* ary, int n)
{
    int i = 0;
    for( i = n/2; i>=1; --i ) {
        shift_Original(ary, i, n);
    }


    for( i = n; i>=2; --i ) {
        // swap [1] <--> [n]
        {
            int last  = ary[i];
            ary[i] = ary[1];
            ary[1] = last;
        }

        shift_Original(ary,1, i-1);
    }


}



void testLineByLine1( const string& filename)
{
    ifstream f( filename.c_str(), ios::in );
    if ( !f ) {
        cout << "[ERROR] : Can't Open File : " << filename << endl;
        return;
    }


    int* pAry = nullptr;
    int lineNo = 0;
    int sortCnt = 0;
    int unsortCnt = 0;
    while ( !f.eof() ) {
        string strLine;
        getline( f, strLine);
		if ( strLine.empty() ) {
			continue;
		}

        ++lineNo;

        if ( pAry != nullptr ) {
            delete [] pAry;
            pAry = nullptr;
        }

        int cnt = 0;
        vector<int> vAry;
        vAry.clear();

        string::size_type pos = string::npos;
        string::size_type beg = 0;
        while ( (pos = strLine.find(",", beg)) != string::npos ) {
            string strnum = strLine.substr(beg, (pos - beg) );
            try {
                int num = stoi( strnum );
                vAry.push_back( num );
                ++cnt;
            } catch (... ) {
                cout << "[ERROR] : can't convert \"" << strnum << "\" to int. " << endl;
            }

            beg = pos + 1;
        }


        string strnum = strLine.substr(beg);
        try {
            int num = stoi( strnum );
            vAry.push_back( num );
            ++cnt;
        } catch ( ... ) {
            cout << "[ERROR] : can't convert \"" << strnum << "\" to int. " << endl;
        }
        cout << "@"  << lineNo << ". parse "  << cnt << " numbers. " << endl;

        int idx1 = 0;
        pAry = new int[ cnt + 1 ];
        pAry[0] = -100;


        idx1 = 1;
        for ( auto e : vAry ) {
            pAry[idx1++] = e;
        }
        vAry.clear();

        
		int errorCnt = 0; 
        HeapSort_Original( pAry, cnt);

        if ( std::is_sorted( pAry+1, pAry + (cnt+1) ) ) {
            ++sortCnt;
            cout << "line : " << lineNo << " is sorted . "; 
			if ( errorCnt > 0 ) {
				cout << "errorCnt = " << errorCnt;
			}
			cout << endl;
        } else {
            ++unsortCnt;
            cout << "line : " << lineNo << " is unsorted . "; 
			if ( errorCnt > 0 ) {
				cout << "errorCnt = " << errorCnt;
			}
			cout << endl;
        }
    }


    if ( pAry != nullptr ) {
        delete [] pAry;
        pAry = nullptr;
    }
    f.close();


    cout << "lineNo = " << lineNo << endl;
    cout << "\t  sorted.count = " << sortCnt   << endl;
    cout << "\tunsorted.count = " << unsortCnt << endl;
}




void testLineByLine2( const string& filename)
{
    ifstream f( filename.c_str(), ios::in );
    if ( !f ) {
        cout << "[ERROR] : Can't Open File : " << filename << endl;
        return;
    }


    int* pAry = nullptr;
    int lineNo = 0;
    int sortCnt = 0;
    int unsortCnt = 0;
    while ( !f.eof() ) {
        string strLine;
        getline( f, strLine);
		if ( strLine.empty() ) {
			continue;
		}

        ++lineNo;

        if ( pAry != nullptr ) {
            delete [] pAry;
            pAry = nullptr;
        }

        int cnt = 0;
        vector<int> vAry;
        vAry.clear();

        string::size_type pos = string::npos;
        string::size_type beg = 0;
        while ( (pos = strLine.find(",", beg)) != string::npos ) {
            string strnum = strLine.substr(beg, (pos - beg) );
            try {
                int num = stoi( strnum );
                vAry.push_back( num );
                ++cnt;
            } catch (... ) {
                cout << "[ERROR] : can't convert \"" << strnum << "\" to int. " << endl;
            }

            beg = pos + 1;
        }


        string strnum = strLine.substr(beg);
        try {
            int num = stoi( strnum );
            vAry.push_back( num );
            ++cnt;
        } catch ( ... ) {
            cout << "[ERROR] : can't convert \"" << strnum << "\" to int. " << endl;
        }
        cout << "@"  << lineNo << ". parse "  << cnt << " numbers. " << endl;

        int idx1 = 0;
        pAry = new int[ cnt ];

        idx1 = 0;
        for ( auto e : vAry ) {
            pAry[idx1++] = e;
        }
        vAry.clear();

        
		int errorCnt = 0; (void)errorCnt;
        // HeapSort2( pAry, cnt, &errorCnt);
        HeapSort2( pAry, cnt);
        if ( std::is_sorted( pAry,   pAry + (cnt) ) ) {
            ++sortCnt;
            cout << "line : " << lineNo << " is sorted . "; 
			if ( errorCnt > 0 ) {
				cout << "errorCnt = " << errorCnt;
			}
			cout << endl;
        } else {
            ++unsortCnt;
            cout << "line : " << lineNo << " is unsorted . "; 
			if ( errorCnt > 0 ) {
				cout << "errorCnt = " << errorCnt;
			}
			cout << endl;
        }
    }


    if ( pAry != nullptr ) {
        delete [] pAry;
        pAry = nullptr;
    }
    f.close();


    cout << "lineNo = " << lineNo << endl;
    cout << "\t  sorted.count = " << sortCnt   << endl;
    cout << "\tunsorted.count = " << unsortCnt << endl;
}






int main ( int argc, char *argv[], char *env[])
{
    if ( argc >= 2 ) {
		cout << "Use Method - 2" << endl;
        // testLineByLine1( string(argv[1]) );
        testLineByLine2( string(argv[1]) );
    }

	return 0;
}