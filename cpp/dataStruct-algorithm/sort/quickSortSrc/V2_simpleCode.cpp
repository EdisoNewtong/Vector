#include <iostream>
#include <string>
#include <iomanip>
#include <list>
#include <fstream>
using namespace std;

namespace 
{
	// 1      : qSortSimpleVersionHead(...) 
	// none 1 : qSortSimpleVersionTail(...)
	static int  g_headerVersion = 1;

	// true  , smaller --> bigger
	// false , bigger  --> smaller
	static bool g_isAscendOrder = true;
}




void swap2Numbers(int* ary, int idx1, int idx2, int sz)
{
	if ( idx1 == idx2 ) {
		return;
	}

	int backupNum = ary[idx1];
	ary[idx1] = ary[idx2];
	ary[idx2] = backupNum;
}

void qSortSimpleVersionHead(int* ary, int begIdx, int endIdx, int sz, bool isAscendOrder)
{
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

void testSort(bool isAscOrder, bool useHeadVersion)
{

    g_isAscendOrder = isAscOrder;
	g_headerVersion = useHeadVersion ? 1 : 2;

	cout << "==================================================" << endl;
	if ( g_isAscendOrder ) {
		cout << "Use Ascending  Order " << endl;
	} else {
		cout << "Use Descending Order " << endl;
	}

	if ( g_headerVersion == 1 ) {
		cout << "Use Pick Head Number " << endl;
	} else {
		cout << "Use Pick Tail Number " << endl;
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

		if ( g_headerVersion == 1 ) {
			qSortSimpleVersionHead(pary, 0, arysz-1, arysz, g_isAscendOrder);
		} else {
			qSortSimpleVersionTail(pary, 0, arysz-1, arysz, g_isAscendOrder);
		}
		auto bIsSorted = checkIsSorted( pary, arysz , g_isAscendOrder);
		if ( bIsSorted ) {
			cout << "=> Sort SUCC" << endl;
		} else {
			cout << "=> Sort Failed   :( " << endl;
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





int main(int argc, char* argv[], char* env[])
{
	if ( argc != 3 ) {
		cout << "Missing args" << endl;
		return -1;
	}

	// "asc"  : ascending  order  => true   
	// "desc" : descending order  => false
	auto isAsc = (string(argv[1]) == "asc");

	// "head"  : head version
	// "tail"  : tail version
	auto useHeadVersion = (string(argv[2]) == "head");

	testSort(isAsc, useHeadVersion);

	return 0;
}


