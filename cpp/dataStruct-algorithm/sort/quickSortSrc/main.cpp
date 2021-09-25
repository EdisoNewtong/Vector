#include <iostream>
#include <string>
#include <iomanip>
#include <list>
#include <fstream>
using namespace std;

extern void quickSort_OldVersion(int* pAry, int startIdx, int endIdx,int sz);

namespace 
{
	static bool g_checkBoundFlag = false;
	static bool g_checkBoundLogFlag = false;
	static bool g_checkOutOfBoundReturnFlag = false;

	// 1 : qSortSimpleVersionHead(...) 
	// 2 : qSortSimpleVersionTail(...)
	// 3 : quickSort_OldVersion(...) // implementation  the <Ascending Order>  Only
	static int  g_sortingFunctionVersion = 1;

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


// make array [1,2,3,4] =>  [4,3,2,1]
void reverseAry(int* ary,int sz)
{
	for( int i = 0; i < sz/2; ++i ) {
		int headidx = i;
		int tailidx = (sz-1) -i;
		swap2Numbers(ary, headidx, tailidx, sz);
	}
}
void qSortSimpleVersionHead(int* ary, int begIdx, int endIdx, int sz, bool isAscendOrder)
{
	if ( g_checkBoundFlag ) {
		auto b1 = checkIsInArrayRange(begIdx,sz, "begIdx");
		auto b2 = checkIsInArrayRange(endIdx,sz, "endIdx");
		if ( g_checkOutOfBoundReturnFlag && (!b1 || !b2) ) {
			return;
		}
	}

	if ( endIdx <= begIdx ) {
		return;
	}

	int povitNumber = ary[begIdx];
	int i = begIdx + 1;
	int j = endIdx;
	while ( i < j ) 
	{
		for ( ; i < j && (isAscendOrder ? ary[i] <= povitNumber
				                        : ary[i] >= povitNumber); ++i )
		{ ; }

		for ( ; j > i && (isAscendOrder ? ary[j] >= povitNumber 
				                        : ary[j] <= povitNumber); --j )
		{ ; }

		swap2Numbers(ary,i,j,sz);
		++i;
		--j;
	}

	int sepIdx = 0;
	if ( i == j ) {
		sepIdx = i;
	} else {
		// i > j
		sepIdx = i - 1;
	}

	auto condition = isAscendOrder ? ary[sepIdx] >= povitNumber 
		                           : ary[sepIdx] <= povitNumber;
	if ( condition ) {
		sepIdx = sepIdx - 1;
	}

	swap2Numbers(ary,begIdx,sepIdx,sz);

	// sort left
	qSortSimpleVersionHead(ary, begIdx, sepIdx - 1, sz, isAscendOrder);
	// sort right
	qSortSimpleVersionHead(ary, sepIdx + 1, endIdx, sz, isAscendOrder);
}

void qSortSimpleVersionTail(int* ary, int begIdx, int endIdx, int sz, bool isAscendOrder)
{

	if ( g_checkBoundFlag ) {
		auto b1 = checkIsInArrayRange(begIdx,sz, "begIdx");
		auto b2 = checkIsInArrayRange(endIdx,sz, "endIdx");
		if ( g_checkOutOfBoundReturnFlag && (!b1 || !b2) ) {
			return;
		}
	}

	if ( endIdx <= begIdx ) {
		return;
	}

	int povitNumber = ary[endIdx];
	int i = begIdx;
	int j = endIdx-1;
	while ( i < j ) 
	{
		for ( ; i < j && (isAscendOrder ? ary[i] <= povitNumber
				                        : ary[i] >= povitNumber); ++i )
		{ ; }

		for ( ; j > i && (isAscendOrder ? ary[j] >= povitNumber 
				                        : ary[j] <= povitNumber); --j )
		{ ; }

		swap2Numbers(ary,i,j,sz);
		++i;
		--j;
	}

	int sepIdx = 0;
	if ( i == j ) {
		sepIdx = i;
	} else {
		// i > j
		sepIdx = i - 1;
	}

	auto condition = isAscendOrder ? ary[sepIdx] <= povitNumber 
		                           : ary[sepIdx] >= povitNumber;
	if ( condition ) {
		sepIdx = sepIdx + 1;
	}

	swap2Numbers(ary,endIdx,sepIdx,sz);

	// sort left
	qSortSimpleVersionTail(ary, begIdx, sepIdx - 1, sz, isAscendOrder);
	// sort right
	qSortSimpleVersionTail(ary, sepIdx + 1, endIdx, sz, isAscendOrder);
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

void testSort(bool isAscOrder, int useHeadVersion)
{
    g_isAscendOrder = isAscOrder;
	g_sortingFunctionVersion = useHeadVersion;

	cout << "==================================================" << endl;
	if ( g_isAscendOrder ) {
		cout << "Sorting by the <Ascending>  Order " << endl;
	} else {
		cout << "Sorting by the <Descending> Order " << endl;
	}

	if ( g_sortingFunctionVersion == 1 ) {
		cout << "Use Pick <Head> Number <== as the povit Number " << endl;
	} else {
		cout << "Use Pick <Tail> Number <== as the povit Number " << endl;
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

		if ( g_sortingFunctionVersion == 1 ) {
			qSortSimpleVersionHead(pary, 0, arysz-1, arysz, g_isAscendOrder);
		} else if ( g_sortingFunctionVersion == 2 ) {
			qSortSimpleVersionTail(pary, 0, arysz-1, arysz, g_isAscendOrder);
		} else {
			quickSort_OldVersion(pary, 0, arysz-1, arysz);
			if ( !g_isAscendOrder ) {
				reverseAry(pary, arysz);
			}
		}

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
		 << " $ ./v2 asc  head" << endl
		 << "or ./v2 asc  tail" << endl
		 << "or ./v2 desc head" << endl
		 << "or ./v2 desc tail" << endl
		 << "or ./v2 asc other" << endl
		 << "==================================================" << endl
		 << endl;
}



int main(int argc, char* argv[], char* env[])
{
	if ( argc != 3 ) {
		cout << "[ERROR] : Missing args!!!" << endl;
		printUsage();
		return -1;
	}

	// "asc"  : ascending  order  => true   
	// "desc" : descending order  => false
	auto isAsc = (string(argv[1]) == "asc");

	// "head"  : head version
	// "tail"  : tail version
	int useWhichVersion = 0;
	string arg2(argv[2]);
	if ( arg2 == "head" ) {
		useWhichVersion = 1;
	} else if ( arg2 == "tail" ) {
		useWhichVersion = 2;
	} else {
		useWhichVersion = 3;
	}

	testSort(isAsc, useWhichVersion);

	return 0;
}




