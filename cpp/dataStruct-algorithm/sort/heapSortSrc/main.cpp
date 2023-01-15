#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;


int getElement1(int* ary, int arysz, int idx, int* pErrorCnt, int* isOK)
{
	// use   idx > 0   rather than       idx >= 0
	if ( idx > 0 && idx < arysz ) {
		if ( isOK != nullptr ) {
			*isOK = 1;
		}

		return ary[idx];
	} else {
		if ( pErrorCnt != nullptr ) {
			++(*pErrorCnt);
		}

		if ( isOK != nullptr ) {
			*isOK = 0;
		}
		cout << "idx is out of range -> idx = " << idx << "  range:[0," << arysz << ") " << endl;

		return 0;
	}
}



int getElement2(int* ary, int arysz, int idx, int* pErrorCnt, int* isOK)
{
	if ( idx >= 0 && idx < arysz ) {
		if ( isOK != nullptr ) {
			*isOK = 1;
		}

		return ary[idx];
	} else {
		if ( pErrorCnt != nullptr ) {
			++(*pErrorCnt);
		}

		if ( isOK != nullptr ) {
			*isOK = 0;
		}
		cout << "idx is out of range -> idx = " << idx << "  range:[0," << arysz << ") " << endl;

		return 0;
	}
}





void shift2(int* ary, int low, int high, int arysz, int* pErrorCnt)
{
    int bIsOK = 0;

	// int pIdx = low;
	int pIdx = low;
    // int leftIdx = pIdx * 2;
    int leftIdx = (pIdx * 2 + 1);


    // int target = ary[pIdx];
    int target = getElement2(ary, arysz, pIdx, pErrorCnt, &bIsOK );

    while ( leftIdx <= high )
    {
        if ( (leftIdx+1) <= high ) {
            // leftIdx = ( ary[leftIdx+1] > ary[leftIdx] ) ? (leftIdx+1) : leftIdx;
			leftIdx = ( getElement2(ary, arysz, (leftIdx+1) , pErrorCnt, &bIsOK ) > getElement2(ary, arysz, (leftIdx), pErrorCnt, &bIsOK ) )
					  ? (leftIdx+1) : leftIdx;
        }


		getElement2(ary, arysz, (leftIdx), pErrorCnt, &bIsOK );
        if ( ary[leftIdx]  > target ) {
            // ary[pIdx] = ary[leftIdx];
			getElement2(ary, arysz, (pIdx), pErrorCnt, &bIsOK );
			getElement2(ary, arysz, (leftIdx), pErrorCnt, &bIsOK );
			ary[pIdx] = ary[leftIdx];

            pIdx = leftIdx;
            leftIdx = (2 * pIdx + 1);
        } else {
            break;
        }

    }

	getElement2(ary, arysz, (pIdx), pErrorCnt, &bIsOK );
    ary[pIdx] = target;

}





/**************************************************
   
========== No Dummy Node : [0] ==========


             [0]
            /   \
           /     \
          /       \
        [1]       [2]
       /  \       /  \
     [3]  [4]   [5]  [6]



leftChildIdx  = (2 * parent) + 1;
rightChildIdx = (2 * parent) + 1 + 1 = (2 * parent) + 2;


========== No Dummy Node : [0] ==========

Array Range :              [0, n-1]

***************************************************/
void HeapSort2(int* ary, int n, int* errorCnt)
{
	// int errorCnt = 0;
	int bIsOK = 0;

    int i = 0;
    // for( i = n/2;     i>=1; --i ) {
       for( i = (n/2-1); i>=0; --i ) {
        // shift2(ary, i,  n,    (n+1), errorCnt);
           shift2(ary, i, (n-1),  (n),   errorCnt);
    }


    // for( i = n;     i>=2; --i ) {
       for( i = (n-1); i>=1; --i ) {
        // swap [1] <--> [n]
        {
			/*   swap [0] <--> [n-1]   */

            // int last  = ary[i];
			// int last = getElement1(ary, (n+1), i, errorCnt, &bIsOK );
			   int last = getElement2(ary, (n),   i, errorCnt, &bIsOK );

            // ary[i] = ary[1];
			// ary[i] = getElement1(ary, (n+1), 1, errorCnt, &bIsOK );
			   ary[i] = getElement2(ary, (n),   0, errorCnt, &bIsOK );

            // ary[1] = last;
               ary[0] = last;
        }


        // shift2(ary, 1,  i-1,   (n+1),  errorCnt);
           shift2(ary, 0, (i-1),   (n),   errorCnt);
    }



}







