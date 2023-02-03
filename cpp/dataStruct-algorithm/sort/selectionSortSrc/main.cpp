#include <iostream>
#include <string>
#include <iomanip>
#include <list>
#include <fstream>
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
   Pick the min/max number , the place it into the 1st/2nd/3rd/ ... slot at the end of each turn

   After that the 1st/2nd/3rd slot must be the min/max , the 2nd min/max ,the 3rd min/max number among the array

   Iterator pointer move the the next through the whole array

		...
		...
		...


	遍历1次数组，在[0,n) 找到最大元素的下标，放在 [0]
	遍历2次数组，在[1,n) 找到最大元素的下标，放在 [1]
	遍历3次数组，在[2,n) 找到最大元素的下标，放在 [2]
	    ...

*/
void selectionSorting(int* ary, int sz, bool isAscendOrder)
{
	// if ( sz <= 1 ) {
	// 	return;
	// }

	for ( int i = 0; i < sz-1; ++i )
	{
		int targetNumberIndex = i;
		for ( int j = i+1; j < sz; ++j ) 
		{

			if ( g_checkBoundFlag ) {
				checkIsInArrayRange(j, sz, "j");
				checkIsInArrayRange(targetNumberIndex, sz, "targetNumberIndex");
			}

			auto bNeedSwap = isAscendOrder ? ary[j] < ary[targetNumberIndex]  :  ary[j] > ary[targetNumberIndex];
			if ( bNeedSwap ) {
				targetNumberIndex = j;
			}
		}

		if ( g_checkBoundFlag ) {
			checkIsInArrayRange(i, sz, "i");
			checkIsInArrayRange(targetNumberIndex, sz, "targetNumberIndex");
		}

		swap2Numbers( ary, i, targetNumberIndex, sz);
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

		selectionSorting(pary, arysz, g_isAscendOrder);
		
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



