#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <list>
#include <stack>
#include <iterator>
using namespace std;


/**
 * Recusively Call
 *
     利用递归调用 ，实现 乘法原理 :

	     完成1件事需要分n个步骤， 而每一个步骤会可以选择多种方法来完成

	Event :
		     event-1 : n1
		     event-2 : n2
		     event-3 : n3
		     event-4 : n4
		         ...
		     event-n : nn

		 那么完成整个 Event 总共有 :  n1 * n2 * n3 * n4 * ... * nn 种方法


----------------------------------------------------------------------------------------------------

目前有2种类型的池子，

A. 各个槽位上的池子是共享同一个 pool，即，往 Slot#1 放置 element-1 后，element-1 从 pool 中移除，pool中只剩下 element-2, element-3, ... , element-n ( 共 n-1 个元素 )
B. 各个槽位上的池子是使用独立的 pool, 即 Slot#1 从 pool#1 中抽取， Slot#2 从 pool#2 中抽取，<彼此都是互相独立的>
   e.g.
	   A-1.  一个2位数，每一位上都可以从数字池 0-9 中进行选择，但是抽完某一个数字后，第2位就不能出现重复的数字了。即 10 ~ 99 中， 11,22,33,44,55,66,77,88,99 不满足这题目的条件 ， 共有 90-9 = 81种可能性

       B-2.  一个2位数，每一位上都可以从数字池 0-9 中进行选择，而且池子中每一种数字的数量不限，那么一共有 10 ~ 99 ，即90种可能
	   B-2.  A地到C地，需要先经过B地， A --> B 有4种交通工具可供选择， B --> C 有2种交通工具可供选择 ，那么一共有 4*2 = 8 种情况



*/


namespace IndependentPool // ( Recusively Version )
{

struct Slot
{
	vector<string> pool;
};

void enumerateSlot(const vector<IndependentPool::Slot*>& slotAry, int slotIdx, vector< vector<int> >& used, vector<string>& grp, vector<vector<string>>& pp)
{
	int slotSz = static_cast<int>( slotAry.size() );
	// recursive end condition
	if ( slotIdx == slotSz ) {
		pp.push_back( grp );
		return;
	}

	for ( size_t elementIdx = 0; elementIdx < slotAry.at(slotIdx)->pool.size(); ++elementIdx ) {
		string content = slotAry.at(slotIdx)->pool.at(elementIdx);
		if ( used[slotIdx][elementIdx] == 0 ) {
			// Core Core Core 
			//     <Set> used flag ### Before ### recursive function
			used[slotIdx][elementIdx] = 1;
			grp[slotIdx] = content;

			// Recursively Call
			enumerateSlot(slotAry, slotIdx + 1, used, grp, pp);

			//     <Unset>  used flag ### After ### recursive function
			used[slotIdx][elementIdx] = 0;
		}
	}
}

/* 
 * get the bit for a decimal number
 * e.g.
 *     0         => 1
 *     5         => 1
 *     12        => 2
 *     315       => 3
 *     1435      => 4
 *     21435     => 5
 *
 *         ....
 *         ....
 *         ....
 *
 *     123456789 => 9
*/
int getNumberBitInDecimal(unsigned long long num) 
{
	int nBits = 0;
	do {
		num /= 10;
		++nBits;
	} while ( num != 0 );
	return nBits;
}

void enumerateSlotAry(const vector<IndependentPool::Slot*>& slotAry)
{
	// init used flag 
	vector< vector<int> >  usedStatus;
	for ( const auto pSlot : slotAry ) {
		vector<int> flags;
		if ( pSlot != nullptr ) {
			for ( size_t i = 0; i < pSlot->pool.size(); ++i ) {
				flags.push_back(0);
			}
		}
		usedStatus.push_back( flags );
	}

	//  One of the possibility 
	vector<string>         possibilityGrp;
	possibilityGrp.resize( slotAry.size() );

	// a collection set of  possibility
	vector<vector<string>> possibilityPool;

	//
	// Core Core Core , Do real permutation function
	//
	int initSlotIdx = 0;
	enumerateSlot(slotAry, initSlotIdx,   usedStatus, possibilityGrp, possibilityPool);



	//////////////////////////////////////////////////
	//
	// print possibilityPool;
	//
	unsigned long long cnt = possibilityPool.size();
	unsigned long long grpIdx = 0ULL;
	auto nBits = getNumberBitInDecimal(cnt);
	for ( const auto& grp : possibilityPool)
	{
		cout << setw(nBits) << (grpIdx + 1) << ". ";
		for ( size_t i = 0; i < grp.size(); ++i ) {
			cout << grp.at(i);
			if ( i != grp.size()-1 ) {
				cout << " - ";
			}
		}
		cout << endl;
		++grpIdx;
	}
}

void PermutationDemo()
{
	// init
	vector<IndependentPool::Slot*> slotAry;

	auto s0 = new IndependentPool::Slot;
	s0->pool.push_back("A");
	s0->pool.push_back("B");
	s0->pool.push_back("C");

	auto s1 = new IndependentPool::Slot;
	s1->pool.push_back("D");
	s1->pool.push_back("E");
	s1->pool.push_back("F");
	s1->pool.push_back("G");

	auto s2 = new IndependentPool::Slot;
	s2->pool.push_back("H");
	s2->pool.push_back("I");
	s2->pool.push_back("J");
	s2->pool.push_back("K");
	s2->pool.push_back("L");

	auto s3 = new IndependentPool::Slot;
	s3->pool.push_back("M");
	s3->pool.push_back("N");
	s3->pool.push_back("O");
	s3->pool.push_back("P");
	s3->pool.push_back("Q");
	s3->pool.push_back("R");

	slotAry.push_back(s0);
	slotAry.push_back(s1);
	slotAry.push_back(s2);
	slotAry.push_back(s3);

	
	// Core Core Core :
	//      Key function
	enumerateSlotAry(slotAry);

	// release
	for ( auto pSlot : slotAry)
	{
		if ( pSlot != nullptr ) {
			delete pSlot;
		}
	}
	slotAry.clear();
}

} // end namespace IndependentPool   ( Recusively Version )