void shift1(int* ary, int low, int high, int arysz, int* pErrorCnt)
{
    int bIsOK = 0;

	int pIdx = low;
    int leftIdx = pIdx * 2;

    // int target = ary[pIdx];
    int target = getElement1(ary, arysz, pIdx, pErrorCnt, &bIsOK );

    while ( leftIdx <= high )
    {
        if ( (leftIdx+1) <= high ) {
            // leftIdx = ( ary[leftIdx+1] > ary[leftIdx] ) ? (leftIdx+1) : leftIdx;
			leftIdx = ( getElement1(ary, arysz, (leftIdx+1) , pErrorCnt, &bIsOK ) > getElement1(ary, arysz, (leftIdx), pErrorCnt, &bIsOK ) )
					  ? (leftIdx+1) : leftIdx;
        }


		getElement1(ary, arysz, (leftIdx), pErrorCnt, &bIsOK );
        if ( ary[leftIdx]  > target ) {
            // ary[pIdx] = ary[leftIdx];
			getElement1(ary, arysz, (pIdx), pErrorCnt, &bIsOK );
			getElement1(ary, arysz, (leftIdx), pErrorCnt, &bIsOK );
			ary[pIdx] = ary[leftIdx];

            pIdx = leftIdx;
            leftIdx = 2 * pIdx;
        } else {
            break;
        }

    }

	getElement1(ary, arysz, (pIdx), pErrorCnt, &bIsOK );
    ary[pIdx] = target;
}



/**************************************************


Typical original sample function   from   <<Algorithm Book>>


   
             Dummy-Node
Array Range :  [0]            [1, n]


***************************************************/
void HeapSort1(int* ary, int n, int* errorCnt)
{
	// int errorCnt = 0;
	int bIsOK = 0;

    int i = 0;
    for( i = n/2; i>=1; --i ) {
        shift1(ary, i, n, (n+1) , errorCnt);
    }


    for( i = n; i>=2; --i ) {
        // swap [1] <--> [n]
        {
            // int last  = ary[i];
			int last = getElement1(ary, (n+1), i, errorCnt, &bIsOK );

            // ary[i] = ary[1];
			ary[i] = getElement1(ary, (n+1), 1, errorCnt, &bIsOK );

            ary[1] = last;
        }


        shift1(ary,1, i-1, (n+1), errorCnt);
    }


}



void testLineByLine1( const string& filename)
{
    ifstream f( filename.c_str(), ios::in );
    if ( !f ) {
        cout << "[ERROR] : Can't Open File : " << filename << endl;
        return;
    }


    int* pAry = nullptr;
    int lineNo = 0;
    int sortCnt = 0;
    int unsortCnt = 0;
    while ( !f.eof() ) {
        string strLine;
        getline( f, strLine);
		if ( strLine.empty() ) {
			continue;
		}

        ++lineNo;

        if ( pAry != nullptr ) {
            delete [] pAry;
            pAry = nullptr;
        }

        int cnt = 0;
        vector<int> vAry;
        vAry.clear();

        string::size_type pos = string::npos;
        string::size_type beg = 0;
        while ( (pos = strLine.find(",", beg)) != string::npos ) {
            string strnum = strLine.substr(beg, (pos - beg) );
            try {
                int num = stoi( strnum );
                vAry.push_back( num );
                ++cnt;
            } catch (... ) {
                cout << "[ERROR] : can't convert \"" << strnum << "\" to int. " << endl;
            }

            beg = pos + 1;
        }


        string strnum = strLine.substr(beg);
        try {
            int num = stoi( strnum );
            vAry.push_back( num );
            ++cnt;
        } catch ( ... ) {
            cout << "[ERROR] : can't convert \"" << strnum << "\" to int. " << endl;
        }
        cout << "@"  << lineNo << ". parse "  << cnt << " numbers. " << endl;

        int idx1 = 0;
        pAry = new int[ cnt + 1 ];
        pAry[0] = -100;


        idx1 = 1;
        for ( auto e : vAry ) {
            pAry[idx1++] = e;
        }
        vAry.clear();

        
		int errorCnt = 0;
        HeapSort1( pAry, cnt, &errorCnt);

        if ( std::is_sorted( pAry+1, pAry + (cnt+1) ) ) {
            ++sortCnt;
            cout << "line : " << lineNo << " is sorted . "; 
			if ( errorCnt > 0 ) {
				cout << "errorCnt = " << errorCnt;
			}
			cout << endl;
        } else {
            ++unsortCnt;
            cout << "line : " << lineNo << " is unsorted . "; 
			if ( errorCnt > 0 ) {
				cout << "errorCnt = " << errorCnt;
			}
			cout << endl;
        }
    }


    if ( pAry != nullptr ) {
        delete [] pAry;
        pAry = nullptr;
    }
    f.close();


    cout << "lineNo = " << lineNo << endl;
    cout << "\t  sorted.count = " << sortCnt   << endl;
    cout << "\tunsorted.count = " << unsortCnt << endl;
}




