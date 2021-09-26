#include <iostream>
#include <string>
#include <iomanip>
#include <list>
#include <fstream>
using namespace std;


namespace 
{
	static bool g_checkBoundFlag = false;
	static bool g_checkBoundLogFlag = true;
	// static bool g_checkOutOfBoundReturnFlag = false;


	// true  , smaller --> bigger
	// false , bigger  --> smaller
	static bool g_isAscendOrder = true;
}

bool checkIsInArrayRange(int idx, int sz, const char* varname)
{
	auto isInRange = (idx >=0 && idx < sz);
	if ( !isInRange && g_checkBoundLogFlag ) {
		cout << "[WARING] : " << varname << " is out of range [ 0, " << sz  << " ] , " << varname << " = " << idx << endl;
	}
	return isInRange;
}


void insertSorting(int* ary, int sz, bool isAscendOrder)
{
	if ( sz <= 1 ) {
		return;
	}

	/*
		The Sorting Process is like draw cards ( Let's assume that you use the ascending order )

		1. There is no cards in your hand
		2. You draw the 1st card ( is sorted )
		3. You draw the 2nd card, and compare from the right to left , 
					if   : the current drawn card's number is smaller than the previous card, then move the previous card to the next position and make a new slot for the drawn card 
		   <Repeat> the if statement check ,  until the if condition doesn't meet the demand
					else : place the current drawn card into the position ( it maybe the new created slot metioned in the "if" statement or the travelsal index )
		   
		   After that the card sequences ( a 2-cards seq ) is well organized by the ascending order

		4. You draw the 3rd card, and compare from the right to left following the step3
		5. You draw the 4th card, and compare from the right to left following the step3

			 ...
			 ...
			 ...

		n. You draw the N-th card, and compare from the right to left following the step3

	*/

    // range from the [2nd, nth)
	for ( int i = 1; i < sz; ++i ) {
        // current draw card
		int currentNumber = ary[i];
		int j = i;
		//
		// Compare from the current right to the most left 
		//     The most left (index = 0) <=== the current idx 
		for ( ; j > 0 &&   (isAscendOrder 
					     ? ary[j-1] > currentNumber
						 : ary[j-1] < currentNumber); --j ) {
			// check index is in the range or not    first
			if ( g_checkBoundFlag ) {
				checkIsInArrayRange(j-1, sz, " j-1 ");
			}

			if ( g_checkBoundFlag ) {
				checkIsInArrayRange(j, sz, " j ");
			}

			// <Move> the number in the iteratored slot into   next slot
			// and make an avalible slot for the current number which is to be inserted
			//
			// Rather than <Swap> 2 compared numbers in 2 slots
			//
			ary[j] = ary[j-1];
		}

		// check index is in the range or not    first
		if ( g_checkBoundFlag ) {
			checkIsInArrayRange(j, sz, " j ");
		}

		// place the current number in its proper index
		ary[j] = currentNumber;
	}
}

bool checkIsSorted(int* ary,int sz,bool isAscendOrder)
{
	auto isSort = true;
	for ( auto i = 0; i < sz-1 ; ++i ) {
		bool meetError = isAscendOrder 
				         ?  ary[i] > ary[i+1]
				         :  ary[i] < ary[i+1];

		if ( meetError ) {
			isSort = false;
			break;
		}
	}

	return isSort;
}

void testSort(bool isAscOrder)
{
    g_isAscendOrder = isAscOrder;

	cout << "==================================================" << endl;
	if ( g_isAscendOrder ) {
		cout << "Sorting by the <Ascending>  Order " << endl;
	} else {
		cout << "Sorting by the <Descending> Order " << endl;
	}
	cout << "==================================================" << endl;

	const char* FILENAME = "1_5.txt";

	ifstream fobj(FILENAME ,ios::in);
	if ( !fobj ) {
		cout << "Can't open file " << FILENAME << endl;
		return;
	}

	int lineNo = 0;
	while ( !fobj.eof() ) 
	{
		string line;
		getline(fobj, line);
		if ( line.empty() ) {
			continue;
		}
		++lineNo;

		string::size_type beg = 0;
		string::size_type foundpos;

		list<int> lary;
		while( (foundpos = line.find(",",beg) ) != string::npos ) {
			string strnum = line.substr(beg, foundpos);
			int num = std::stoi( strnum );
			lary.push_back(num);
			beg = foundpos + 1;
		}

		// push rest one
		string reststr = line.substr(beg);
		int num = std::stoi( reststr );
		lary.push_back(num);

		int arysz = static_cast<int>( lary.size() );
		int* pbackupAry = new int[arysz];
		int* pary = new int[arysz];
		int idx = 0;
		for ( const auto& ele : lary  ) {
			pbackupAry[idx] = pary[idx] = ele;
			++idx;
		}

		cout << "Line " << setw(5) << lineNo << " : [ ";
		for ( int i = 0; i < arysz; ++i ) {
			cout << pbackupAry[i];
			if ( i < arysz-1 ) {
				cout << ", ";
			}
		}
		cout << " ]    "; // << endl;

		insertSorting(pary, arysz, g_isAscendOrder);
		
		auto bIsSorted = checkIsSorted( pary, arysz , g_isAscendOrder);
		if ( bIsSorted ) {
			cout << "=> Sort SUCC : contains " << arysz << " elements. " << endl;
		} else {
			cout << "=> Sort Failed   :(   ==> contains " << arysz << " elements" << endl;
			cout << "After Sort : " << endl;
			cout << "\t [ ";
			for ( int i = 0; i < arysz; ++i ) {
				cout << pary[i];
				if ( i < arysz-1 ) {
					cout << ", ";
				}
			}
			cout << " ]    " << endl;
		}

		delete [] pbackupAry;
		delete [] pary;
	}

	fobj.close();
}


void printUsage()
{
	cout << "Usage : " << endl
		 << "==================================================" << endl
		 << " $ ./main asc " << endl
		 << "or ./main desc" << endl
		 << "==================================================" << endl
		 << endl;
}



int main(int argc, char* argv[], char* env[])
{
	if ( argc != 2 ) {
		cout << "[ERROR] : Missing args!!!" << endl;
		printUsage();
		return -1;
	}

	// "asc"  : ascending  order  => true   
	// "desc" : descending order  => false
	auto isAsc = (string(argv[1]) == "asc");

	testSort(isAsc);

	return 0;
}