namespace SharedPool1 // recusively Version
{

static int g_nPossibilityCnt = 0;

static char* g_pickupPool = nullptr;
// g_flagsAry must has the same size of g_pickupPool
static int*  g_flagsAry   = nullptr;

// with the size of nPicked
static char* g_retAry     = nullptr;

void Permutation(int deep,  int nPicked, int nPoolCnt)
{
	//
	// ==== Core Core Core ====
	// Recursive termate condition
	//
	if ( deep == nPicked ) {
		// print result array
		cout << setw(4) << (g_nPossibilityCnt+1) << ". ";
		for ( int i = 0; i < nPicked; ++i ) {
			cout << g_retAry[i];
			if ( i != nPicked-1 ) {
				cout << " - ";
			}
		}
		cout << endl;

		++g_nPossibilityCnt;
		return;
	}

	for ( int i = 0; i < nPoolCnt; ++i ){
		if ( g_flagsAry[i] == 0 ) {
			// set flag
			g_flagsAry[i] = 1;

			// pick up one
			g_retAry[deep] = g_pickupPool[i];

			SharedPool1::Permutation( deep+1, nPicked, nPoolCnt);
			// clear flag
			g_flagsAry[i] = 0;
		}
	}


}

/*
    There are 4 slots  , A B C D 
    and  4 numbers in the poll { 1,2,3,4 }
 
    Step-1 : A is filled with 1 ,then the rest 3 slots B/C/D can pick from the pool with (2,3,4) only , can only filled with one of  {   2, 3, 4 } without 1
    Step-2 : B is filled with 2 ,then the rest 2 slots   C/D can pick from the pool with (  3,4) only , can only filled with one of  {      3, 4 } without 1 and 2
       ...
 
 
    This example is different from 4 A,B,C,D slots, with 4 numbers {1,2,3,4} , some possibilities is shown below :

	------------------
	   A  B  C  D
	   1  1  1  1
	------------------
	   A  B  C  D
	   2  2  2  2

	   ....

	------------------
	   A  B  C  D
	   1  2  2  1
	------------------



*/


// Permutation
void PermutationRun(int nPicked, int nPoolSz)
{
	g_nPossibilityCnt = 0ull;
	//   3
	// P 
	//   4
	g_pickupPool = new char[nPoolSz];
	for( int i = 0; i < nPoolSz; ++i ) {
		g_pickupPool[i] = ('A' + i);
	}

	g_flagsAry   = new int[nPoolSz];
	for( int i = 0; i < nPoolSz; ++i ) {
		g_flagsAry[i] = 0;
	}

	g_retAry     = new char[nPicked];
	for( int i = 0; i < nPicked; ++i ) {
		g_retAry[i] = 0;
	}

	SharedPool1::Permutation(0, nPicked, nPoolSz );

	delete [] g_pickupPool;
	delete [] g_flagsAry;
	delete [] g_retAry;

	// cout << "==================================================" << endl
	// 	 << "\tThere are " << g_nPossibilityCnt << " possiblities in all. " << endl
	// 	 << "==================================================" << endl;
}

} // end namespace SharedPool1  ( Recusively Version )




