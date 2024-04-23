#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

/*

               1 ^ 1
               2 ^ 2
               3 ^ 3
               4 ^ 4
               5 ^ 5
               6 ^ 6
               7 ^ 7
               8 ^ 8
               9 ^ 9
Too Large :  10 ^ 10


*/



int**  genSortTestCaseArray(unsigned long long arySz)
{
    if (  arySz <= 1 ) {
        cout << "arySz can't be  <= 1. " << endl;
        return nullptr;
    }

    // arySz >= 2
    unsigned long long genSize = static_cast<unsigned long long>( pow(arySz, arySz) );

    int** allCollection = new int*[genSize];
    for( unsigned long long i = 0; i < genSize; ++i ) {
        allCollection[i] = new int[arySz];
    }

    int* idxAry = new int[arySz];
    for ( unsigned long long i = 0 ; i < arySz; ++i ) { idxAry[i] = 0; }

    unsigned long long genIdx = 0;
    // unsigned long long rangeCnt = static_cast<unsigned long long>( pow(arySz, arySz-1) );
    do {
        // unsigned long long i = static_cast<int>( genIdx / rangeCnt );
        // unsigned long long j = static_cast<int>( genIdx % rangeCnt );

        // if ( i < arySz && j < arySz ) {

        cout << setw(4) << setfill('0') << (genIdx+1) << ".   [ ";
        for ( unsigned long long idx = 0; idx < arySz; ++idx ) { 
            int num = idxAry[idx] + 1;
            allCollection[genIdx][idx] = num;
            cout << num << ((idx!=arySz-1) ? ", " : " ");
        }
        cout << " ] " << endl;

        // } else {
            //cout << "either i or j is out of range : i = " << i << ", j = " << j << endl;
        // }

        int lastIdx = arySz-1;
        ++idxAry[lastIdx];
        while ( lastIdx > 0 && static_cast<unsigned long long>( idxAry[lastIdx] ) >= arySz ) {
            for ( int j = lastIdx; j <= lastIdx; ++j ) {
                idxAry[j] = 0;
            }

            --lastIdx;
            if ( lastIdx < 0 ) {
                break;
            }

            ++idxAry[lastIdx];
        }

        ++genIdx;
    } while ( static_cast<unsigned long long>( idxAry[0] )  < arySz );

    cout << "genIdx = " << genIdx << endl;

    // release tmp index array's memory
    delete [] idxAry; idxAry = nullptr;

    return allCollection;
}


int main(int argc, char* argv[])
{
    unsigned long long sz = stoul( string(argv[1]) );
    auto ppArray = genSortTestCaseArray(sz);

    if ( ppArray != nullptr ) {
        for( unsigned long long i = 0; i < sz; ++i ) {
            delete [] ppArray[i]; ppArray[i] = nullptr;
        }

        delete [] ppArray;
        ppArray = nullptr;
    }


    return 0;
}


