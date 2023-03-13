#include <iostream>
#include <random>    // for random(...)
#include <ctime>     // for time(...)
#include <algorithm> // nth_elements(...)
#include <fstream>   // to write errorCase into file
using namespace std;


int quickSeletionNth(int* ary, int sz, int leftIdx, int rightIdx, int N_th)
{
	int rangeSz = (rightIdx - leftIdx) + 1;
	if ( rangeSz == 1 ) {
		return ary[leftIdx];
	}


	int povitIdx = leftIdx;
	int povitNumber = ary[povitIdx];
	int i = leftIdx;
	int j = rightIdx;

	while ( i < j )
	{
		for( ; (i < j) &&  (ary[j] >  povitNumber); --j ) { ; }
		for( ; (i < j) &&  (ary[i] <= povitNumber); ++i ) { ; }

		if ( i < j ) {
			int tmp = ary[i];
			ary[i] = ary[j];
			ary[j] = tmp;
		}
	}
	// Swap pivot-Number   <-->    i
	int tmp = ary[i];
	ary[i] = povitNumber;
	ary[povitIdx] = tmp;


	int leftPart_Sz  = ( (i-1) - leftIdx  ) + 1;
	// int rightPart_Sz = ( rightIdx - (i+1) ) + 1;

	if ( N_th <= leftPart_Sz ) {
		return quickSeletionNth(ary, sz, leftIdx, i-1, N_th);
	} else if ( N_th == (1 + leftPart_Sz) ) { 
		return ary[i];
	} else {
		return quickSeletionNth(ary, sz, (i+1), rightIdx, (N_th - leftPart_Sz -1) );
	}

}


bool checkNthIsCorrect(int arySz, int N_th, int randomSeed)
{
	bool bRet = false;

	int* originAry       = new int[arySz];
	int* transformAry    = new int[arySz];
	int* checkCorrectAry = new int[arySz];

	default_random_engine dftEng(randomSeed);

	uniform_int_distribution<int> range(0, arySz * 100);
	for( int i = 0; i < arySz; ++i ) {
		int randomNum = range(dftEng);
		originAry[i] = transformAry[i] = checkCorrectAry[i] = randomNum;
	}

	int* nthIterator = checkCorrectAry + (N_th-1);
	nth_element(checkCorrectAry, nthIterator, checkCorrectAry + arySz);

	int nthNumber = quickSeletionNth(transformAry, arySz, 0, arySz-1, N_th);
	bRet = (nthNumber == *nthIterator );
	if ( !bRet ) {
		const string errorLogFile = "ErrorCase-Array.txt";
		ofstream file(errorLogFile.c_str(), (ios::out | ios::ate ) );
		if ( file ) {
			file << " [ ";
			for( int i = 0; i < arySz; ++i ) {
				file << originAry[i] << ( (i < arySz-1) ? ", " : "" );
			}
			file << " ] " << endl;
			file.flush();
			file.close();
		} else {
			cout << "[ERROR] Can't open error long file : " << errorLogFile << endl;
		}
	}


	delete [] originAry;         originAry = nullptr;
	delete [] transformAry;      transformAry = nullptr;
	delete [] checkCorrectAry;   checkCorrectAry = nullptr;
	return bRet;
}

void printUsage()
{
	const string str_USAGE = R"(------------------------------
Usage :
------------------------------
$ main <Ary-Size>   <N-th>   <testCount> 
)";
	cout << str_USAGE << endl;

}



int main(int argc, char* argv[])
{
	if ( argc < 4 ) {
		printUsage();
		return -1;
	}

	int arySz = atoi( argv[1] );
	if ( arySz <= 0 ) {
		cout << "[ERROR] Array.size <= 0 ." << endl;
		printUsage();
		return -1;
	}

	int n_th = atoi( argv[2] );
	if ( n_th <= 0 ) {
		cout << "[ERROR] N-th <= 0 ." << endl;
		printUsage();
		return -1;
	} else if ( n_th > arySz ) {
		cout << "[ERROR] N-th >= array.size ." << endl;
		printUsage();
		return -1;
	}


	int testCount = atoi( argv[3] );
	if ( testCount <= 0 ) {
		cout << "[ERROR] testCount <= 0 ." << endl;
		printUsage();
		return -1;
	}


	int seed = time(nullptr);
	int errorCnt = 0;
	for( int idx = 0; idx < testCount; ++idx )
	{
		cout << "Processing TestCase [" << (idx+1) << "] ..." << endl;
		if ( !checkNthIsCorrect(arySz, n_th, (seed + idx)) ) {
			++errorCnt;
		}
	}

	cout << "--------------------------------------------------" << endl;
	cout << "Test-Case Done : " << (errorCnt) << " / " << testCount << " Failed." << endl;
	cout << "--------------------------------------------------" << endl;

	return 0;

}
