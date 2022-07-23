#include <iostream>
#include <string>
#include <iomanip>
#include <list>
#include <fstream>


// #define  INDEX_CHECK_SAFE


#ifdef INDEX_CHECK_SAFE
    #include "sortUtil_safe.h"
#else
    #include "sortUtil.h"
#endif

using namespace std;

///////////////////////////////////////////////////////////////////////////
//
// Global debug flag varible
//
///////////////////////////////////////////////////////////////////////////
bool G_checkArrayBoundFlag = true;     // default , Switch check array index   flag  is  [ON]
unsigned long long G_indexOutBoundCnt = 0;

bool G_quickSortRangeCheck = false;
bool G_quickSortOutOfRange_return = false;
unsigned long long G_quickSortRangesOutOfBoundCnt = 0;



namespace 
{

	// 1 : qSortSimpleVersionHead(...) 
	// 2 : qSortSimpleVersionTail(...)
	// 3 : quickSort_OldVersion(...) // implementation  the <Ascending Order>  Only
	static int  g_sortingFunctionVersion = 1;

	// true  , smaller --> bigger
	// false , bigger  --> smaller
	static bool g_isAscendOrder = true;

    static unsigned long long g_CaseCnt     = 0ULL;
    static unsigned long long g_CaseSuccCnt = 0ULL;
    static unsigned long long g_CaseFailed  = 0ULL;
    static list< list<int> >  g_CaseFailedCollection;

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
	} else if ( g_sortingFunctionVersion == 2 ) {
		cout << "Use Pick <Tail> Number <== as the povit Number " << endl;
	} else {
		cout << "[ERROR] : Unknown SortFunction Version " << endl;
    }
	cout << "==================================================" << endl;

    const int   C_FORMAT_WIDTH = 7;
    const string C_LINE_TITLE  = "Line ";
    const string C_COLON_FORMAT  = " : ";
    const int C_NEXT_LINE_INDENT_CNT = C_FORMAT_WIDTH + C_LINE_TITLE.size() + C_COLON_FORMAT.size();
	const char* FILENAME = "1_6.txt";

	ifstream fobj(FILENAME ,ios::in);
	if ( !fobj ) {
		cout << "Can't open file " << FILENAME << endl;
		return;
	}

    g_CaseFailedCollection.clear();
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

		cout << C_LINE_TITLE << setw(C_FORMAT_WIDTH) << lineNo << C_COLON_FORMAT <<"[ ";
		for ( int i = 0; i < arysz; ++i ) {
			cout << pbackupAry[i];
			if ( i < arysz-1 ) {
				cout << ", ";
			}
		}
		cout << " ]    "; // << endl;



#ifdef INDEX_CHECK_SAFE
		if ( g_sortingFunctionVersion == 1 ) {
			// qSortSimpleVersionHead(pary, 0, arysz-1, arysz, g_isAscendOrder);
            quickSortHead(pary, 0, arysz-1, arysz, g_isAscendOrder);
		} else {
			// qSortSimpleVersionTail(pary, 0, arysz-1, arysz, g_isAscendOrder);
            quickSortTail(pary, 0, arysz-1, arysz, g_isAscendOrder);
		} 
#else
        if (  g_isAscendOrder ) {
            if (  g_sortingFunctionVersion == 1 ) {
                // Pick <Head> as Pivot Number
                quickSortHead_AscendOrder(pary, 0, arysz-1, arysz);
            } else {
                // Pick <Tail> as Pivot Number
                quickSortTail_AscendOrder(pary, 0, arysz-1, arysz);
            }
        } else {
            if (  g_sortingFunctionVersion == 1 ) {
                // Pick <Head> as Pivot Number
                quickSortHead_DescendOrder(pary, 0, arysz-1, arysz);
            } else {
                // Pick <Tail> as Pivot Number
                quickSortTail_DescendOrder(pary, 0, arysz-1, arysz);
            }
        }
#endif


#ifdef INDEX_CHECK_SAFE
		auto bIsSorted = isArrayWellSorted( pary, arysz , g_isAscendOrder);
