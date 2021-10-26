#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <new>
#include <exception>

#include "parser.h"
#include "parserException.h"

using namespace std;


static auto bPrintFileLengthFlag = true;


int main(int argc, char* argv[], char* env[])
{
	if ( argc < 2 ) {
		cout << "[ERROR] : Missing file name to parse !!" << endl;
		return -1;
	}

	//
	// check file is existed or not
	//
	string fname(argv[1]);
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
		cout << "==================================================" << endl;
	}

	if ( filelen == 0 ) {
		cout << "[DONE] : Nothing to Do , File has 0 byte " << endl;
		file.close();
		return 0;
	}


	Parser p;
	char* filebuf = nullptr;
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
	} catch( const  std::bad_array_new_length& e_alloc ) {
		(void)e_alloc;
		cout << "Can't Alloc Memory for file content" << endl;
	} catch ( const ParserExpection& e ) {
		cout << "[ERROR] Parse Failed : " << e.what() << endl;
	} catch ( const std::exception& e ) {
		cout << "[ERROR] Meet an exception : " << e.what() << endl;
	} catch ( ... ) {
		cout << "[ERROR] Meet unexpected expection, ... " << endl;
	}

	//
	// Release Memory
	//
	file.close();

	delete [] filebuf;
    filebuf = nullptr;


	return 0;
}