namespace SharedPool2 // none-recusively Version
{

struct Slot
{
	int         loopIdx;
	list<char>  avalibleCandidatePool;
};




/**
     Non Recursive Version for   Permutation
	 Such as:
                  3     
				P       
				  4     
	            
*/
unsigned long long Permutation(int nPicked, int nCandidates)
{
	if ( nPicked > nCandidates ) {
		cerr << "[ERROR] nPicked > nPoolSize is not valid. " << endl;
		return 0ULL;
	}

	list<char> pool;
	for( int i = 0; i < nCandidates; ++i ) { pool.push_back( ('A' + i) ); }

	unsigned long long possibilities = 0ULL;
	SharedPool2::Slot* slotAry = new SharedPool2::Slot[nPicked];
	for( int i = 0; i < nPicked; ++i ) {
		// init loopIdx
		slotAry[i].loopIdx = 0;
		slotAry[i].avalibleCandidatePool.clear();

		if ( i == 0 ) {
			slotAry[i].avalibleCandidatePool = pool;
		} else {
			// copy previous pool
			slotAry[i].avalibleCandidatePool = slotAry[i-1].avalibleCandidatePool;
			// remove the 1st element
			slotAry[i].avalibleCandidatePool.erase( slotAry[i].avalibleCandidatePool.begin() );
		}
	}

	do {
		cout << setw(4) << (possibilities + 1) << ". ";
		for( int slotIdx = 0; slotIdx < nPicked; ++slotIdx ) {
			auto step = slotAry[slotIdx].loopIdx;

			auto it = slotAry[slotIdx].avalibleCandidatePool.begin();
			advance(it, step);
			cout << *it << (slotIdx < (nPicked-1) ? " - " : "");
		}
		cout << endl;
		++possibilities;

		// apply increasement for the last slot if necessary
		int jumpSlotIdx = nPicked - 1;
		bool bMoveNext = false;
		while ( !bMoveNext )
		{
			if ( jumpSlotIdx > 0 ) {
				if ( (slotAry[jumpSlotIdx].loopIdx + 1) < static_cast<int>( slotAry[jumpSlotIdx].avalibleCandidatePool.size() ) ) {
					++slotAry[jumpSlotIdx].loopIdx;
					bMoveNext = true;
				} else {
					--jumpSlotIdx;
				}
			} else {
				// jumpSlotIdx == 0
				++slotAry[jumpSlotIdx].loopIdx;
				bMoveNext = true;
			}

			if ( bMoveNext &&   slotAry[jumpSlotIdx].loopIdx < static_cast<int>( slotAry[jumpSlotIdx].avalibleCandidatePool.size() ) ) {
				for ( auto j = jumpSlotIdx; j < nPicked; ++j ) {
					if ( j + 1 < nPicked ) {
						slotAry[j + 1].avalibleCandidatePool = slotAry[j].avalibleCandidatePool;

						auto movedIt = slotAry[j + 1].avalibleCandidatePool.begin();
						// remove to be picked
						advance(movedIt, slotAry[j].loopIdx );
						slotAry[j + 1].avalibleCandidatePool.erase( movedIt);
						// reset idx
						slotAry[j + 1].loopIdx = 0;
					}
				}
			}
		}

	} while ( slotAry[0].loopIdx < nCandidates );

	delete [] slotAry;
	return possibilities;
}

} // end namespace SharedPool2  ( none-recusively )




namespace SharedPool3 // none-recusively Version
{

int Permutation(int nPicked, int nPoolSize)
{
	int nPossibilities = 0;

	char* elementPool = new char[nPoolSize];
	for ( int i = 0; i < nPoolSize; ++i ) {
		elementPool[i] = ('A' + i);
	}
	


	stack<int> stkIdx;
	int* usedStateAry = new int[nPoolSize];
	for( int i = 0; i < nPoolSize; ++i ) {
		usedStateAry[i] = -1;
	}

	char* slotFilledAry = new char[nPicked];

	int slotIdx = 0;
	int firstAvaliableIdx = 0;

	usedStateAry[ firstAvaliableIdx ] = slotIdx;
	slotFilledAry[ slotIdx ] = elementPool[ firstAvaliableIdx ];

	stkIdx.push( firstAvaliableIdx );
	int topIdx = stkIdx.top();
	while ( !stkIdx.empty() )
	{
		int stksz = static_cast<int>( stkIdx.size() );

		if ( stksz == nPicked ) {
			topIdx = stkIdx.top();
			stkIdx.pop();

			// output one group
			cout << setw(4) <<  (++nPossibilities) << ". ";
			for( int i = 0; i < nPicked; ++i ) {
				cout << slotFilledAry[i] << ( (i < nPicked-1) ? " - " : "");
			}
			cout << endl;
		} else {
			//
			// try to push next layer
			//
			int iNextAvaliable = -1;
			do {
				int next = topIdx + 1;
				for( ; next < nPoolSize; ++next ) {
					if ( usedStateAry[next] == -1 ) {
						iNextAvaliable = next;
						break;
					}
				}

				if ( next < nPoolSize ) {
					iNextAvaliable = next;
					topIdx = -1; // from index 0 to start loop
				} else {
					if ( !stkIdx.empty() ) {
						topIdx = stkIdx.top();
						stkIdx.pop();
						--stksz;

						// clear occupy flag
						for( int k = 0;  k < nPoolSize; ++k )
						{
							if ( usedStateAry[k] >= stksz ) {
								usedStateAry[k] = -1;
							}
						}

					} else {
						iNextAvaliable = -2; // found nothing at all
					}
				} 
			} while ( iNextAvaliable == -1 );

			if ( iNextAvaliable >= 0 ) {
				stkIdx.push(iNextAvaliable);

				usedStateAry[iNextAvaliable] = stksz;
				slotFilledAry[ stksz ]  = elementPool[ iNextAvaliable ];
			}
		}
	}




	delete [] elementPool;
	delete [] usedStateAry;
	delete [] slotFilledAry;

	return nPossibilities;
}

} // end namespace SharedPool3 ( none-recusively )

