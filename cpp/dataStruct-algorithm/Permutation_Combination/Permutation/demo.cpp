#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
using namespace std;

struct Slot
{
	vector<string> pool;
};

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

*/
void enumerateSlot(const vector<Slot*>& slotAry, int slotIdx, vector< vector<int> >& used, vector<string>& grp, vector<vector<string>>& pp)
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

void enumerateSlotAry(const vector<Slot*>& slotAry)
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

	// print possibilityPool;
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

void permutationDemo()
{
	// init
	vector<Slot*> slotAry;

	auto s0 = new Slot;
	s0->pool.push_back("A");
	s0->pool.push_back("B");
	s0->pool.push_back("C");

	auto s1 = new Slot;
	s1->pool.push_back("D");
	s1->pool.push_back("E");
	s1->pool.push_back("F");
	s1->pool.push_back("G");

	auto s2 = new Slot;
	s2->pool.push_back("H");
	s2->pool.push_back("I");
	s2->pool.push_back("J");
	s2->pool.push_back("K");
	s2->pool.push_back("L");

	auto s3 = new Slot;
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



int main(int argc, char* argv[], char* env[])
{
	permutationDemo();
	return 0;
}