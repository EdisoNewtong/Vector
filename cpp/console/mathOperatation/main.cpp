#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <new>
#include <exception>

#include "parser.h"

using namespace std;


auto bPrintFileLengthFlag = false;


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
	std::streamsize filelen = file.tellg();
	file.seekg(0, ios::beg); // file-pointer move to the begin of file
	if ( bPrintFileLengthFlag ) {
		cout << "filelen = " << filelen << endl;
	}

	if ( filelen == 0 ) {
		cout << "[DONE] : File has 0 byte(s) " << endl;
		file.close();
		return 0;
	}


	char* filebuf = nullptr;
	try {
		// read all
		filebuf = new char[filelen];
		file.get(filebuf, filelen);

	    Parser p(filebuf, filelen);
		auto iret = p.doParse();

		if ( iret ) {
		} else {
		}
	} catch( const  std::bad_array_new_length& e_alloc ) {
		(void)e_alloc;
		cout << "Can't Alloc Memory for file content" << endl;
	} catch ( const std::exception& e ) {
		cout << "Meet exception : " << e.what() << endl;
	} catch ( ... ) {
		cout << "Meet unexpected expection, ... " << endl;
	}

	//
	// Release Memory
	//
	delete [] filebuf;
    filebuf = nullptr;

	file.close();

	return 0;
}

