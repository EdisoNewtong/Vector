#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#ifndef _WIN32 
    #include <unistd.h>
#else
    #include <windows.h>
#endif


#include "myException.h"
#include "buff.h"
#include "globalDirector.h"
#include "tokenMgr.h"
#include "cmdOptions.h"
#include "variblePool.h"
#include "dataTypeUtil.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Global Option
//
static auto bPrintFileLengthFlag = true;
////////////////////////////////////////////////////////////////////////////////////////////////////

//
// get the abs path for the current running process :
// e.g.    "/usr/bin/ls"    =>   "/usr/bin/"  ( result path contains the last seperate char   '/' )
//
string getBinaryPath()
{
    string retPath;

#ifndef _WIN32 
    //  Unix/Linux Implementation Here
    const size_t C_PATH_LEN = 1024;
    char bufPath[C_PATH_LEN] = { 0 };
    // get the abs path for the running bin   e.g.   /usr/bin/ll
    size_t nn = readlink("/proc/self/exe", bufPath, C_PATH_LEN);
    if ( nn < C_PATH_LEN ) {
        bufPath[nn] = '\0';
    }

    string runningFullPath(bufPath);
    auto pos = runningFullPath.rfind("/");
    string runningPath(runningFullPath);
    if ( pos != string::npos ) {
        runningPath = runningFullPath.substr(0, pos+1);
    }
    retPath = runningPath;
#else
    //  Windows  Implementation Here
#endif
    return retPath;

}


int main(int argc, char* argv[], char* env[])
{
    if ( argc < 2 ) {
        cout << CmdOptions::getUserManual() << endl;
        return -1;
    }


    // else    :   argc >= 2
    if ( argc > 2 ) {
        //
        // Parser cmd-line-argument
        //
        string errorMsg;
        vector<string> arglist;
        for( int idx = 1; idx < (argc-1); ++idx ) {
            arglist.push_back( string(argv[idx]) );
        }

        auto bOptionParserFlag = CmdOptions::analyzeOption(arglist , errorMsg);
        if ( !bOptionParserFlag ) {
            cout << errorMsg << endl
                 << CmdOptions::getUserManual() << endl;
            return -1;
        }
    }


    //
    // Read File from the last args
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

