#include <iostream>
#include <string>
using namespace std;

void getSubString(const string& inputstr, bool bAllowedEmptyStr)
{
    int sz = static_cast<int>( inputstr.size() );
    int cnt = 0;
    if ( sz == 0 ) {
        cout << ++cnt << ". " <<  "\"\"" << endl;
        return;
    } else {
        if ( bAllowedEmptyStr ) {
            cout << ++cnt << ". " <<  "\"\"" << endl;
        }

        // sz > 0
        for( int sublen = 1; sublen <= sz;  ++sublen )
        {
            for ( int idx = 0; idx <= (sz-sublen); ++idx ) 
            {
                string substr = inputstr.substr(idx,sublen);
                cout << ++cnt << ". " << substr << endl;
            }
        }
    }
}


int main(int argc, char* argv[], char* env[])
{
    // cout << "argc = " << argc << endl;
    // cout << "argv[0] = " << argv[0] << endl;
    // 
    // if ( argc >= 2 ) {
    //     cout << "argv[1] = " << argv[1] << endl;
    // }

    if ( argc == 1 ) {
        cout << "[ERROR]. require 1 string argument , missing input string !! " << endl;
        return 0;
    }

    bool allowedEmptyStrFlag = false;
    getSubString( string(argv[1]) , allowedEmptyStrFlag);

    return 0;
}

