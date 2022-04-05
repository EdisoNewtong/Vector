#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <list>
#include <new>
#include <exception>

#include "parser.h"
#include "parserOptions.h"
#include "myException.h"
#include "charUtil.h"
#include "typeUtil.h"
#include "tokenParserMgr.h"

#include "variblePool.h"

using namespace std;


static auto bPrintFileLengthFlag = true;


int main(int argc, char* argv[], char* env[])
{
	TypeUtil::init();
	CharUtil::init();
	TokenParserMgr::init();
    VariblePool::init();

	Parser p;

	if ( argc < 3 ) {
		cout << "[ERROR] : arglist.count < 3" << endl;
		cout << ParserOptions::getUserManual() << endl;
		cout << endl;
		return -1;
	}
    // else   : argc > 3 , argc >=4


	//
	// check file is existed or not
	//
	string fname(argv[argc-1]);
	ifstream file(fname.c_str(), ios::in | ios::binary);
	if ( !file ) {
		cout << "[ERROR] : File < " << fname << "> not found " << endl;
		return -1;
	}

	//
	// get file-length
	//
	file.seekg(0, ios::end); // file-pointer move to the end of file
	size_t filelen = static_cast<size_t>( file.tellg() );
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


	list<string> strArgList;
	for( int i = 1; i <= (argc-2); ++i ) {
		strArgList.push_back( string(argv[i]) );
	}
	

	string errorMsg;
	char* filebuf = nullptr;
	auto anaRet = ParserOptions::analyzeOptionArgs(strArgList, errorMsg);

	if ( anaRet ) {
		try {
			// read all
			filebuf = new char[filelen];
			file.read(filebuf, filelen);

			p.setContent(filebuf, filelen );
			//
			// Core Core Core / Operation
			//
			auto iret = p.doParse();

			if ( iret ) {
			} else {
			}
		} 
		/*
		catch( const  std::bad_array_new_length& e_alloc ) {
			(void)e_alloc;
			cout << "Can't Alloc Array Buffer Memory for file content" << endl;
		} 
		*/
		catch ( const MyException& e ) {
			cout << "[ERROR] Run-time exception : " << e.what() << endl;
		} catch ( const std::exception& e ) {
			cout << "[ERROR] Meet a Normal exception => " << e.what() << endl;
		} catch ( ... ) {
			cout << "[ERROR] Meet unexpected expection, ... " << endl;
		}
	} else {
		cout << "[ERROR] : Invalid arguments => \" " << errorMsg << "\" " << endl;
		cout << ParserOptions::getUserManual() << endl << endl;
	}

	//
	// Release Memory
	//
	file.close();

	if ( filebuf != nullptr ) {
		delete [] filebuf;
		filebuf = nullptr;
	}

	CharUtil::finalize();
	TypeUtil::finalize();
	TokenParserMgr::finalize();
    VariblePool::finalize();

	return 0;
}