void testLineByLine2( const string& filename)
{
    ifstream f( filename.c_str(), ios::in );
    if ( !f ) {
        cout << "[ERROR] : Can't Open File : " << filename << endl;
        return;
    }


    int* pAry = nullptr;
    int lineNo = 0;
    int sortCnt = 0;
    int unsortCnt = 0;
    while ( !f.eof() ) {
        string strLine;
        getline( f, strLine);
		if ( strLine.empty() ) {
			continue;
		}

        ++lineNo;

        if ( pAry != nullptr ) {
            delete [] pAry;
            pAry = nullptr;
        }

        int cnt = 0;
        vector<int> vAry;
        vAry.clear();

        string::size_type pos = string::npos;
        string::size_type beg = 0;
        while ( (pos = strLine.find(",", beg)) != string::npos ) {
            string strnum = strLine.substr(beg, (pos - beg) );
            try {
                int num = stoi( strnum );
                vAry.push_back( num );
                ++cnt;
            } catch (... ) {
                cout << "[ERROR] : can't convert \"" << strnum << "\" to int. " << endl;
            }

            beg = pos + 1;
        }


        string strnum = strLine.substr(beg);
        try {
            int num = stoi( strnum );
            vAry.push_back( num );
            ++cnt;
        } catch ( ... ) {
            cout << "[ERROR] : can't convert \"" << strnum << "\" to int. " << endl;
        }
        cout << "@"  << lineNo << ". parse "  << cnt << " numbers. " << endl;

        int idx1 = 0;
        pAry = new int[ cnt ];

        idx1 = 0;
        for ( auto e : vAry ) {
            pAry[idx1++] = e;
        }
        vAry.clear();

        
		int errorCnt = 0;
        HeapSort2( pAry, cnt, &errorCnt);
        if ( std::is_sorted( pAry,   pAry + (cnt) ) ) {
            ++sortCnt;
            cout << "line : " << lineNo << " is sorted . "; 
			if ( errorCnt > 0 ) {
				cout << "errorCnt = " << errorCnt;
			}
			cout << endl;
        } else {
            ++unsortCnt;
            cout << "line : " << lineNo << " is unsorted . "; 
			if ( errorCnt > 0 ) {
				cout << "errorCnt = " << errorCnt;
			}
			cout << endl;
        }
    }


    if ( pAry != nullptr ) {
        delete [] pAry;
        pAry = nullptr;
    }
    f.close();


    cout << "lineNo = " << lineNo << endl;
    cout << "\t  sorted.count = " << sortCnt   << endl;
    cout << "\tunsorted.count = " << unsortCnt << endl;
}






int main ( int argc, char *argv[], char *env[])
{
    if ( argc >= 2 ) {
		cout << "Use Method - 2" << endl;
        // testLineByLine1( string(argv[1]) );
        testLineByLine2( string(argv[1]) );
    }

	return 0;
}
