#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cstdint>
#include <set>
#include <sstream>
#include <unordered_set>
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
// every set's max size is  500,0000

// set<unsigned long long>* g_aryCmpPossibilities1;
// set<unsigned long long>* g_aryCmpPossibilities2;
// set<unsigned long long>* g_aryCmpPossibilities3;
// set<unsigned long long>* g_aryCmpPossibilities4;
// set<unsigned long long>* g_aryCmpPossibilities5;
// set<unsigned long long>* g_aryCmpPossibilities6;

unordered_set<unsigned long long>* g_aryCmpPossibilities1;
unordered_set<unsigned long long>* g_aryCmpPossibilities2;
unordered_set<unsigned long long>* g_aryCmpPossibilities3;
unordered_set<unsigned long long>* g_aryCmpPossibilities4;
unordered_set<unsigned long long>* g_aryCmpPossibilities5;
unordered_set<unsigned long long>* g_aryCmpPossibilities6;

static int g_CurrentWhichSet = 1;
// static const size_t g_MaxSetSize = 740000;
static const size_t g_MaxHashTableSize = 680000;

static const size_t g_MaxWriteBufSz = 1024*1024/2;  // 1 MB
stringstream*       g_unformattedBuf = nullptr;

static ofstream* g_pOutputFile = nullptr;





typedef int (*opFunc)(int* pAry,int arySz);

