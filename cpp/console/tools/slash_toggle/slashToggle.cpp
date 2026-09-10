#include <iostream>
#include <string>
using namespace std;


void print_Usage()
{
    cout << R"($ slashToggle --[s|bs|slash|backslash]   <Src Path>"
// s (slash)      : slash           \ -> /
// bs (backslash) : back slash      / -> \
)"       << endl;

}

int convert(string& in_outstr, int flag)
{
	if ( flag == 0 ) {
		cout << "Invalid flag , flag must be 1 or 2! " << endl;
		return -1;
	}

    int matchCnt = 0;
    if ( flag == 1 ) {
        //        '\'  ===>  '/'
        for ( size_t i = 0; i < in_outstr.size(); ++i ) {
            if ( in_outstr[i] == '\\' ) {
                ++matchCnt;
                in_outstr[i] = '/';
            }
        }
        cout << "\tConverting ...     '\\' => '/'  [DONE] " << matchCnt << " occurrence matched" << endl;
    } else {
        //        '/'  ===>  '\'
        for ( size_t i = 0; i < in_outstr.size(); ++i ) {
            if ( in_outstr[i] == '/' ) {
                ++matchCnt;
                in_outstr[i] = '\\';
            }
        }
        cout << "\tConverting ...     '/' => '\\'  [DONE] " << matchCnt << " occurrence matched" << endl;
    }
    return matchCnt;

}

int main(int argc, char* argv[], char* env[])
{
    // The command [Must] contain  at least 3 arguments 
    if ( argc < 3 ) {
        print_Usage();
        return -1;
    }

    // type == 1 : 
    // type == 2 :
    int type = 0;
    string arg2( argv[1] );
    if ( arg2 == "--s" || arg2 == "--slash" ) {
        type = 1;    // '\'  ->  '/'
    } else if ( arg2 == "--bs" || arg2 == "--backslash" ) {
        type = 2;    // '/'  ->  '\'  
    }

    string src( argv[2] ); string old=src;
    int res = convert(src, type);
	cout << endl;
	if ( res >= 0 ) {
		cout << " Original : \"" << old << "\"" << endl;
		cout << "Converted : \"" << src << "\"" << endl;
	}

    return 0;
}


