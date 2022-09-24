#include <iostream>
#include <iomanip>
#include <string>
#include <stack>
using namespace std;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin 
//
//
// Question:     
//     There are m characters  A,B,C,D, ...,  and  n slots s1,s2,s3, ... , sn
//     fill any character into the slot until all slots are filled
//
// Notes: 
//         A,B,C / A,C,B / B,A,C / B,C,A / C,A,B / C,B,A   are treat as 1 kind
//
//     How many possibilities exists ?
//


namespace V1  // ( recusively Call )
{


static int g_nPossibilityCnt = 0;

static char*   pickupPool = nullptr;
static int*    flagsAry   = nullptr;

static char*   retAry     = nullptr;

void combination(int deep, int startIdx, int nPicked, int nPoolsz)
{
	//
	// ==== Core Core Core ====
	// Recursive termate condition
	//
	if ( deep == nPicked ) {
		// print result array
		cout << setw(4) << (g_nPossibilityCnt+1) << ". ";
		for ( int i = 0; i < nPicked; ++i ) {
			cout << retAry[i];
			if ( i != nPicked-1 ) {
				cout << " - ";
			}
		}
		cout << endl;

		++g_nPossibilityCnt;
		return;
	}

	//
	// Core Core Core
	//
	//     3
	//   C 
	//     9
	//                          maxBound
	//   slot-0  : [0,6] => [0,  7   )      7 = 9 - 3 + 1 + 0 = 7
	//   slot-1  : [1,7] => [1,  8   )      8 = 9 - 3 + 1 + 1 = 8
	//   slot-2  : [2,8] => [2,  9   )      9 = 9 - 3 + 1 + 2 = 9
	//
	int maxBound = (nPoolsz - nPicked + 1 + deep);

	for ( int i = startIdx; i < maxBound; ++i ){
		if ( flagsAry[i] == 0 ) {
			// set flag
			flagsAry[i] = 1;

			// pick up one
			retAry[deep] = pickupPool[i];

			combination( deep+1, i+1, nPicked, nPoolsz);
			// clear flag
			flagsAry[i] = 0;
		}
	}


}

void runCombination(int nPicked, int nPoolSize)
{

    pickupPool = new char[nPoolSize];
    for( int i = 0; i < nPoolSize; ++i ) { pickupPool[i] = ('A' + i); }

    flagsAry   = new int[nPoolSize];
    for( int i = 0; i < nPoolSize; ++i ) { flagsAry[i] = 0; }

    retAry     = new char[nPicked];
    for( int i = 0; i < nPicked; ++i ) { retAry[i] = 0; }

	combination(0,0,  nPicked, nPoolSize );

    delete [] pickupPool;
    delete [] flagsAry;
    delete [] retAry;

	// cout << "==================================================" << endl
	// 	 << "\tThere are " << g_nPossibilityCnt << " possiblities in all. " << endl
	// 	 << "==================================================" << endl;
}


} // end namespace V1 ( recursively Call )


namespace V2  // ( none-recursively Call )
{

int runCombination(int nPick, int nPoolSize)
{
	int nPossibilities = 0;
	char* elementAry = new char[nPoolSize];
	int idx = 0;
	for( idx = 0; idx < nPoolSize; ++idx ) {
		elementAry[idx] = ('A' + idx);
	}

	char* slotAry = new char[nPick];
	int*  flagAry = new int[nPoolSize];
	for( idx = 0; idx < nPoolSize; ++idx ) { 
		flagAry[idx] = -1;
	}

	int slotIdx = 0;
	int avaliableIdx = 0;
	slotAry[slotIdx] = elementAry[avaliableIdx];
	flagAry[slotIdx] = avaliableIdx;

	stack<int> stk;
	stk.push(0);

	while ( !stk.empty() )
	{
		int stksz = static_cast<int>( stk.size() );

		if ( stksz == nPick )
		{
			slotIdx = stk.top();
			stk.pop();

			cout << setw(4) << (++nPossibilities) << ". ";
			for( idx = 0; idx < nPick; ++idx )
			{
				cout << slotAry[idx] << ((idx < nPick-1) ? " - " : "");
			}
			cout << endl;
		}
		else
		{
			avaliableIdx = -1;
			do {
				
				for( idx = slotIdx+1 ; idx < nPoolSize; ++idx ) {
					if ( flagAry[idx] == -1 ) {
						break;
					}
				}

				if ( idx < nPoolSize ) {
					avaliableIdx = idx;
				} else {
					if ( !stk.empty() ) {
						slotIdx = stk.top();
						stk.pop();

						for( idx = slotIdx+1; idx < nPoolSize ; ++idx )
						{
							flagAry[idx] = -1;
						}
					} else {
						avaliableIdx = -2;
					}
				}
			} while ( avaliableIdx == -1 );

			if ( avaliableIdx >= 0 ) {
				stksz = static_cast<int>( stk.size() );
				stk.push( avaliableIdx );

				flagAry[ avaliableIdx ] = stksz;
				slotAry[ stksz ] = elementAry[ avaliableIdx ];
			}
		}
	}


	delete [] elementAry;
	delete [] slotAry;
	delete [] flagAry;

	return nPossibilities;
}

} // end namespace V2 ( none-recursively Call )



int main(int argc, char* argv[], char* env[])
{
    if ( argc < 3 ) {
        cerr << "[ERROR] require 2 arguments. " << endl;
        return -1;
    }

    int nPicked = stoi(argv[1]);
    int nPoolSz = stoi(argv[2]);

	// V1::runCombination( nPicked, nPoolSz );

	// or 
	V2::runCombination( nPicked, nPoolSz );
    
	return 0;
}