unsigned long long generateAllPermutation(int arySz, bool needPrintGeneratedAry, opFunc func)
{
	unsigned long long possibilityCnt = 0ull;

    unsigned long long differentCnt = 0; // The total different permuation count of the array group
    unsigned char nArySz = static_cast<unsigned char>( arySz );
    // 0 :    Big-Endian |    1 : Little-Endian
    if ( g_iLittleEndianFlag ) {
        nArySz |= 0x80U; // g_iLittleEndianFlag != 0, Set Leader as 1
    } else {
        nArySz &= 0x7FU; // g_iLittleEndianFlag == 0, Set Leader as 0
    }
    g_unformattedBuf->write( reinterpret_cast<const char*>(&nArySz),       sizeof(nArySz) );
    g_unformattedBuf->write( reinterpret_cast<const char*>(&differentCnt), sizeof(differentCnt) );    // differentCnt is a placehold int with 8 bytes
    if ( g_pOutputFile != nullptr ) {
        (*g_pOutputFile) <<  g_unformattedBuf->str();
        g_pOutputFile->flush();
    }
    g_unformattedBuf->str(""); // clear buffer after file header has been written

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

    bool extraFlag = false;
	bool isAllEnumurated = false;
	do {
		++possibilityCnt;

        // Print and    *** Set *** stateAry   Part 
		if ( extraFlag && needPrintGeneratedAry  ) { cout << setw(arySz+1) << possibilityCnt << ". [ "; }
		for( int i = 0; i < arySz; ++i ) {
			pAry[i] = pStateAry[i].currentValue;
			// pBackupAry[i] = pAry[i];
			if ( extraFlag && needPrintGeneratedAry  ) { cout << pAry[i] << ((i < arySz-1) ? ", " : " "); }
		}
		if ( extraFlag && needPrintGeneratedAry  ) { cout << " ] "; }

		if ( func != nullptr ) { func(pAry, arySz); }

        if ( extraFlag && needPrintGeneratedAry  ) { cout << endl;  extraFlag = false; }


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

				// bool needPrintLog = false;
				if ( arySz>=7 && rIdx <= 3  ) {
                    extraFlag = true;
                    // if ( needPrintLog ) {
					//     cout << "\tNew turn come : " << pStateAry[rIdx].currentValue << endl;
                    // }
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
    /*
    CmpResult ret;
    ret.pAry = pAry;
    ret.cmpList.clear();
    */

    unsigned long long ret = 0ull;

    for( int i = 0; i < (arySz-1); ++i ) {
        for( int j = (i+1); j < (arySz); ++j ) {
            unsigned long long cmpRes = 0;
            if ( pAry[i] < pAry[j] ) {
                cmpRes = 2;   // 0011
            } else if ( pAry[i] == pAry[j] ) {
                cmpRes = 1;   // 0001
            } else {
                cmpRes = 0;   // 0000
            }

            ret |= cmpRes;

            if ( i < (arySz-2) ) {
                ret <<= 2;
            }

            // ret.cmpList.push_back( cmpRes );
        }

        /*
        if ( i < (arySz-2) ) {
            ret <<= 2;
        }
        */
    }

    // int istret = 0;

    bool bHasNewInserted = false;
    // g_CurrentWhichSet
    if( g_CurrentWhichSet == 1 ) {
        if ( g_aryCmpPossibilities1->find( ret )  == g_aryCmpPossibilities1->end() ) {
            g_aryCmpPossibilities1->insert( ret );
            bHasNewInserted = true;
            if ( g_aryCmpPossibilities1->size() == g_MaxHashTableSize ) {
                cout << "g_CurrentWhichSet : 1 -> 2" << endl;
                g_CurrentWhichSet = 2;
            }
        }
    } else if ( g_CurrentWhichSet == 2 ) {
        if (       g_aryCmpPossibilities1->find( ret ) == g_aryCmpPossibilities1->end() 
              &&   g_aryCmpPossibilities2->find( ret ) == g_aryCmpPossibilities2->end() ) 
        {
            g_aryCmpPossibilities2->insert( ret );
            bHasNewInserted = true;
            if ( g_aryCmpPossibilities2->size() == g_MaxHashTableSize ) {
                cout << "g_CurrentWhichSet : 2 -> 3" << endl;
                g_CurrentWhichSet = 3;
            }
        }


    } else if ( g_CurrentWhichSet == 3 ) {
        if (       g_aryCmpPossibilities1->find( ret ) == g_aryCmpPossibilities1->end()  
              &&   g_aryCmpPossibilities2->find( ret ) == g_aryCmpPossibilities2->end() 
              &&   g_aryCmpPossibilities3->find( ret ) == g_aryCmpPossibilities3->end() )
        {
            g_aryCmpPossibilities3->insert( ret );
            bHasNewInserted = true;
            if ( g_aryCmpPossibilities3->size() == g_MaxHashTableSize ) {
                cout << "g_CurrentWhichSet : 3 -> 4" << endl;
                g_CurrentWhichSet = 4;
            }
        }

    } else if ( g_CurrentWhichSet == 4 ) {
        if (       g_aryCmpPossibilities1->find( ret ) == g_aryCmpPossibilities1->end()  
              &&   g_aryCmpPossibilities2->find( ret ) == g_aryCmpPossibilities2->end() 
              &&   g_aryCmpPossibilities3->find( ret ) == g_aryCmpPossibilities3->end() 
              &&   g_aryCmpPossibilities4->find( ret ) == g_aryCmpPossibilities4->end() )
        {
            g_aryCmpPossibilities4->insert( ret );
            bHasNewInserted = true;
            if ( g_aryCmpPossibilities4->size() == g_MaxHashTableSize ) {
                cout << "g_CurrentWhichSet : 4 -> 5" << endl;
                g_CurrentWhichSet = 5;
            }
        }
    } else if ( g_CurrentWhichSet == 5 ) {
        if (       g_aryCmpPossibilities1->find( ret ) == g_aryCmpPossibilities1->end()  
              &&   g_aryCmpPossibilities2->find( ret ) == g_aryCmpPossibilities2->end() 
              &&   g_aryCmpPossibilities3->find( ret ) == g_aryCmpPossibilities3->end() 
              &&   g_aryCmpPossibilities4->find( ret ) == g_aryCmpPossibilities4->end() 
              &&   g_aryCmpPossibilities5->find( ret ) == g_aryCmpPossibilities5->end() )
        {
            g_aryCmpPossibilities5->insert( ret );
            bHasNewInserted = true;
            if ( g_aryCmpPossibilities5->size() == g_MaxHashTableSize ) {
                cout << "g_CurrentWhichSet : 5 -> 6" << endl;
                g_CurrentWhichSet = 6;
            }
        }

    } else if ( g_CurrentWhichSet == 6 ) {
        if (       g_aryCmpPossibilities1->find( ret ) == g_aryCmpPossibilities1->end()  
              &&   g_aryCmpPossibilities2->find( ret ) == g_aryCmpPossibilities2->end() 
              &&   g_aryCmpPossibilities3->find( ret ) == g_aryCmpPossibilities3->end() 
              &&   g_aryCmpPossibilities4->find( ret ) == g_aryCmpPossibilities4->end() 
              &&   g_aryCmpPossibilities5->find( ret ) == g_aryCmpPossibilities5->end() 
              &&   g_aryCmpPossibilities6->find( ret ) == g_aryCmpPossibilities6->end() )
        {
            g_aryCmpPossibilities6->insert( ret );
            bHasNewInserted = true;
        }
    }


    // auto pr = g_aryCmpPossibilities.insert( ret );
    if ( bHasNewInserted ) {
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
                // The number at the even index
                bytes2Merge |= (ucNum << 4);
                bytes2Merge &= 0xF0;
                // if index is the last one and the even order , write the last bytes
                if ( i == (arySz-1) ) {
                    g_unformattedBuf->write(reinterpret_cast<const char*>(&bytes2Merge),  sizeof(bytes2Merge) );
                }
            } else {
                // The number at the odd index
                bytes2Merge |= (ucNum & 0x0F);
                g_unformattedBuf->write(reinterpret_cast<const char*>(&bytes2Merge),  sizeof(bytes2Merge) );

                bytes2Merge = 0u;
            }
        }

        if ( g_pOutputFile!=nullptr    &&    ( (g_unformattedBuf->str().size()) >= g_MaxWriteBufSz) ) {
            (*g_pOutputFile) << g_unformattedBuf->str();
            g_pOutputFile->flush();

            g_unformattedBuf->str("");
        }
        // istret = 1;
    }

    return (bHasNewInserted ? 1 : 0);
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


    g_aryCmpPossibilities1 = new unordered_set<unsigned long long>(); g_aryCmpPossibilities1->clear(); 
    g_aryCmpPossibilities2 = new unordered_set<unsigned long long>(); g_aryCmpPossibilities2->clear(); 
    g_aryCmpPossibilities3 = new unordered_set<unsigned long long>(); g_aryCmpPossibilities3->clear(); 
    g_aryCmpPossibilities4 = new unordered_set<unsigned long long>(); g_aryCmpPossibilities4->clear(); 
    g_aryCmpPossibilities5 = new unordered_set<unsigned long long>(); g_aryCmpPossibilities5->clear(); 
    g_aryCmpPossibilities6 = new unordered_set<unsigned long long>(); g_aryCmpPossibilities6->clear(); 

    g_unformattedBuf = new stringstream; g_unformattedBuf->clear();

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


    if ( !g_FileName.empty() ) {
        g_pOutputFile = new ofstream(g_FileName, ios::out | ios::trunc | ios::binary);

        if ( g_pOutputFile == nullptr  ||  !(*g_pOutputFile) ) {
            cout << "[ERROR] Can't open binary file " << g_FileName << " to write. " << endl;
            return -1;
        }
    }

    unsigned long long totalPoss = generateAllPermutation(nArraySize,  true,  collectionAry);

    unsigned long long szsz1 = static_cast<unsigned long long>( g_aryCmpPossibilities1->size() );
    unsigned long long szsz2 = static_cast<unsigned long long>( g_aryCmpPossibilities2->size() );
    unsigned long long szsz3 = static_cast<unsigned long long>( g_aryCmpPossibilities3->size() );
    unsigned long long szsz4 = static_cast<unsigned long long>( g_aryCmpPossibilities4->size() );
    unsigned long long szsz5 = static_cast<unsigned long long>( g_aryCmpPossibilities5->size() );
    unsigned long long szsz6 = static_cast<unsigned long long>( g_aryCmpPossibilities6->size() );
    unsigned long long finalSz = szsz1 + szsz2 + szsz3 + szsz4 + szsz5 + szsz6;

    cout << "Size of hTable-1 = " << szsz1 << endl;
    cout << "Size of hTable-2 = " << szsz2 << endl;
    cout << "Size of hTable-3 = " << szsz3 << endl;
    cout << "Size of hTable-4 = " << szsz4 << endl;
    cout << "Size of hTable-5 = " << szsz5 << endl;
    cout << "Size of hTable-6 = " << szsz6 << endl;
    cout << nArraySize << " element(s) inside an array  can generate  " << finalSz << " different  comparsion  array(s). Total = " << totalPoss << ", g_CurrentWhichSet = " << g_CurrentWhichSet << endl;
    // cerr << nArraySize << " element(s) inside an array  can generate  " << finalSz << " different  comparsion  array(s). Total = " << totalPoss << ", g_CurrentWhichSet = " << g_CurrentWhichSet << endl;

    // outputfile
    /*
    g_unformattedBuf->seekp(1, ios::beg);
    g_unformattedBuf->write( reinterpret_cast<const char*>(&finalSz), sizeof(finalSz) );
    g_unformattedBuf->seekp(ios::end);

    outputfile << g_unformattedBuf->str();
    */

    if ( !( g_unformattedBuf->str().empty() ) ) {
        // write the rest part of buffer
        (*g_pOutputFile) << g_unformattedBuf->str();

        g_unformattedBuf->str("");
    }
    g_pOutputFile->flush();
    // Update Final size of the possibilities count
    g_pOutputFile->seekp(1, ios::beg);
    g_pOutputFile->write( reinterpret_cast<const char*>(&finalSz), sizeof(finalSz)  );
    g_pOutputFile->flush();
    g_pOutputFile->close();

    cout << "Wrote Binary file " << g_FileName << "  [Done] " << endl;

    delete g_pOutputFile;   g_pOutputFile = nullptr;

    g_aryCmpPossibilities1->clear(); delete g_aryCmpPossibilities1; g_aryCmpPossibilities1 = nullptr;
    g_aryCmpPossibilities2->clear(); delete g_aryCmpPossibilities2; g_aryCmpPossibilities2 = nullptr;
    g_aryCmpPossibilities3->clear(); delete g_aryCmpPossibilities3; g_aryCmpPossibilities3 = nullptr;
    g_aryCmpPossibilities4->clear(); delete g_aryCmpPossibilities4; g_aryCmpPossibilities4 = nullptr;
    g_aryCmpPossibilities5->clear(); delete g_aryCmpPossibilities5; g_aryCmpPossibilities5 = nullptr;
    g_aryCmpPossibilities6->clear(); delete g_aryCmpPossibilities6; g_aryCmpPossibilities6 = nullptr;

    delete g_unformattedBuf;  g_unformattedBuf = nullptr;

    return 0;
}

