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


//
// get the abs path for the current running process :
// e.g.    "/usr/bin/ls"    =>   "/usr/bin/"  ( result path contains the last seperate char   '/' )
//
string getBinaryPath()
{
    string retPath;
    string platformSeperate;

#ifndef _WIN32 
    //  Unix/Linux Implementation Here
    platformSeperate = "/";

    const size_t C_PATH_LEN = 1024;
    char bufPath[C_PATH_LEN] = { 0 };
    // get the abs path for the running bin   e.g.   /usr/bin/ll
    size_t nsize = readlink("/proc/self/exe", bufPath, C_PATH_LEN);
    if ( nn < C_PATH_LEN ) {
        bufPath[nsize] = '\0';
    }

#else
    //  Windows  Implementation Here
    platformSeperate = "\\";

    constexpr size_t C_PATH_LEN = MAX_PATH + 1;
    HMODULE handleModule = nullptr;
    char bufPath[C_PATH_LEN] = { 0 };
    size_t fileSz = static_cast<size_t>( ::GetModuleFileNameA(handleModule, bufPath, MAX_PATH) );
    if ( fileSz < C_PATH_LEN ) {
        bufPath[fileSz] = '\0';
    }

#endif


    string runningFullPath(bufPath);
    auto pos = runningFullPath.rfind(platformSeperate);
    string runningPath(runningFullPath);
    if ( pos != string::npos ) {
        runningPath = runningFullPath.substr(0, pos+1);
    }

    retPath = runningPath;
    return retPath;
}


int main(int argc, char* argv[], char* env[])
{
    string runningPath = getBinaryPath();

    if ( argc < 2 ) {
        cout << "[ERROR] : Missing a given file to parse !!" << endl;
        cout << CmdOptions::getUserManual() << endl;
        return -1;
    }


    auto successParseArgs = false;
    string cfgfile;
    // else    :   argc >= 2
    if ( argc > 2 ) {
        // Prepare cmd-line-argument
        vector<string> arglist;
        for( int idx = 1; idx < (argc-1); ++idx ) {
            arglist.push_back( string(argv[idx]) );
        }

        auto pr = CmdOptions::parseCmdArgs(arglist);
        if ( !pr.first ) {
            cout << pr.second << endl
                 << CmdOptions::getUserManual() << endl;
            return -1;
        } else {
            // success parsing args
            cfgfile = pr.second;
            successParseArgs = true;
        }
    }
    // args == 2      ./prog   <srcCode>
    // CmdOptions::

    auto pr = CmdOptions::parseCfgFile(successParseArgs, cfgfile, runningPath);
    if ( !pr.first ) {
        cout << pr.second << endl
             << CmdOptions::getUserManual() << endl;
        cout << "== Use Default Options ===" << endl;
        if ( !successParseArgs ) {
            cout << "You can create a config file named : " << CmdOptions::getDefaultCfgFileName() << " to set program's runtime features. " << endl;
            cout << "Like the following format : " << endl;
            cout << "==================================================" << endl;
            cout << CmdOptions::sampleCfgFile() << endl;
            cout << "==================================================" << endl;
        }
    } else {
        cout << "----------------------------------------------------------------------------------------------------" << endl;
        if ( !successParseArgs ) {
            cout << "[INFO] : Success Reading the ***[Default]*** config File : " << pr.second << endl; 
        } else {
            cout << "[INFO] : Success Reading specified config file : " << pr.second << endl;
        }
        cout << "----------------------------------------------------------------------------------------------------" << endl;
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

    if ( CmdOptions::needPrintSrcCodeLength()  ) {
        cout << endl;
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

    TokenMgr::release();
    VariblePool::finalize();
    DataTypeUtil::finalize();

    return 0;
}

