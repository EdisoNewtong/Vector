#include <iostream>
using namespace std;

// 
// # new expression
//


//  alloc an object by the given destination placeaddress
template<typename T>
void test1()
{
    alignas(T) unsigned char buffer_1[sizeof(T)] = { 0 };
    // 1 ~ 16
    for( int i = 0; i < 4; ++i ) {
        buffer_1[i] = i+1;
    }
    cout << "#0. &buffer[0] = " << static_cast<void*>( &buffer_1[0] ) << " = " << static_cast<void*>( buffer_1 ) << endl;
    int idx;
    idx = 0;
    cout << "#0 ============================================" << endl;
    for( auto e : buffer_1 ) {
        // [0] = 1
        // [1] = 2
        // [2] = 3
        // [3] = 4
        cout << idx++ << ".  " << std::dec << static_cast<int>( e ) <<  " = 0x" <<  std::hex << static_cast<int>( e ) << endl;
    }
    cout << "#0.============================================" << endl << endl;


    //
    // ::(optional) new (placement-args ) type new-initializer (optional)	(4)
    //
    T* tptr = new(buffer_1) T(0xAB12CD34); // Construct a “T” object, placing it directly into your
                                 // pre-allocated storage at memory address “buf”.

    // Same address as original address
    cout << "#1. &buffer[0] = " << static_cast<void*>( &buffer_1[0] ) << " = " << static_cast<void*>( buffer_1 ) << endl;
    cout << "#1 ============================================" << endl;
    idx = 0;
    for( auto e : buffer_1 ) {
        // [0] = 0x34
        // [1] = 0xCD
        // [2] = 0x12
        // [3] = 0xAB
        cout << idx++ << ".  " << std::dec << static_cast<int>( e ) <<  " = 0x" <<  std::hex << static_cast<int>( e ) << endl;
    }
    cout << "#0.============================================" << endl;
    
    // main(94722,0x1ecdf7100) malloc: *** error for object 0x16db9ee8c: pointer being freed was not allocated
    // main(94722,0x1ecdf7100) malloc: *** set a breakpoint in malloc_error_break to debug
    /****************************************************************************************************

    delete tptr; [runtime ERROR]

    ****************************************************************************************************/

    tptr->~T(); // [CORRECT]
}


int main(int argc, char* argv[], char* env[])
{
    test1<int>();
    return 0;
}





