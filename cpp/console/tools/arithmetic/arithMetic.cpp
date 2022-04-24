#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "myException.h"
#include "buff.h"
#include "globalDirector.h"
#include "tokenMgr.h"
#include "parserOption.h"
#include "variblePool.h"
#include "dataTypeUtil.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Global Option
//
static auto bPrintFileLengthFlag = true;
////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[], char* env[])
{
    if ( argc < 2 ) {
        cout << ParserOption::getUserManual() << endl;
        return -1;
    }
    // else    :   argc >= 4


    if ( argc > 2 ) {
        //
        // Parser cmd-line-argument
        //
        string errorMsg;
        vector<string> arglist;
        for( int idx = 1; idx < (argc-1); ++idx ) {
            arglist.push_back( string(argv[idx]) );
        }

        auto bOptionParserFlag = ParserOption::analyzeOption(arglist , errorMsg);
        if ( !bOptionParserFlag ) {
            cout << errorMsg << endl
                 << ParserOption::getUserManual() << endl;
            return -1;
        }
    }


    // argc == 2   ./<program>   <fileName>
    //
    // Read File
    //
    string fname(argv[argc-1]);
    ifstream file(fname.c_str(), ios::in | ios::binary);
    if ( !file ) {
        cout << "[ERROR] : File < " << fname << "> not found " << endl;
        return -1;
    }

    file.seekg(0, ios::end); // file-pointer move to the end of file
    auto filelen = static_cast<int>( file.tellg() );
    file.seekg(0, ios::beg); // file-pointer move to the begin of file

    if ( bPrintFileLengthFlag ) {
        cout << "==================================================" << endl;
        cout << "file.length = " << filelen << " byte(s)" << endl;
        cout << "==================================================" << endl
             << endl;
    }

    if ( filelen == 0 ) {
        cout << "[DONE] : Nothing to Do , File has 0 byte " << endl;
        file.close();
        return 0;
    }

    DataTypeUtil::init();
    VariblePool::init();
    TokenMgr::init();

    // if ( !DataTypeUtil::testCase() ) {
    //     cout << "testCase Failed" << endl;
    // } else {
    //     cout << "testCase Successful" << endl;
    // }

    //////////////////////////////////////////////////////////////////////////////////////////
    //
    // Core Core Core :  Do Parser
    //
    //////////////////////////////////////////////////////////////////////////////////////////
    GlobalDirector director;
    try {
        // prepare file Buff
        char* filebuf = new char[filelen];
        file.read(filebuf, filelen);
        file.close();
        Buff* fileBuff = new Buff(filebuf,filelen);

        director.setData(fileBuff);
        director.doParse();
    } catch( const MyException& e ) { 
        // cout << "[ERROR] : Meet a self-defined exception : " << e.what() << endl;
        cout << "[ERROR] : Meet a self-defined exception : " << e.getExceptionDetail() << endl;
    } catch(const exception& e) {
        cout << "[ERROR] : Meet a common exception : " << e.what() << endl;
    } catch( ... ) {
        cout << "[ERROR] : Meet an unexpection exception : " << endl;
    }

    VariblePool::finalize();
    TokenMgr::release();
    DataTypeUtil::finalize();

    return 0;
}

