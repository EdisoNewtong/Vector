#include <iostream>
#include <iomanip>
#include <list>
#include <string>
#include <iterator>
using namespace std;

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
	Slot* slotAry = new Slot[nPicked];
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
		cout << setw(3) << (possibilities + 1) << ". ";
		for( int slotIdx = 0; slotIdx < nPicked; ++slotIdx ) {
			auto step = slotAry[slotIdx].loopIdx;

			auto it = slotAry[slotIdx].avalibleCandidatePool.begin();
			advance(it, step);
			cout << *it << (slotIdx < (nPicked-1) ? "-" : "");
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



int main(int argc, char* argv[], char* env[])
{
    if ( argc <= 2 ) {
        cout  << "[ERROR] Missing a given nonnegative number " << endl;
        return -1;
    }

	auto nPossibilities = Permutation( stoi(argv[1]), stoi( argv[2] ) );
	cout << "==================================================" << endl;
	cout << "There are " << nPossibilities << " possibilites. " << endl;
	cout << "==================================================" << endl;

    return 0;
}