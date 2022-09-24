#include <iostream>
#include <iomanip>
#include <ctime>
#include <random>    // to generate random number
#include <map>    
using namespace std;



void genInt(int minVal, int maxVal,int times)
{
	cout << "minVal = " << minVal << endl;
	cout << "maxVal = " << maxVal << endl;
	cout << "generate times : " << times << endl;

	// create a default engine as source of randomness
	auto seed = time(nullptr);
	// default_random_engine dftEng;       // no   seed constructor
	default_random_engine dftEng(seed); // with seed constructor
	map<int, int> summaryMp;

	//****************************************************************************************************
	// Core Core Core
	//
	//		The passed arguments must meet the demand the following condition :  minVal <= maxVal
	// !!! Otherwise !!! : the generated random number will get the !!!!wrong!!! value
	//****************************************************************************************************

	// both included                   [minVal, maxVal]
	uniform_int_distribution<int> range(minVal, maxVal);
	for( int i = 0; i < times; ++i ) 
	{
		int randomNum = range(dftEng);
		cout << setw(3) << (i+1) << ".  => " << randomNum << endl;
		auto it = summaryMp.find(randomNum);
		if ( it == summaryMp.end() ) {
			summaryMp.insert( make_pair(randomNum,1) );
		} else {
			++(it->second);
		}
	}

	cout << "====================================================================================================" << endl;
	int idx = 1;
	for( const auto& pr : summaryMp ) 
	{
		cout << idx << ".  " << pr.first << " occurs   " << pr.second << " times." << endl;
		++idx;
	}
}

template<class T>
void genFloat(T minVal, T maxVal,int times)
{
	cout << "minVal = " << minVal << endl;
	cout << "maxVal = " << maxVal << endl;
	cout << "generate times : " << times << endl;

	// create a default engine as source of randomness
	auto seed = time(nullptr);

	// default_random_engine dftEng;    // no   seed  constructor
	default_random_engine dftEng(seed); // with seed  constructor


	// minVal include, maxVal not included          
	//
	//                                [minVal, maxVal)
	uniform_real_distribution<T> range(minVal, maxVal);
	for( int i = 0; i < times; ++i ) 
	{
		T randomNum = range(dftEng);
		cout << setw(3) << (i+1) << ".  => " << randomNum << endl;
	}

}





int main(int argc, char* argv[])
{
	if ( argc < 4 ) {
		cout << "[ERROR] , require 2 numbers to make range. " << endl;
		return -1;
	}

	try {
		// *** integer version ***
		int minNum = stoi( argv[1] );
		int maxNum = stoi( argv[2] );
		int times  = stoi( argv[3] );
		genInt(minNum, maxNum, times);

		
		// *** float version ***
		// float fNum1 = stof( argv[1] );
		// float fNum2 = stof( argv[2] );
		// int   times = stoi( argv[3] );

		// genFloat<float>(fNum1, fNum2, times);


		// *** double version ***
		// double fNum1 = stod( argv[1] );
		// double fNum2 = stod( argv[2] );
		// int   times  = stoi( argv[3] );

		// genFloat<double>(fNum1, fNum2, times);
		
	} catch ( const exception& e ) {
		cout << "[Exception] : " << e.what() << endl;
	}
}
