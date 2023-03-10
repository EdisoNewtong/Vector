#include <iostream>
#include <string>
#include <exception>
using namespace std;



void testCaseForDynamicArray3D(int n1, int n2, int n3)
{
    const int INIT_VALUE = 100;
    //
    // Alloc Memory Part , Notes , [DO NOT!!]  Add parenthesis arround   (int**)   or   (int*)
    //
    int*** p3DAry = new int**[n1]; // A 3D array is definded with 3 '*' 
                                   //            and alloced with 2 '*'

    for( int i = 0; i < n1; ++i ) {
        p3DAry[i] = new int*[n2];  // A 2D array's pointer is alloced with 1 '*'
        for( int j = 0; j < n2; ++j ) {
            p3DAry[i][j] = new int[n3]; // A 1D array's pointer is alloced with 0 '*'
        }
    }


    int element = INIT_VALUE;
    // Assign Element's value
    for( int i = 0; i < n1; ++i ) {
        for( int j = 0; j < n2; ++j ) {
            for( int k = 0; k < n3; ++k ) {
                p3DAry[i][j][k] = ++element;
            }
        }
    }


    
    //
    // Print 
    //
    const string SC_TAB1(2, ' ');
    const string SC_TAB2(4, ' ');
    cout << "The beginning element of the array is " << (INIT_VALUE+1) << endl;
    cout << "int ary3D[" << n1 << "][" << n2 << "][" << n3 << "] = " << endl;
    cout << "{ " << endl;
    for( int i = 0; i < n1; ++i ) {
        cout << SC_TAB1 << "{ // --------------------[" << i << "]--------------------" << endl;
        for( int j = 0; j < n2; ++j ) {
            cout << SC_TAB2 << "{ ";
            for( int k = 0; k < n3; ++k ) {
                cout << p3DAry[i][j][k] << ( (k<n3-1) ? ", " : " ");
            }
            cout << " }" << ( (j < n2-1) ? "," : "") << ((j < n2-1) ? "  // [" : "   // [") << i << "][" << j << "]" << endl;
        }
        cout << SC_TAB1 << "}" << ((i<n1-1)  ? "," : "") << endl;
    }
    cout << "} " << endl;



    //
    // Release Memory Part
    //
    for( int i = 0; i < n1; ++i ) {
        for( int j = 0; j < n2; ++j ) {
            delete [] p3DAry[i][j];    // release the memory of an 1D Array whose size is N3
            p3DAry[i][j] = nullptr;
        }

        delete [] p3DAry[i];  // release the memory of a 2D Array whose size is N2
        p3DAry[i] = nullptr;
    }

    delete [] p3DAry;  // release the memory of a 3D Array whose size is N1
    p3DAry = nullptr;

    cout << endl;
}


void printUsage()
{
    cout << R"(
--------------------------------------------------
Usage : 
--------------------------------------------------
// Each of N1/N2/N3 is a greater than Zero.
$ dynamicArray <N1> <N2> <N3>)" << endl << endl;
}

int main(int argc, char* argv[])
{
    if ( argc < 4 ) {
        printUsage();
        return -1;
    }

    bool bIsOK = true;
    try {
        int n1 = stoi( argv[1] );
        if ( n1 <= 0 ) {
            cout << "N1 is not a positive number." << endl;
            return -1;
        }

        int n2 = stoi( argv[2] );
        if ( n2 <= 0 ) {
            cout << "N2 is not a positive number." << endl;
            return -1;
        }

        int n3 = stoi( argv[3] );
        if ( n3 <= 0 ) {
            cout << "N3 is not a positive number." << endl;
            return -1;
        }

        testCaseForDynamicArray3D(n1,n2,n3);
    } catch ( const std::invalid_argument& e ) {
        bIsOK = false;
        cout << "[ERROR] Invalid_argument argument : " << e.what() << endl;
    } catch ( const std::out_of_range& e ) {
        bIsOK = false;
        cout << "[ERROR] Out_of_range argument : " << e.what() << endl;
    } catch( ... ) {
        bIsOK = false;
        cout << "[ERROR] Unknown exception " << endl;
    }


    if ( !bIsOK ) {
        return -1;
    }

    return 0;
}

