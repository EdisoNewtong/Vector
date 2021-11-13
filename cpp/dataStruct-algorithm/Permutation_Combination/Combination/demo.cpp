#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
using namespace std;



////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin 
//
//
// Question:     
//     There are 4 numbers 1,2,3,4 and  4 slots 
//     fill any one number into the slot until all slots are filled
//     How many possibilities exists ?
//

static int g_nPossibilityCnt = 0;

static int pickupPool[] = { 1,2,3,4 };
static int flagsAry[]   = { 0,0,0,0 };
static int retAry[]     = { 0,0,0,0 };

void comination(int deep, int cnt)
{
	//
	// ==== Core Core Core ====
	// Recursive termate condition
	//
	if ( deep == cnt ) {
		// print result array
		cout << (g_nPossibilityCnt+1) << ". ";
		for ( int i = 0; i < cnt; ++i ) {
			cout << retAry[i];
			if ( i != cnt-1 ) {
				cout << " - ";
			}
		}
		cout << endl;

		++g_nPossibilityCnt;
		return;
	}

	for ( int i = 0; i < cnt; ++i ){
		if ( flagsAry[i] == 0 ) {
			// set flag
			flagsAry[i] = 1;

			// pick up one
			retAry[deep] = pickupPool[i];

			comination( deep+1, cnt);
			// clear flag
			flagsAry[i] = 0;
		}
	}


}

/*
    There are 4 slots  , A B C D 
    and  4 numbers in the poll { 1,2,3,4 }
 
    Step-1 : A is filled with 2 ,then the rest 3 slots B C D  , can only filled with one of  { 1,   3, 4 } without 1
    Step-2 : B is filled with 3 ,then the rest 2 slots C D    , can only filled with one of  { 1,      4 } without 2 and 3
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
void runCombinationDemo()
{
	comination(0, sizeof(retAry) /  sizeof(retAry[0]) );

	cout << "==================================================" << endl
		 << "\tThere are " << g_nPossibilityCnt << " possiblities in all. " << endl
		 << "==================================================" << endl;
}


int main(int argc, char* argv[], char* env[])
{
	runCombinationDemo();
	return 0;
}

