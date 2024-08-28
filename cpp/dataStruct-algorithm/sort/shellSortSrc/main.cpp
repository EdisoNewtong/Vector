#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

/*
    希尔排序法，又称  增量递减 的排序法

     但是必须保证，最后一次的增量是 ** 1 ** ，以保证 任意2个相邻的数都进行了 比较 和 排序

    int deltaAry[] = { 11, 8, 6, 4,3, 1 };
    即对于每一个 增量delta , 都要进行排序，以保证

       a[0] <= a[0+delta_1*1] <= a[0+delta_1*2] <= a[0+delta_1*3] , ...
       a[1] <= a[1+delta_1*1] <= a[1+delta_1*2] <= a[1+delta_1*3] , ...
       a[2] <= a[2+delta_1*1] <= a[2+delta_1*2] <= a[2+delta_1*3] , ...
       a[3] <= a[3+delta_1*1] <= a[3+delta_1*2] <= a[3+delta_1*3] , ...
                              
                                ...
                                ...
                                ...

*/
void shellSort(int ary[], int n,int decreasementAry[], int deArySize)
{
    for( int didx = 0; didx < deArySize; ++didx ) {
        int delta = decreasementAry[didx];
        for ( int i = delta; i < n; ++i ) {
            int tmp = ary[i];
            int j;
            for( j = i; j >= delta; j -= delta ) {
                if ( tmp < ary[j-delta] ) {
                    ary[j] = ary[j-delta];
                } else {
                    break;
                }
            }

            ary[j] = tmp;
        }
    }


}

int main(int argc, char* argv[], char* env[])
{
    const int ARY_SIZE = 13;
    int ary[ARY_SIZE] = { 81,94,11,96,12,35,17,95,28,58,41,75,12 };

    const int DE_SIZE = 3;
    // Other gap can be any arbitrary interger greater than 1
    // the last element must be !!! 1 !!! in order to 
    //     make the last round comparison pair are 2 adjcent elements
    int deAry[DE_SIZE] = {  6, 3 ,1 };

    shellSort(ary,ARY_SIZE, deAry, DE_SIZE);

    if ( is_sorted(ary, ary+ARY_SIZE) ) {
        cout << "[Nice] Array is well sorted by ascending order.   :) " << endl;
    } else {
        cout << "[ERROR] Array is !![NOT]!! well sorted by ascending order.   :( " << endl;
    }

    return 0;
}