#else
		auto bIsSorted = g_isAscendOrder
                         ? isArray_AscendOrdered(pary, arysz) 
                         : isArray_DescendOrdered(pary, arysz);
#endif

        ++g_CaseCnt;
		if ( bIsSorted ) {
            ++g_CaseSuccCnt;

			cout << "=> Sort SUCC : contains " << arysz << " elements. " << endl;
		} else {
            ++g_CaseFailed;

			cout << "=> Sort Failed   :(   ==> contains " << arysz << " elements. " << endl;
			cout << "    " << "After Sort : " << endl;
			cout << setw(C_NEXT_LINE_INDENT_CNT) << setfill(' ') << ""
                 << "[ ";
            list<int> failedAry;
			for ( int i = 0; i < arysz; ++i ) {
				cout << pary[i];
                failedAry.push_back( pbackupAry[i] );

				if ( i < arysz-1 ) {
					cout << ", ";
				}
			}
			cout << " ]    " << endl;
            g_CaseFailedCollection.push_back(  failedAry );
		}

		delete [] pbackupAry;
		delete [] pary;
	}

	fobj.close();





    cout << "==================================================" << endl;
    if ( g_CaseFailed > 0 ) {
        cout << "\t\t[Error-Case-List] : " << endl;
        int idx = 0;

        for( const auto& failary : g_CaseFailedCollection ) {
            cout << "\t\t\t" << idx << ". [ ";
            for( const auto num : failary  ) { cout << num << " "; }
            cout << " ] " <<endl;
            ++idx;
        }
    }
    cout << endl << endl;

    cout << "==================================================" << endl;
    if ( G_checkArrayBoundFlag ) {
        cout << " Index Check : " << endl;
        if ( G_indexOutBoundCnt > 0 ) {
            cout << "\t[ERROR] : ";
        } else {
            cout << "\t";
        }
        cout << "G_indexOutBoundCnt = " << G_indexOutBoundCnt 
             << endl;
        cout << (G_quickSortRangeCheck  ? "" : "\n");
    } else {
        cout << "[INFO] : Index Check is [ OFF ] " << endl;
    }

    if ( G_quickSortRangeCheck ) {
        cout << "\t G_quickSortRangesOutOfBoundCnt = " << G_quickSortRangesOutOfBoundCnt
             << endl
             << endl;
    }
    cout << "Total " << g_CaseCnt << " case(s) : " << endl;
    cout << "\tSucc   : " << g_CaseSuccCnt << " / " << g_CaseCnt << endl;
    cout << "\tFailed : " << g_CaseFailed  << " / " << g_CaseCnt << endl;

    cout << "==================================================" << endl;
}


void printUsage()
{
	cout << "Usage : " << endl
		 << "==================================================" << endl
		 << " $ ./main [asc|desc]   [head|tail]  --checkIndex=[true|false]" << endl
		 << "==================================================" << endl
		 << endl;
}



int main(int argc, char* argv[], char* env[])
{
	if ( argc < 3 ) {
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
        printUsage();
        cout << "[ERROR] : Unknown Sort Version \"" << arg2 << "\"  , program exit. " << endl
             << endl
             << "   ByeBye !!! " << endl;
        return -1;
    } 

    if ( argc > 3 ) {
        string arg3(argv[3]);
        if ( arg3 == "--checkIndex=true" ) {
            G_checkArrayBoundFlag = true;
        } else if ( arg3 == "--checkIndex=false" ) {
            G_checkArrayBoundFlag = false;
        } else {
            printUsage();
            cout << "[ERROR] : Unknown argument \"" << arg3 << "\"  , program exit. " << endl
                 << endl
                 << "   ByeBye !!! " << endl;
            return -1;
        }
    }

#ifndef INDEX_CHECK_SAFE
    G_checkArrayBoundFlag = false;
#endif

    cout << "[INFO] Check index is out of range : " << (G_checkArrayBoundFlag ?  "[ON]" : "[OFF]") << endl;
    cout << endl;

	testSort(isAsc, useWhichVersion);

	return 0;
}



