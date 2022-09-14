#include <iostream>
#include <stack>
#include <list>
#include <string>
#include <iterator>
using namespace std;

struct Slot
{
	int  loopIdx;
	list<char> avalibleCandidatePool;
};



/**
     Non Recursive Version for   Full-Permutation
	 Such as:
                 3       4           n
				P       P    ...    P
				 3       4           n
	            
*/
unsigned long long PermutationFull(int num)
{
	unsigned long long possibilites = 0;
	list<char> collections;
	char chBeg = 'A';
	for ( int idx = 0; idx < num; ++idx ) {        //  'A',  'B',  'C', 'D'
		collections.push_back( chBeg + idx * 1 ); 
	}

	Slot* pSlotAry = new Slot[num];
	for( int i = 0; i < num; ++i )
	{
		pSlotAry[i].loopIdx = 0;
		pSlotAry[i].avalibleCandidatePool.clear();
	}


	for( auto it = collections.cbegin(); it != collections.cend(); ++it ) {
		pSlotAry[0].avalibleCandidatePool.push_back( *it );
	}


	// int addFlag = false;
	int increaseIdxFlag = -1;
	while ( pSlotAry[0].loopIdx < static_cast<int>( pSlotAry[0].avalibleCandidatePool.size() ) )
	{
		if ( increaseIdxFlag>=0   &&  increaseIdxFlag < num ) {
			// normally increase 1
			pSlotAry[increaseIdxFlag].loopIdx += 1;
			int incIdx = pSlotAry[increaseIdxFlag].loopIdx;
			if ( incIdx >= static_cast<int>( pSlotAry[increaseIdxFlag].avalibleCandidatePool.size() ) ) {
				if ( increaseIdxFlag != 0 ) {
					pSlotAry[increaseIdxFlag].loopIdx = 0; // shift to beginning index
					increaseIdxFlag	= increaseIdxFlag - 1;
				} else {
					break;
				}
				continue;
			} else {
				// increaseIdxFlag = -1;
			}
		}

		cout << (possibilites + 1) << ". ";

		for( int slotIdx = 0; slotIdx < num; ++slotIdx ) 
		{
			int innerIdx = pSlotAry[slotIdx].loopIdx;

			auto listIt = pSlotAry[slotIdx].avalibleCandidatePool.begin();
			std::advance(listIt,innerIdx);

			// fill element into current  Slot
			cout << *listIt << ( slotIdx!=(num-1) ? " - " : "" );

			if ( slotIdx == (num - 1) ) {
				cout << endl;
				increaseIdxFlag = slotIdx - 1; // only forward iterator "once"  for the previous layer
				++possibilites;
			} else {
				if ( increaseIdxFlag != -1 ) {
					if ( slotIdx >= increaseIdxFlag && (slotIdx+1 <= num) ) { 
						if ( slotIdx == num -1 ) {
							increaseIdxFlag = -1;
						} else {
							pSlotAry[slotIdx+1].avalibleCandidatePool = pSlotAry[slotIdx].avalibleCandidatePool;
							auto listItNext = pSlotAry[slotIdx+1].avalibleCandidatePool.begin();
							std::advance(listItNext, innerIdx);
							pSlotAry[slotIdx+1].avalibleCandidatePool.erase( listItNext );
						}
					}
				} else {
					pSlotAry[slotIdx+1].avalibleCandidatePool = pSlotAry[slotIdx].avalibleCandidatePool;
					auto listItNext = pSlotAry[slotIdx+1].avalibleCandidatePool.begin();
					std::advance(listItNext, innerIdx);
					pSlotAry[slotIdx+1].avalibleCandidatePool.erase( listItNext );
					pSlotAry[slotIdx+1].loopIdx = 0;
				}
			}
		}

	}

	delete [] pSlotAry;
	
	return possibilites;
}



int main(int argc, char* argv[], char* env[])
{
    if ( argc <= 1 ) {
        cout  << "[ERROR] Missing a given nonnegative number " << endl;
        return -1;
    }

	auto nPossibilities = PermutationFull( stoi(argv[1]) );
	cout << "==================================================" << endl;
	cout << "There are " << nPossibilities << " possibilites. " << endl;
	cout << "==================================================" << endl;

    return 0;
}