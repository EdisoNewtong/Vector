#include <iostream>
#include <random>    // for random(...)
#include <ctime>     // for time(...)
#include <algorithm> // is_sorted(...)
// #include <fstream>   // to write errorCase into file
using namespace std;


// MSD : Most Significant Digit
// LSD : Last Significant Digit

static bool printOriginalArray = true;
static bool printFlag          = false;


struct Bucket
{
	int*  filledNumbers;
	int   index;
};

int getDigitBits(int num)
{
	int retBits = 1;
	while ( (num /= 10) != 0 ) {
		++retBits;
	}

	return retBits;
}

int get_NBitForNumber(int num, int nBit)
{
	// nBit >=2
	for( int i = 1; i <= nBit; ++i ) {
		if ( i == nBit ) {
			num %= 10;
			break;
		}

		num /= 10;
		if ( num == 0 ) {
			break;
		}
	}

	return num;
}

void radixSortAry_LSD(int* ary, int sz)
{
	int longestBits = 1;
	for( int i = 0; i < sz; ++i ) {
		int nBits = getDigitBits( ary[i] );
		if ( nBits > longestBits ) {
			longestBits = nBits;
		}
	}

	const int BUCKET_CNT = 10;
	Bucket* bucketAry = new Bucket[BUCKET_CNT];
	for( int i = 0; i < BUCKET_CNT; ++i ) {
		bucketAry[i].filledNumbers = new int[sz];
		bucketAry[i].index = 0;
	}

	for( int nBits = 1; nBits <= longestBits; ++nBits )
	{
		// init idx every loop
		for( int i = 0; i < BUCKET_CNT; ++i ) {
			bucketAry[i].index = 0;
		}

		// put every bits into the proper bucket
		for( int i = 0; i < sz; ++i ) {
			int nDigit = get_NBitForNumber(ary[i], nBits);
			bucketAry[nDigit].filledNumbers[ bucketAry[nDigit].index++ ] = ary[i];
		}

		// re-order the array by bucket sort
		int idx = 0;
		for( int i = 0; i < BUCKET_CNT; ++i ) {
			for( int j = 0; j < bucketAry[i].index; ++j ) {
				ary[idx++] = bucketAry[i].filledNumbers[j];
			}
		}

		if ( printFlag ) {
			cout << "  nBits = " << nBits << ". [ ";
			for( int i = 0; i < sz; ++i ) {
				cout << ary[i] << "  ";
			}
			cout << " ] " << endl;
		}
	}


	// Release Bucket
	for( int i = 0; i < BUCKET_CNT; ++i ) {
		delete [] bucketAry[i].filledNumbers;
		bucketAry[i].filledNumbers = nullptr;
	}
	delete [] bucketAry; bucketAry = nullptr;

}



void radixSortAry_MSD(int* ary,int* subAry, int subSz, int given_nBit_th, int* pIdx)
{
	static const int BUCKET_CNT = 10;

	int* processedAry   = subAry;
	int  processedArySz = subSz;
	
	int longestBits = 1;
	if ( given_nBit_th  == 0 ) {
		for( int i = 0; i < processedArySz; ++i ) {
			int nBits = getDigitBits( processedAry[i] );
			if ( nBits > longestBits ) {
				longestBits = nBits;
			}
		}

		given_nBit_th = longestBits;
	}

	Bucket* bucketAry = new Bucket[BUCKET_CNT];
	for( int i = 0; i < BUCKET_CNT; ++i ) {
		bucketAry[i].filledNumbers = new int[processedArySz];
		bucketAry[i].index = 0;
	}

	int nBits = given_nBit_th;
	// put every bits into the proper bucket
	for( int i = 0; i < processedArySz; ++i ) {
		int nDigit = get_NBitForNumber( processedAry[i], nBits);
		bucketAry[nDigit].filledNumbers[ bucketAry[nDigit].index++ ] = processedAry[i];
	}

	for( int i = 0; i < BUCKET_CNT; ++i ) {
		int innerCnt = bucketAry[i].index;
		if ( innerCnt > 0 ) {
			if ( innerCnt == 1 ) {
				// index == 1
				ary[(*pIdx)++] = bucketAry[i].filledNumbers[0];
			} else {
				// innerCnt >= 2
				if ( nBits == 1 ) {
					for ( int k = 0; k < innerCnt; ++k ) {
						ary[(*pIdx)++] = bucketAry[i].filledNumbers[k];
					}
				} else {
					int* subbAry = new int[ innerCnt ];
					for( int j = 0; j < innerCnt; ++j ) {
						subbAry[j] = bucketAry[i].filledNumbers[j];
					}
					radixSortAry_MSD(ary, subbAry, innerCnt, (nBits-1), pIdx );
					delete [] subbAry; subbAry = nullptr;
				}

			}
		}
	}


	if ( printFlag ) {
		cout << "  nBits = " << nBits << ". [ ";
		for( int i = 0; i < processedArySz; ++i ) {
			cout << processedAry[i] << "  ";
		}
		cout << " ] " << endl;
	}


	// Release Bucket
	for( int i = 0; i < BUCKET_CNT; ++i ) {
		delete [] bucketAry[i].filledNumbers;
		bucketAry[i].filledNumbers = nullptr;
	}
	delete [] bucketAry; bucketAry = nullptr;

}




int main(int argc, char* argv[])
{
	if ( argc < 2 ) {
		cout << "[ERROR] Missing argument for test-case count. " << endl;
		return -1;
	}

	const int FIXED_CNT = 10;
	int errorCnt = 0;
	int testCount = atoi( argv[1] );
	int* pAry         = new int[FIXED_CNT];
	int* transformAry = new int[FIXED_CNT];

	int seed = time(nullptr);
	// default_random_engine dftEng(seed + i);
	default_random_engine dftEng(seed);
	uniform_int_distribution<int> range(0, 1000);


	int innerIdx = 0;
	for( int i = 0; i < testCount; ++i ) {
		innerIdx = 0;
		if ( printOriginalArray ) {
			cout << (i+1) << ". [ ";
		}
		
		// generate an array with 'FIXED_CNT' elements   randomly
		for( int j = 0; j < FIXED_CNT; ++j ) {
			pAry[j] = transformAry[j] =  range(dftEng);
			if ( printOriginalArray ) {
				cout << pAry[j] << " ";
			}
		}

		if ( printOriginalArray ) {
			cout << " ] " << endl;
		}

		cout << "Doing " << (i+1) << " / " << testCount << " Sorting ... " << endl;
		// radixSortAry_LSD(transformAry, FIXED_CNT);
		radixSortAry_MSD(transformAry, transformAry, FIXED_CNT, 0, &innerIdx);

		if ( !is_sorted(transformAry, transformAry + FIXED_CNT) ) {
			++errorCnt;
			cout << errorCnt << ". [ ";
			for( int i = 0; i < FIXED_CNT; ++i ) {
				cout << pAry[i] << " ";
			}
			cout << " ]  <== Original Array  " << endl;

			cout << errorCnt << ". [ ";
			for( int i = 0; i < FIXED_CNT; ++i ) {
				cout << transformAry[i] << " ";
			}
			cout << " ] is <NOT> well Asc-Sorted. " << endl;
		}
	}

	cout << "--------------------------------------------------" << endl;
	cout << "[DONE] " << errorCnt << " / " << testCount << " Failed." << endl;
	cout << "--------------------------------------------------" << endl;


	delete [] pAry;          pAry = nullptr;
	delete [] transformAry;  transformAry = nullptr;
	return 0;

}
