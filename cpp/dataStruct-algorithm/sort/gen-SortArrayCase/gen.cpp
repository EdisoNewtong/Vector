#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cstdint>
#include <set>
#include <sstream>
using namespace std;

const string C_prefix("--bin=");
static string g_FileName = ("");

union EndianCheck {
    short shortNum;
    char  buf[2];
};

struct SlotState {
	int currentValue;
};

struct CmpResult
{
    int* pAry;
    vector<int> cmpList;
};

struct CmpUtil
{
	bool operator() (const CmpResult& aryA, const CmpResult& aryB) const
	{
		bool prtLog = false;
		if ( aryA.cmpList.size() != aryB.cmpList.size() ) {
			if ( prtLog ) { cout << "===== Size() not Equal =====" << endl; }
			return false;
		}

		if ( prtLog ) {
			cout << "cmpA = [ ";
			for( auto e : aryA.cmpList ) {
				cout << e << " ";
			}
			cout << "]  ";

			cout << "cmpB = [ ";
			for( auto e : aryB.cmpList ) {
				cout << e << " ";
			}
			cout << "] " << endl;
		}


		int isAllSame = 0;
		for( int i = 0; i < static_cast<int>( aryA.cmpList.size() ); ++i ) {
			if ( aryA.cmpList[i] != aryB.cmpList[i] ) {
				isAllSame = (aryA.cmpList[i] < aryB.cmpList[i] ? 1 : -1);
				break;
			}
		}

        // e.g.
        // 1. [ -1, -1, -1 ]
        //    (  >,  >, > )
        // 2. [ -1, -1, 0  ]
        //    (  >,  >, == )
        // 3. [ -1, -1, 1  ]
        //    (  >,  >, <  )

		bool retFlag = (isAllSame == 1);
		return retFlag;
	}
};

static int                     g_iLittleEndianFlag = 0;
static set<CmpResult, CmpUtil> g_aryCmpPossibilities;
static stringstream            g_unformattedBuf;


typedef int (*opFunc)(int* pAry,int arySz);

unsigned long long generateAllPermutation(int arySz, bool needPrintGeneratedAry, opFunc func)
{
    uint32_t differentCnt = 0;
	unsigned long long possibilityCnt = 0ull;
    unsigned char nArySz = static_cast<unsigned char>( arySz );
    // 0 :    Big-Endian |    1 : Little-Endian
    if ( g_iLittleEndianFlag ) {
        nArySz |= 0x80U; // g_iLittleEndianFlag != 0, LeaderBit set 1
    } else {
        nArySz &= 0x7FU; // g_iLittleEndianFlag == 0, LeaderBit set 0
    }
    g_unformattedBuf.write( reinterpret_cast<const char*>(&nArySz),       sizeof(nArySz) );
    g_unformattedBuf.write( reinterpret_cast<const char*>(&differentCnt), sizeof(differentCnt) );    // differentCnt is a placehold int with 4 bytes

	if ( arySz <= 1 ) {
		// cout << "[WARNING] Array.Size == 1 : return" << endl;
        possibilityCnt = 1ull;
        if ( func != nullptr ) {
            int ary[1] = { 1 };
            func(ary,arySz);
        }
		return possibilityCnt;
	}

    //
    // arySz >=2
    //
	int* pAry            = new int[arySz];
	// int* pBackupAry      = new int[arySz];
	SlotState* pStateAry = new SlotState[arySz];
	for( int i = 0; i < arySz; ++i ) {
		pStateAry[i].currentValue = 1; // start with 1, loop in the range [1, arySz]
	}

	bool isAllEnumurated = false;
	do {
		++possibilityCnt;

        // Print and    *** Set *** stateAry   Part 
		if ( needPrintGeneratedAry ) { cout << setw(arySz+1) << possibilityCnt << ". [ "; }
		for( int i = 0; i < arySz; ++i ) {
			pAry[i] = pStateAry[i].currentValue;
			// pBackupAry[i] = pAry[i];
			if ( needPrintGeneratedAry ) { cout << pAry[i] << ((i < arySz-1) ? ", " : " "); }
		}
		if ( needPrintGeneratedAry ) { cout << " ] "; }



		if ( func != nullptr ) {
			func(pAry, arySz);
		}
        if ( needPrintGeneratedAry ) { cout << endl; }




		if ( (pStateAry[arySz-1].currentValue + 1 ) <= arySz ) {
			++(pStateAry[arySz-1].currentValue);
			isAllEnumurated = false;
		} else {
			int rIdx = 0;
			bool foundPreviousIterator = false;
			for( rIdx = arySz-2; rIdx >=0; --rIdx ) {
				if ( (pStateAry[rIdx].currentValue + 1) <= arySz ) {
					foundPreviousIterator = true;
					break;
				}
			}

			if ( foundPreviousIterator ) {
				++(pStateAry[rIdx].currentValue);
				// reset other slots after rIdx to init
				for( int i = rIdx+1; i < arySz; ++i ) {
					pStateAry[i].currentValue = 1;
				}

				bool needPrintLog = false;
				if ( rIdx == 0 && needPrintLog ) {
					cout << "\tNew turn come : " << pStateAry[rIdx].currentValue << endl;
				}
			}

			isAllEnumurated = !foundPreviousIterator;
		}
	} while ( !isAllEnumurated );



	delete [] pAry;        pAry = nullptr;
	// delete [] pBackupAry;  pBackupAry = nullptr;
	delete [] pStateAry;   pStateAry = nullptr;
	return possibilityCnt;
}

