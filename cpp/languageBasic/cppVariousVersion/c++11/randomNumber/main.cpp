#include <iostream>
#include <ctime>
#include <random>
using namespace std;

void genRandomNumber()
{

    // std::time_t seed 
	auto seed = time( nullptr );
	// default_random_engine dftEng;       // no   seed constructor
	default_random_engine dftEng(seed); // with seed constructor

    int a = 1;
    int b = 10000;

    /********************************************************************************
     
      a must <= b , Otherwise the program will raise an unbehaviored result

      Condition ( a <= b )  is a must by the programmer 
    ********************************************************************************/ 
    //                          range :[ a,  b ]
	uniform_int_distribution<int> range(  a,  b );
    int num = 0;
    unsigned long long tryTimes = 0;
    do {
        num = range(dftEng);
        ++tryTimes;
    } while( num != b );
    // it can reach the maxValue
    cout << "Finally , num = " << num << "   ( for " << tryTimes << " tries ) ." << endl;
}


int main(int argc, char* argv[], char* env[])
{
    genRandomNumber();
    return 0;
}







