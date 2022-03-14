#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;


void printUsgae()
{
    cout << "Usage : " << endl;
    cout << "       hexstring <inputFile>" << endl;
    cout << "or     hexstring [ -o <outputFile> ] <inputFile>" << endl;
}


int main(int argc,char* argv[], char* env[])
{
    if ( argc < 2 ) {
        printUsgae();
        return -1;
    }

    const string fixedarg1("-o");

    // argc >= 2
    string arg1( argv[1] );
    if ( arg1 == fixedarg1 ) {
        if ( argc != 4 ) {
            printUsgae();
            return -1;
        }

        // == 4
        string arg2( argv[2] ); // -o  <Output File>
        string arg3( argv[3] ); // input file
        
        ifstream inputfile(arg3.c_str() , ios::in | ios::binary);
        if ( !inputfile ) {
            cout << "[ERROR] Can't open file  : " << arg3 << "  or file is not existed  :(" << endl;
            return -1;
        }

        if ( arg2 == arg3 ) {
            inputfile.close();
            cout << "[ERROR] Specifying same input and output file is not allowed :(" << endl;
            return -1;
        }

        
        ifstream testOutputfile(arg2.c_str() , ios::in | ios::binary);
        if ( testOutputfile ) {
            cout << "[ERROR] File : " << arg2 << " is already existed ! Please specify a new file name. " << endl;
            return -1;
        }
        testOutputfile.close();

        ofstream outputfile( arg2.c_str(), ios::out | ios::trunc | ios::binary);
        if ( !outputfile ) {
            cout << "[ERROR] Can't open file : " << arg2 << "  or permission deny " << endl;
            return -1;
        }

        // get the input file's length
        inputfile.seekg(0, ios::end);
        size_t filelen = static_cast<int>( inputfile.tellg() );
        inputfile.seekg(0, ios::beg);

        outputfile << "\"";
        if ( filelen > 0 ) {
            for ( size_t idx = 0; idx < filelen; ++idx ) {
                char ch = 0;
                inputfile.read(&ch, sizeof(ch) );
                // cout << std::dec << idx << ". 0x" << std::hex << static_cast<int>(ch & 0xFFU) << endl;
                outputfile << "\\x" << std::hex << setw(2) << setfill('0') << std::uppercase << static_cast<int>(ch & 0xFFU);
            }
        }
        outputfile << "\"";

        outputfile.flush();
        outputfile.close();
        cout << "[DONE] hexstring result is written into file : " << arg2 << endl;
    } else {
        // !=  "-o" , treat as       "hexstring <inputFile>"   command format
        ifstream inputfile(arg1.c_str() , ios::in | ios::binary);
        if ( !inputfile ) {
            cout << "[ERROR] Can't open file  : " << arg1 << "  or file is not existed  :(" << endl;
            return -1;
        }

        // file is existed , output convert result to console
        inputfile.seekg(0, ios::end);
        size_t filelen = static_cast<int>( inputfile.tellg() );
        inputfile.seekg(0, ios::beg);

        // cout << "file.length = " <<  filelen << endl;

        ///////////////////////////////////////////////////////////////////////////
        // Begin
        cout << "\"";

        if ( filelen > 0 ) {
            for ( size_t idx = 0; idx < filelen; ++idx ) {
                char ch = 0;
                inputfile.read(&ch, sizeof(ch) );
                // cout << std::dec << idx << ". 0x" << std::hex << static_cast<int>(ch & 0xFFU) << endl;
                cout << "\\x" << std::hex << setw(2) << setfill('0') << std::uppercase << static_cast<int>(ch & 0xFFU);
            }
        }

        cout << "\""; 
        cout << endl;
        // End
        ///////////////////////////////////////////////////////////////////////////
    }

    return 0;
}
