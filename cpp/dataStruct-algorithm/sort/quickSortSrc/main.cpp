#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <fstream>



#include "sortUtil1.h"
#include "sortUtil2.h"
#include "sortUtil3.h"
#include "sortUtilRandom.h"
#include "common.h"


using namespace std;

///////////////////////////////////////////////////////////////////////////
//
// Global debug flag varible
//
///////////////////////////////////////////////////////////////////////////


static const bool G_NeedBreakIfError = true;


namespace 
{
    static unsigned long long g_CaseCnt     = 0ULL;
    static unsigned long long g_CaseSuccCnt = 0ULL;
    static unsigned long long g_CaseFailed  = 0ULL;
    static vector< vector<int> >  g_CaseFailedCollection;

}


int getDecimalNumberBits(int n)
{
    int nBits = 0;
    do {
        n /= 10;
        ++nBits;
    } while( n != 0 );
    return nBits;
}



void testSort(bool isAscOrder, int headOrTailOrRandom, int version, bool bNeedIndexCheck)
{

	cout << "==================================================" << endl;
	if ( isAscOrder ) {
		cout << "Sorting by the <Ascending>  Order " << endl;
	} else {
		cout << "Sorting by the <Descending> Order " << endl;
	}

	if ( headOrTailOrRandom == 1 ) {
		cout << "Use Pick <Head> Number <== as the povit Number " << endl;
	} else if ( headOrTailOrRandom == 2 ) {
		cout << "Use Pick <Tail> Number <== as the povit Number " << endl;
	} else if ( headOrTailOrRandom == 3 ) {
		cout << "Use Pick <RandomIndex> (Neither Head nor Tail) Number as the povit Number " << endl;
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

		vector<int> lary;
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


		if ( isAscOrder ) {
			//
			// Ascending Order
			//
			if ( headOrTailOrRandom == 1 ) {
				// head version
				if ( version == 1 ) {
					bNeedIndexCheck 
						? v1::quickSortHead_AscendOrder_Safe(pary,0, arysz-1, arysz)  
						: v1::quickSortHead_AscendOrder(pary,0, arysz-1, arysz);
				} else if ( version == 2 ) {
					bNeedIndexCheck 
						? v2::quickSortHead_AscendOrder_Safe(pary,0, arysz-1, arysz)  
						: v2::quickSortHead_AscendOrder(pary,0, arysz-1, arysz);
				} else if ( version == 3 ) {
					bNeedIndexCheck 
						? v3::quickSortHead_AscendOrder_Safe(pary,0, arysz-1, arysz)  
						: v3::quickSortHead_AscendOrder(pary,0, arysz-1, arysz);
				}
			} else if ( headOrTailOrRandom == 2 ) {
				// tail version
				if ( version == 1 ) {
					bNeedIndexCheck 
						? v1::quickSortTail_AscendOrder_Safe(pary,0, arysz-1, arysz)  
						: v1::quickSortTail_AscendOrder(pary,0, arysz-1, arysz);
				} else if ( version == 2 ) {
					bNeedIndexCheck 
						? v2::quickSortTail_AscendOrder_Safe(pary,0, arysz-1, arysz)  
						: v2::quickSortTail_AscendOrder(pary,0, arysz-1, arysz);
				} else if ( version == 3 ) {
					bNeedIndexCheck 
						? v3::quickSortTail_AscendOrder_Safe(pary,0, arysz-1, arysz)  
						: v3::quickSortTail_AscendOrder(pary,0, arysz-1, arysz);
				}
			} else {
				//  random == 3
				bNeedIndexCheck 
					? vRandom::quickSortRandom_AscendOrder_Safe(pary,0 , arysz-1, arysz)
					: vRandom::quickSortRandom_AscendOrder(pary,0 , arysz-1, arysz);
			}
		} else {
			//
			// Descending Order
			//
			if ( headOrTailOrRandom == 1 ) {
				// head version
				if ( version == 1 ) {
					bNeedIndexCheck 
						? v1::quickSortHead_DescendOrder_Safe(pary,0, arysz-1, arysz)  
						: v1::quickSortHead_DescendOrder(pary,0, arysz-1, arysz);
				} else if ( version == 2 ) {
					bNeedIndexCheck 
						? v2::quickSortHead_DescendOrder_Safe(pary,0, arysz-1, arysz)  
						: v2::quickSortHead_DescendOrder(pary,0, arysz-1, arysz);
				} else if ( version == 3 ) {
					bNeedIndexCheck 
						? v3::quickSortHead_DescendOrder_Safe(pary,0, arysz-1, arysz)  
						: v3::quickSortHead_DescendOrder(pary,0, arysz-1, arysz);
				}
			} else if ( headOrTailOrRandom == 2 ) {
				// tail version
				if ( version == 1 ) {
					bNeedIndexCheck 
						? v1::quickSortTail_DescendOrder_Safe(pary,0, arysz-1, arysz)  
						: v1::quickSortTail_DescendOrder(pary,0, arysz-1, arysz);
				} else if ( version == 2 ) {
					bNeedIndexCheck 
						? v2::quickSortTail_DescendOrder_Safe(pary,0, arysz-1, arysz)  
						: v2::quickSortTail_DescendOrder(pary,0, arysz-1, arysz);
				} else if ( version == 3 ) {
					bNeedIndexCheck 
						? v3::quickSortTail_DescendOrder_Safe(pary,0, arysz-1, arysz)  
						: v3::quickSortTail_DescendOrder(pary,0, arysz-1, arysz);
				}
			} else {
				//  random == 3
				bNeedIndexCheck 
					? vRandom::quickSortRandom_DescendOrder_Safe(pary,0 , arysz-1, arysz)
					: vRandom::quickSortRandom_DescendOrder(pary,0 , arysz-1, arysz);
			}


		}






        



		auto bIsSorted = isAscOrder
                         ? isArray_AscendOrdered(pary, arysz) 
                         : isArray_DescendOrdered(pary, arysz);

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
            vector<int> failedAry;
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

        if ( G_NeedBreakIfError && !bIsSorted ) {
            break;
        }
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
    if ( bNeedIndexCheck  ) {
        cout << "[INFO] : Index Check is [ YES ] " << endl;
    } else {
        cout << "[INFO] : Index Check is [ NO ] " << endl;
    }

    cout << "Total " << g_CaseCnt << " case(s) : " << endl;
    cout << "\tSucc   : " << g_CaseSuccCnt << " / " << g_CaseCnt << endl;
    cout << "\tFailed : " << g_CaseFailed  << " / " << g_CaseCnt << endl;

    cout << "==================================================" << endl;

}



void testSortWithNumber(bool isAscOrder, int headOrTailOrRandom, int version, bool bNeedIndexCheck, int maxNumber)
{
    int nBits = getDecimalNumberBits(maxNumber);
    int arysz = nBits;
    int* pAry = new int[nBits];
    int* pBackupAry = new int[nBits];

    int base = 0;
    for( int num = 1; num <= maxNumber; ++num )
    {
        base = 1;
        for( int nthBit = nBits-1; nthBit >= 0; --nthBit )
        {
            int bitNumber = (num / base) % 10;
            pAry[nthBit] = pBackupAry[nthBit] = bitNumber;
            base *= 10;
        }


		if ( isAscOrder ) {
			//
			// Ascending Order
			//
			if ( headOrTailOrRandom == 1 ) {
				// head version
				if ( version == 1 ) {
					bNeedIndexCheck 
						? v1::quickSortHead_AscendOrder_Safe(pAry ,0, arysz-1, arysz)  
						: v1::quickSortHead_AscendOrder(pAry ,0, arysz-1, arysz);
				} else if ( version == 2 ) {
					bNeedIndexCheck 
						? v2::quickSortHead_AscendOrder_Safe(pAry ,0, arysz-1, arysz)  
						: v2::quickSortHead_AscendOrder(pAry ,0, arysz-1, arysz);
				} else if ( version == 3 ) {
					bNeedIndexCheck 
						? v3::quickSortHead_AscendOrder_Safe(pAry ,0, arysz-1, arysz)  
						: v3::quickSortHead_AscendOrder(pAry ,0, arysz-1, arysz);
				}
			} else if ( headOrTailOrRandom == 2 ) {
				// tail version
				if ( version == 1 ) {
					bNeedIndexCheck 
						? v1::quickSortTail_AscendOrder_Safe(pAry ,0, arysz-1, arysz)  
						: v1::quickSortTail_AscendOrder(pAry ,0, arysz-1, arysz);
				} else if ( version == 2 ) {
					bNeedIndexCheck 
						? v2::quickSortTail_AscendOrder_Safe(pAry ,0, arysz-1, arysz)  
						: v2::quickSortTail_AscendOrder(pAry ,0, arysz-1, arysz);
				} else if ( version == 3 ) {
					bNeedIndexCheck 
						? v3::quickSortTail_AscendOrder_Safe(pAry ,0, arysz-1, arysz)  
						: v3::quickSortTail_AscendOrder(pAry ,0, arysz-1, arysz);
				}
			} else {
				//  random == 3
				bNeedIndexCheck 
					? vRandom::quickSortRandom_AscendOrder_Safe(pAry ,0 , arysz-1, arysz)
					: vRandom::quickSortRandom_AscendOrder(pAry ,0 , arysz-1, arysz);
			}
		} else {
			//
			// Descending Order
			//
			if ( headOrTailOrRandom == 1 ) {
				// head version
				if ( version == 1 ) {
					bNeedIndexCheck 
						? v1::quickSortHead_DescendOrder_Safe(pAry ,0, arysz-1, arysz)  
						: v1::quickSortHead_DescendOrder(pAry ,0, arysz-1, arysz);
				} else if ( version == 2 ) {
					bNeedIndexCheck 
						? v2::quickSortHead_DescendOrder_Safe(pAry ,0, arysz-1, arysz)  
						: v2::quickSortHead_DescendOrder(pAry ,0, arysz-1, arysz);
				} else if ( version == 3 ) {
					bNeedIndexCheck 
						? v3::quickSortHead_DescendOrder_Safe(pAry ,0, arysz-1, arysz)  
						: v3::quickSortHead_DescendOrder(pAry ,0, arysz-1, arysz);
				}
			} else if ( headOrTailOrRandom == 2 ) {
				// tail version
				if ( version == 1 ) {
					bNeedIndexCheck 
						? v1::quickSortTail_DescendOrder_Safe(pAry ,0, arysz-1, arysz)  
						: v1::quickSortTail_DescendOrder(pAry ,0, arysz-1, arysz);
				} else if ( version == 2 ) {
					bNeedIndexCheck 
						? v2::quickSortTail_DescendOrder_Safe(pAry ,0, arysz-1, arysz)  
						: v2::quickSortTail_DescendOrder(pAry ,0, arysz-1, arysz);
				} else if ( version == 3 ) {
					bNeedIndexCheck 
						? v3::quickSortTail_DescendOrder_Safe(pAry ,0, arysz-1, arysz)  
						: v3::quickSortTail_DescendOrder(pAry ,0, arysz-1, arysz);
				}
			} else {
				//  random == 3
				bNeedIndexCheck 
					? vRandom::quickSortRandom_DescendOrder_Safe(pAry ,0 , arysz-1, arysz)
					: vRandom::quickSortRandom_DescendOrder(pAry ,0 , arysz-1, arysz);
			}


		}


		auto bIsSorted = isAscOrder
                         ? isArray_AscendOrdered(pAry , arysz )
                         : isArray_DescendOrdered(pAry , arysz);


        ++g_CaseCnt;
		if ( bIsSorted ) {
            ++g_CaseSuccCnt;

			cout << "=> Sort SUCC : contains " << arysz << " elements. num = " << num <<  ", Ary Original [ ";
			for( int idx = 0; idx < arysz; ++idx ) {
				cout << pBackupAry[idx] << (idx!=arysz-1 ? ", " : " ");
			}
			cout << " ] ";
			cout << " => [ ";
			for( int idx = 0; idx < arysz; ++idx ) {
				cout << pAry[idx] << (idx!=arysz-1 ? ", " : " ");
			}
			cout << " ] " << endl;
		} else {
            ++g_CaseFailed;

			cout << "=> Sort Failed   :(   ==> contains " << arysz << " elements. num = " << num <<  "Ary Original [";
			for( int idx = 0; idx < arysz; ++idx ) {
				cout << pBackupAry[idx] << (idx!=arysz-1 ? ", " : " ");
			}
			cout << " ] ";
			cout << "    " << "After Sort : " << endl
				 << " [ ";
            vector<int> failedAry;
			for ( int i = 0; i < arysz; ++i ) {
				cout << pAry[i];

                // pAry[nthBit] = pBackupAry[nthBit] = bitNumber;
                failedAry.push_back( pBackupAry[i] );

				if ( i < arysz-1 ) {
					cout << ", ";
				}
			}
			cout << " ]    " << endl;
            g_CaseFailedCollection.push_back(  failedAry );
		}

        if ( G_NeedBreakIfError && !bIsSorted ) {
            break;
        }
    }

	delete [] pAry;
	delete [] pBackupAry;


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
    if ( bNeedIndexCheck  ) {
        cout << "[INFO] : Index Check is [ YES ] " << endl;
    } else {
        cout << "[INFO] : Index Check is [ NO ] " << endl;
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
		 // total 6 arguments  ( at least 4 elements )
		 //       0         1             2               3            4
		 << " $ ./main  asc|desc   head|tail|random  --ver=1  [--checkIndex=true|false]  [--maxNumber=intValue]" << endl
		 << "==================================================" << endl
		 << endl;
}



int main(int argc, char* argv[], char* env[])
{
	if ( argc < 4 ) {
		cout << "[ERROR] : Missing args!!!" << endl;
		printUsage();
		return -1;
	}

	// "asc"  : ascending  order  => true   
	// "desc" : descending order  => false
	auto isAsc = (string(argv[1]) == "asc");

	// "head"  : head version
	// "tail"  : tail version
	// "random"  : tail version
	int useWhichVersion = 0;
	string arg2(argv[2]);
	if ( arg2 == "head" ) {
		useWhichVersion = 1;
	} else if ( arg2 == "tail" ) {
		useWhichVersion = 2;
	} else if ( arg2 == "random" ) {
		useWhichVersion = 3;
    } else {
        printUsage();
        cout << "[ERROR] : Unknown Sort Version \"" << arg2 << "\"  , program exit. " << endl
             << endl
             << "   ByeBye !!! " << endl;
        return -1;
    }

	int mVersion = 0;
	string arg3(argv[3]);
	if ( arg3 == "--ver=1" ) {
		mVersion = 1;
	} else if ( arg3 == "--ver=2" ) {
		mVersion = 2;
	} else if ( arg3 == "--ver=3" ) {
		mVersion = 3;
	} else {
		printUsage();
		cout << "[ERROR] : Unknown argument \"" << arg3 << "\"  , program exit. " << endl
			 << endl
			 << "   ByeBye !!! " << endl;
		return -1;
	}


	bool bCheckIndex = false;
	if ( argc >= 5 ) {
		string arg4(argv[4]);
		if ( arg4 == "--checkIndex=true" ) {
			bCheckIndex = true;
		} else if ( arg4 == "--checkIndex=false" ) {
			bCheckIndex = false;
		} else {
			printUsage();
			cout << "[ERROR] : Unknown argument \"" << arg4 << "\"  , program exit. " << endl
				 << endl
				 << "   ByeBye !!! " << endl;
			return -1;
		}
	}




	try {
        int limitNumber = 1;
        if ( argc >= 6 ) {
            const string prefix("--maxNumber=");
            string arg5( argv[5] );
            auto strPos = arg5.find(prefix);
            if ( strPos != 0 ) {
                printUsage();
                cout << "[ERROR] : Unknown argument \"" << arg5 << "\"  , program exit. " << endl
                     << endl
                     << "   ByeBye !!! " << endl;
                return -1;
            } 

            string strTailNumber =  arg5.substr( prefix.size() );
            limitNumber = stoi(strTailNumber);
        }

        cout << "[INFO] Check index is out of range : " << (bCheckIndex ?  "[YES]" : "[NO]") << endl;
        cout << endl;


        if ( argc < 6 ) {
		    testSort(isAsc, useWhichVersion, mVersion, bCheckIndex);
        } else {
            testSortWithNumber(isAsc, useWhichVersion, mVersion, bCheckIndex, limitNumber);
        }
	} catch ( const myIdxOutOfRangeException& e ) {
        cerr << "[ERROR] : " << e.what() << endl;
	} catch ( const std::exception& e ) {
        cerr << "[ERROR] : " << e.what() << endl;
    }

	return 0;
}