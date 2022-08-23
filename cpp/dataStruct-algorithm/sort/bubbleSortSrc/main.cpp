#include <iostream>
#include <list>
#include <iomanip>
#include <fstream>
#include <string>
using namespace std;

namespace 
{
	static bool g_checkBoundFlag = true;
	static bool g_checkBoundLogFlag = true;
	static bool g_checkOutOfBoundReturnFlag = false;


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



void swap2Numbers(int* ary, int idx1, int idx2, int sz)
{
	if ( g_checkBoundFlag ) {
		auto b1 = checkIsInArrayRange(idx1,sz, "idx-1");
		auto b2 = checkIsInArrayRange(idx2,sz, "idx-2");
		if ( g_checkOutOfBoundReturnFlag && (!b1 || !b2) ) {
			return;
		}
	}
	
	if ( idx1 == idx2 ) {
		return;
	}

	int backupNum = ary[idx1];
	ary[idx1] = ary[idx2];
	ary[idx2] = backupNum;
}


/*
************************************************** 
   Assume sorting by <Ascending> Order
************************************************** 

   1st turn .  
        Compare every 2 adjacent numbers, if the left one is larger than the right one, swap 2 numbers, until the <Last> pair
           After the 1st turn, the last number is the most largest one among the array

   2nd turn .  
        Compare every 2 adjacent numbers, if the left one is larger than the right one, swap 2 numbers, until the <last But One> pair
           After the 2nd turn, the last but one number is the 2nd largest one among the array

        ...
        ...
        ...





e.g.  The count of an array is 5 , so there will be (5-1) = 4 turns in all
----------------------------------------------------------------------------------------------------
Turn Index           |    0     |    1     |     2    |     3    |
-------------------- --------------------------------------------------------------------------------
                     | 1st turn | 2nd turn | 3rd turn | 4th turn |
                     |   0-1    |   0-1    |   0-1    |    0-1   |
Compare Pair Detail  |   1-2    |   1-2    |   1-2    |          |
                     |   2-3    |   2-3    |          |          |
                     |   3-4    |          |          |          |
-------------------- --------------------------------------------------------------------------------
last pair's right :  |    4     |    3     |    2     |     1    | 
-------------------- --------------------------------------------------------------------------------
                     | (5-1)-0  | (5-1)-1  | (5-1)-2  | (5-1)-3  |

*/
void bubbleSort(int* ary, int sz, bool isAscendOrder)
{
    
    for ( int iTurnIdx = 0; iTurnIdx < (sz-1); ++iTurnIdx )
    {
        for ( int i = 0; i < ((sz - 1) - iTurnIdx); ++i )
        {
            if ( g_checkBoundFlag ) {
                checkIsInArrayRange(i, sz, " i ");
                checkIsInArrayRange(i+1, sz , " i + 1");
            }

            bool needSwap = isAscendOrder ? ary[i] > ary[i+1]
                                          : ary[i] < ary[i+1];
            if ( needSwap ) {
                swap2Numbers(ary, i, i+1, sz);
            }
        }
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

		bubbleSort(pary, arysz, g_isAscendOrder);
		
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