namespace SharedPool4  // none-recusively Version
{

int Permutation(int nPicked, int nPoolSize)
{
	int nPossibilities = 0;

	char* elementPool = new char[nPoolSize];
	for ( int i = 0; i < nPoolSize; ++i ) {
		elementPool[i] = ('A' + i);
	}
	
	stack<int> stkIdx;
	int* usedStateAry = new int[nPoolSize];
	for( int i = 0; i < nPoolSize; ++i ) {
		usedStateAry[i] = -1;
	}

	char* slotFilledAry = new char[nPicked];

	int slotIdx = 0;
	int firstAvaliableIdx = 0;

	usedStateAry[ firstAvaliableIdx ] = slotIdx;
	slotFilledAry[ slotIdx ] = elementPool[ firstAvaliableIdx ];

	stkIdx.push( firstAvaliableIdx );
	while ( !stkIdx.empty() )
	{
		int stksz = static_cast<int>( stkIdx.size() );

		if ( stksz == nPicked ) {
			int topIdx = stkIdx.top();
			stkIdx.pop();
			--stksz; // update size

			// output one group
			cout << setw(4) <<  (++nPossibilities) << ". ";
			for( int i = 0; i < nPicked; ++i ) {
				cout << slotFilledAry[i] << ( (i < nPicked-1) ? " - " : "");
			}
			cout << endl;

			int iNextAvaliable = -1;
			do {
				int nextAvaliable = topIdx + 1;
				for( ; nextAvaliable < nPoolSize; ++nextAvaliable ) {
					if ( usedStateAry[nextAvaliable] == -1 ) {
						break;
					}
				}

				if ( nextAvaliable < nPoolSize ) {
					iNextAvaliable = nextAvaliable;
					stkIdx.push( iNextAvaliable );

					usedStateAry[ iNextAvaliable ] = stksz;
					slotFilledAry[ stksz ] = elementPool[ iNextAvaliable ];
				} else {
					if ( !stkIdx.empty() ) {
						topIdx = stkIdx.top();
						stkIdx.pop();
						--stksz;

						// clear occupy flag
						for( int k = 0;  k < nPoolSize; ++k )
						{
							if ( usedStateAry[k] >= stksz ) {
								usedStateAry[k] = -1;
							}
						}
					} else {
						iNextAvaliable = -2; // found nothing at all
					}
				}

			} while ( iNextAvaliable == -1 );
		} else {
			// push next layer
			int iFoundIdx = -1;
			for( int i = 0; i < nPoolSize; ++i ) {
				if ( usedStateAry[i] == -1 ) {
					iFoundIdx = i;
					break;
				}
			}

			if ( iFoundIdx != -1 ) {
				stkIdx.push(iFoundIdx);

				usedStateAry[iFoundIdx] = stksz;
				slotFilledAry[ stksz ]  = elementPool[ iFoundIdx ];
			}
		}
	}




	delete [] elementPool;
	delete [] usedStateAry;
	delete [] slotFilledAry;

	return nPossibilities;
}


} // end namespace SharedPool4     ( none-recusively )




int main(int argc, char* argv[], char* env[])
{
	if ( argc == 1 ) {
		IndependentPool::PermutationDemo();
	} else {
		if ( argc > 2 ) {
			int nPicked = stoi( argv[1] );
			int nPoolsz = stoi( argv[2] );

			SharedPool1::PermutationRun( nPicked, nPoolsz);

			// or
			// SharedPool2::Permutation( nPicked, nPoolsz);

			// or 
			// SharedPool3::Permutation( nPicked, nPoolsz);
			
			// or 
			// SharedPool4::Permutation( nPicked, nPoolsz);
		} else {
			cerr << "[ERROR] Require 2 arguments" << endl;
		}
	}



	return 0;
}

