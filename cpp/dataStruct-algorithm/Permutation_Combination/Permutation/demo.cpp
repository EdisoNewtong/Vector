#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
using namespace std;

struct Slot
{
	vector<string> pool;
};

void enumerateSlot(const vector<Slot*>& slotAry, int deep, vector< vector<int> >& used, vector<string>& grp, vector<vector<string>>& pp) 
{
	int slotSz = static_cast<int>( slotAry.size() );
	if ( deep > slotSz ) {
		return;
	}

	if ( deep == slotSz ) {
		pp.push_back( grp );
		return;
	}

	for ( size_t i = 0; i < slotAry.at(deep)->pool.size(); ++i ) {
		string content = slotAry.at(deep)->pool.at(i);
		if ( used[deep][i] == 0 ) {
			used[deep][i] = 1;
			grp[deep] = content;
			enumerateSlot(slotAry, deep + 1, used, grp, pp);
			used[deep][i] = 0;
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
int getNumberBitInDecimal(unsigned long long cnt) 
{
	int ret = 0;
	if ( cnt == 0 ) {
		ret = 1;
		return ret;
	}
	
	while ( cnt > 0 ) {
		++ret;
		cnt /= 10;
	}
	return ret;
}

void enumerateSlotAry(const vector<Slot*>& slotAry)
{
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

	vector<string>         possibilityGrp;
	possibilityGrp.resize( slotAry.size() );

	vector<vector<string>> possibilityPool;

	//
	// Core Core Core , Do real permutation function
	//
	enumerateSlot(slotAry, 0,   usedStatus, possibilityGrp, possibilityPool);

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

	//
	// Core Core Core
	//
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

////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin 
//
//
// Question:     
//     There are 4 numbers 1,2,3,4 and  4 slots 
//     fill any one number into the slot until all slots are filled
//     How many possibilities exists ?
//
static int testAry[] = { 1,2,3,4 };
static int flags[]   = { 0,0,0,0 };
static int retAry[]  = { 0,0,0,0 };

void perm(int deep, int cnt)
{
	if ( deep > cnt ) {
		cout << "case 1 : deep > cnt" << endl;
		return;
	}

	if ( deep == cnt ) {
		for ( int i = 0; i < cnt; ++i ) {
			cout << retAry[i];
			if ( i != cnt-1 ) {
				cout << " - ";
			}
		}
		cout << endl;
		return;
	}

	for ( int i = 0; i < cnt; ++i ){
		if ( flags[i] == 0 ) {
			flags[i] = 1;
			retAry[deep] = testAry[i];
			perm( deep+1, cnt);
			flags[i] = 0;
		}
	}
}

void permutation2()
{
	perm(0, sizeof(retAry) /  sizeof(retAry[0]) );
}

// End
////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[], char* env[])
{
	// permutationDemo();
	permutation2();
	return 0;
}