int collectionAry(int* pAry,int arySz)
{
    CmpResult ret;
    ret.pAry = pAry;
    ret.cmpList.clear();

    for( int i = 0; i < (arySz-1); ++i ) {
        for( int j = (i+1); j < (arySz); ++j ) {
            int cmpRes = 0;
            if ( pAry[i] < pAry[j] ) {
                cmpRes = 1;
            } else if ( pAry[i] == pAry[j] ) {
                cmpRes = 0;
            } else {
                cmpRes = -1;
            }

            ret.cmpList.push_back( cmpRes );
        }
    }

    int istret = 0;
    auto pr = g_aryCmpPossibilities.insert( ret );
    if ( pr.second ) {
        // Insert one Successful
        unsigned char bytes2Merge = 0u;
        for( int i = 0; i < arySz; ++i ) {
            unsigned char ucNum = static_cast<unsigned char>( pAry[i] ) ;
            // Merge 2 number into a byte  , every 4 bits to present 1 number
            //--------------------------------------------------------------------
            // Leader 4bits | Tail 4bits
            // 1st number   | 2nd number
            //       xxxx   |       xxxx
            //--------------------------------------------------------------------
            if ( (i % 2) == 0 ) {
                // The 1st number
                bytes2Merge |= (ucNum << 4u);
                bytes2Merge &= 0xF0;
                // if index is the last one and the even order , write the last bytes
                if ( i == (arySz-1) ) {
                    g_unformattedBuf.write(reinterpret_cast<const char*>(&bytes2Merge),  sizeof(bytes2Merge) );
                }
            } else {
                // The 2nd number
                bytes2Merge |= ucNum;
                g_unformattedBuf.write(reinterpret_cast<const char*>(&bytes2Merge),  sizeof(bytes2Merge) );
                bytes2Merge = 0u;
            }
        }
        istret = 1;
    }

    return istret;
}

void printUsage()
{
    cout << "--------------------------------------------------" << endl;
    cout << "Usage : " << endl;
    cout << "      $ gen <N> [--bin=<filename>]" << endl;
    cout << "--------------------------------------------------" << endl;
    cout << endl;
}

/****************************************************************************************************

Usage :
$ gen <N>   [--bin=<filename>]

*****************************************************************************************************/
int main(int argc, char* argv[])
{
    EndianCheck ec; ec.shortNum = 0xABCD;
    // 0 :    Big-Endian |    1 : Little-Endian
    g_iLittleEndianFlag = (ec.buf[0] == 0xAB   &&   ec.buf[1] == 0xCD) ? 0 : 1;
    cout << "\t" << (g_iLittleEndianFlag ? " Little-Endian" : " Big-Endian") << endl;
    g_aryCmpPossibilities.clear();
    g_unformattedBuf.clear();

    if ( argc < 2 ) {
        cout << "[ERROR] missing argument , a positive array size must be specified. " << endl;
        printUsage();
        return -1;
    }

    bool convertSuccess = false;
    int nArraySize = 0;
    try {
        nArraySize = atoi( argv[1] );
        convertSuccess = true;
    } catch ( ... ) {
        cout << "[ERROR] Can't convert " << argv[1] << " to a number." << endl;
    }

    if ( !convertSuccess ) {
        return -1;
    }

    // convertSuccess == true
    if ( nArraySize < 1 ) {
        cout << "[ERROR] The given array's size must >=1  "  << endl;
        return -1;
    }

    string arg4fileName;
    if ( argc > 2 ) {
        arg4fileName = argv[2];
        if ( arg4fileName.find(C_prefix) != 0 ||  arg4fileName.size() <= C_prefix.size() ) {
            cout << "[ERROR] the 2nd argument must be start with \"--bin=\" with a none empty filename. "  << endl;
            printUsage();
            return -1;
        } else {
            // arg4fileName = arg4fileName.substr( C_prefix.size() );
            g_FileName = arg4fileName.substr( C_prefix.size() );
        }
    }


    generateAllPermutation(nArraySize,  true,  collectionAry);
    uint32_t finalSz = static_cast<uint32_t>( g_aryCmpPossibilities.size() );
    cout << nArraySize << " element(s) inside an array  can generate  " << finalSz << " different  comparsion  array(s). " << endl;
    cerr << nArraySize << " element(s) inside an array  can generate  " << finalSz << " different  comparsion  array(s). " << endl;

    if ( !g_FileName.empty() ) {
        ofstream outputfile(g_FileName, ios::out | ios::trunc | ios::binary);
        if ( !outputfile ) {
            cout << "[ERROR] Can't open binary file " << g_FileName << " to write. " << endl;
            return -1;
        }

        g_unformattedBuf.seekp(1, ios::beg);
        g_unformattedBuf.write( reinterpret_cast<const char*>(&finalSz), sizeof(finalSz) );
        g_unformattedBuf.seekp(ios::end);

        outputfile << g_unformattedBuf.str();
        outputfile.flush();
        outputfile.close();
        cout << "Wrote Binary file " << g_FileName << "  [Done] " << endl;
    }



    return 0;
}

