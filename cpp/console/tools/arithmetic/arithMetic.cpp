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
#include "functionMgr.h"
#include "expEvaluation.h"

using namespace std;



constexpr static const int G_MAJOR = 1;
constexpr static const int G_MINOR = 4;
constexpr static const int G_PATCH = 12;



std::string getProgram_Version()
{
    const std::string S_FEATURE_DESCRIPTION("( Support Function-Call )");

    return    string("  Arith V ")
            + to_string(G_MAJOR) + string(".") 
            + to_string(G_MINOR) + string(".")
            + to_string(G_PATCH)
            + string(" is Running.  ") + S_FEATURE_DESCRIPTION;
}


//
// get the abs path for the current running process :
// e.g.    "/usr/bin/ls"    =>   "/usr/bin/"  ( result path contains the last seperate char   '/' )
//
string getBinaryPath(const string& strProgName, char* env[])
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
    // cout << "nsize = " << std::dec << nsize << endl;
    // cout << "bufPath = " << std::dec << bufPath << endl;
    if ( bufPath[0] != 0 ) {
        if ( nsize < C_PATH_LEN ) {
            bufPath[nsize] = '\0';
        }
    } else {
        // in MacOS Platform , the prog : '/proc/self/exe' is not existed , so use char* env[] to get the prog running path
        int foundIdx = -1;
        int iidx = 0;
        const string searchHead("_=");
        string pathprefix;
        while ( env[iidx] ) {
            string sOpt(env[iidx]);
            auto foundpos = sOpt.find(searchHead);
            if ( foundpos == 0 ) {
                foundIdx = iidx;
                pathprefix = sOpt.substr( searchHead.size() );
                break;
            }
            ++iidx;
        }

        auto sz = 0;
        if ( foundIdx != -1 ) {
            sz = static_cast<int>(pathprefix.size());
            for( int idx = 0; idx < sz; ++idx ) 
            {
                bufPath[idx] = pathprefix.at(idx);
            }
            bufPath[sz] = '\0';
        } else { // foundIdx == -1
            auto fSlashPos = strProgName.find("/");
            if ( fSlashPos == string::npos ) {
                bufPath[0] = '.';
                bufPath[1] = '/';
                bufPath[2] = '\0';
            } else {
                sz = static_cast<int>(strProgName.size());
                for( int idx = 0; idx < sz; ++idx ) 
                {
                    bufPath[idx] = strProgName.at(idx);
                }
                bufPath[sz] = '\0';
            }
        }
    }

#else
    (void)strProgName;
    (void)env;
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
    string runningPath = getBinaryPath(string(argv[0]), env);
    cout << getProgram_Version() << endl;

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

    CmdOptions::printDataTypeRangeIfNecessary();


    //
    // Read File from the last args
    //
    string fname(argv[argc-1]);
    int fnamesz = static_cast<int>( fname.size() );
    bool bIsFile = false;
    // cout << "fname = |\"" << fname << "\"|" << endl;
    if ( fnamesz >=1   &&  (fname.find(';') != string::npos) ) {
        bIsFile = false;
    } else {
        bIsFile = true;
    }
    // cout << "bIsFile = " << (bIsFile ? "true" : "false") << endl;


    ifstream file;
    auto filelen = 0;
    if ( bIsFile ) {
        file.open(fname.c_str(), ios::in | ios::binary);
        if ( !file.is_open() ) {
            cout << "[ERROR] : File < " << fname << "> not found " << endl;
            return -1;
        }

        file.seekg(0, ios::end); // file-pointer move to the end of file
        filelen = static_cast<int>( file.tellg() );
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
    } else {
        file.close();
        filelen = fnamesz;
    }

    DataTypeUtil::init();
    VariblePool::init();
    TokenMgr::init();
    FunctionMgr::init();
    ExpEvaluation::init();

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
        if ( bIsFile ) {
            file.read(filebuf, filelen);
            file.close();
        } else {
            // copy the content
            for( int idx = 0; idx < filelen; ++idx ) {
                filebuf[idx] = fname.at(idx);
            }
        }


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

    ExpEvaluation::release();
    FunctionMgr::finalize();
    TokenMgr::release();
    VariblePool::finalize();
    DataTypeUtil::finalize();

    return 0;
}

