#include <iostream>
#include <algorithm>
#include <string>
using namespace std;




void merge2Seperate(int* ary, int begIdx, int middle, int endIdx)
{
    int len = endIdx - begIdx + 1;
    int* tmpAry = new int[len];
    int i = begIdx;
    int j = middle;
    int k = 0;


    while ( i < middle && j <= endIdx )
    {
        if ( ary[i] < ary[j] ) {
            tmpAry[k++] = ary[i];
            ++i;
        } else {
            tmpAry[k++] = ary[j];
            ++j;
        }
    }

    if ( i >= middle ) {
        for( ; j <= endIdx; ++j ) {
            tmpAry[k++] = ary[j];
        }
    } else {
        for( ; i < middle ; ++i ) {
            tmpAry[k++] = ary[i];
        }
    }

    for ( int idx = 0; idx < len; ++idx )
    {
        ary[begIdx + idx] = tmpAry[idx];
    }

    delete [] tmpAry;
}


void mergeSort(int* ary,int begIdx, int endIdx)
{
    if ( begIdx < endIdx ) {
        int middle = (begIdx + endIdx) / 2;
        mergeSort(ary, begIdx, middle );
        mergeSort(ary, middle+1,  endIdx);
        merge2Seperate(ary, begIdx, middle+1, endIdx);
    }
}


void MergeSort_2Way(int len, int testCnt)
{
    int failCnt = 0;
    bool needPrintOriginal = false;

    int* backAry = new int[len];

    for( int idx = 0; idx < testCnt; ++idx ) {
        bool isSuccessful = true;
        int* ary = new int[len];

        for( int i = 0; i < len; ++i ) {
            ary[i] = rand();
            backAry[i] = ary[i];
        }

        if ( needPrintOriginal ) {
            for( int i = 0; i < len; ++i ) {
                cout << ary[i] << " ";
            }
            cout << endl;
        }


        if ( is_sorted(ary,ary+len) ) {
            cout << (idx+1) << ". Well Sorted" << endl;
            isSuccessful = true;
        } else {
            cout << (idx+1) << ". <Not>  Well Sorted" << endl;
            cout << "     Sorting ... " << endl;
            mergeSort(ary, 0, len-1);

            if ( !is_sorted(ary,ary+len) ) {
                cout << "[ERROR] After Sort , Not well sorted.  " << endl;

                cout << "Original : ";
                for( int i = 0; i < len; ++i ) {
                    cout << backAry[i] << ", ";
                }
                cout << endl;

                cout << "Sorted   : ";
                for( int i = 0; i < len; ++i ) {
                    cout << ary[i] << ", ";
                }
                cout << endl;

                isSuccessful = false;
                ++failCnt;
            }
        }


        delete [] ary;
        if ( !isSuccessful ) {
            break;
        }
    }


    delete [] backAry;
    cout << "[INFO] : failCnt = " << failCnt << endl;
}

void printUsage()
{
    cout << "----------" << endl
         << " Usage :  " << endl
         << "----------" << endl
         << " $ ./main   <Array-Size>   <Test-Case>" << endl
         << endl;
}


int main ( int argc, char *argv[], char *env[])
{
    if ( argc < 3 ) {
        printUsage();
        return -1;
    }

    MergeSort_2Way( atoi(argv[1]),  atoi(argv[2]) );
    return 0;
}

