#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// outFile.write( content, static_cast<streamsize>(1) );

static const char G_R = '\r';
static const char G_N = '\n';


void printUsage()
{
	cout << R"(Usage :
    $ deleteLastEOL   <--del>  <filename>
or
    $ deleteLastEOL   <--add>  [--win|--unix|--mac] <filename>
)" << endl;
}


void delEOL(const string& filename, char* content, unsigned long long fileLength)
{
	if ( fileLength == 0ULL ) {
		cout << "[INFO] File is Empty , No delete eol operatation to do. " << endl;
		return;
	}

	ofstream outputfile( filename.c_str(), ios::out |  ios::trunc | ios::binary);
	if ( fileLength == 1ULL ) {
		if ( content[0] == G_R || content[0] == G_N ) {  // \r
			cout << "[INFO] delete only 1 ch : " << (content[0] == G_R ? "\\r" : "\\n") << endl;
			// write nothing   ==>    deleting (\r or \n)
		} else {
			// recover to origin content
			cout << "[INFO] Keep content as origin state " << endl;
			outputfile.write( content, static_cast<streamsize>(fileLength) );
		}	
	} else {
		// fileLength >= 2
		unsigned long long lastIdx = fileLength - 1;
		char lastCh = content[lastIdx];
		if ( lastCh == G_R ) {
			// to delete \r
			cout << "[INFO] delete last ch : \\r" << endl;
			outputfile.write( content, static_cast<streamsize>(fileLength-1ULL) );
		} else if ( lastCh == G_N ) {
			unsigned long long nHowManyBytesToWrite = fileLength;
			if ( content[lastIdx-1] == G_R ) {
				// last 2 is \r\n
				cout << "[INFO] delete last 2 ch(s) : \\r\\n" << endl;
				nHowManyBytesToWrite -= 2;
			} else {
				// last 2 is ?\n
				cout << "[INFO] delete last 1 ch : \\n" << endl;
				nHowManyBytesToWrite -= 1;
			}

			outputfile.write( content, static_cast<streamsize>(nHowManyBytesToWrite) );
		} else {
			// last character sequence is none of the following 3 kinds   \r   or   \n    or  \r\n
			// recover to origin content
			cout << "[INFO] Keep content as origin state " << endl;
			outputfile.write( content, static_cast<streamsize>(fileLength) );
		}
	}

	outputfile.flush();
	outputfile.close();
}


void addEOL(const string& filename, char* content, unsigned long long fileLength, int eAddWhichType)
{
	// int addEOLType = -1;  eAddWhichType:   1:win  2:unix   3:mac
	static const char defaultInsertCh = G_N;
	if ( fileLength == 0ULL ) {
		cout << "[INFO] File is Empty , No append operatation to do. " << endl;
		return;
	}


	ofstream outputfile( filename.c_str(), ios::out |  ios::trunc | ios::binary);
	if ( fileLength == 1ULL ) {
		if ( content[0] == G_R || content[0] == G_N ) {
			// recover to origin content
			cout << "[INFO] origin content is end with " << (content[0] == G_R ? "\\r" : "\\n") << ". No need to append EOL." << endl;
			outputfile.write( content, static_cast<streamsize>(fileLength) );
		} else {
			// to append
			outputfile.write( content, static_cast<streamsize>(fileLength) );
			if ( eAddWhichType == -1 ) { // use default => unix
				cout << "[INFO] Append default '\\n'" << endl;
				outputfile.write( &defaultInsertCh,    static_cast<streamsize>(1ULL) );
			} else if ( eAddWhichType == 1 ) { // win
				cout << "[INFO] Append specfied \\r\\n by the given argument. " << endl;
				outputfile.write( &G_R,    static_cast<streamsize>(1ULL) );
				outputfile.write( &G_N,    static_cast<streamsize>(1ULL) );
			} else if ( eAddWhichType == 2 ) { // unix
				cout << "[INFO] Append specfied \\n by the given argument. " << endl;
				outputfile.write( &G_N,    static_cast<streamsize>(1ULL) );
			} else if ( eAddWhichType == 3 ) { // mac
				cout << "[INFO] Append specfied \\r by the given argument. " << endl;
				outputfile.write( &G_R,    static_cast<streamsize>(1ULL) );
			}
		}
	} else {
		// fileLength >= 2
		char lastCh = content[fileLength-1];
		// char lastButOneCh = content[fileLength-2];

		if ( !(lastCh == G_R || lastCh == G_N) ) {
			int nWinEOL = 0;
			int nUnixEOL = 0;
			int nMacEOL = 0;
			unsigned long long idx = 0ULL;
			while ( idx < fileLength )
			{
				char ch = content[idx];
				if ( ch == G_R ) {
					auto nextIdx = idx + 1;
					if ( nextIdx < fileLength ) {
						if ( content[nextIdx] == G_N ) { // \r\n
							++nWinEOL;
							++idx; // skip \n
						} else {
							++nMacEOL;
						}
					} else {
						// last ch is  \r
						++nMacEOL;
					}
				} else if ( ch == G_N ) {
					++nUnixEOL;
				}

				++idx;
			}


			if (  nMacEOL  == 0  &&  nWinEOL == 0 && nUnixEOL == 0  ) {
				// Original is Unknow EOL Type
				cout << "[WARNING] : Unknow End Of Line of file : \"" << filename << "\"" << endl;
				outputfile.write( content, static_cast<streamsize>(fileLength) );
				if ( eAddWhichType == -1 ) { // use default => unix
					cout << "[INFO] : Append default eol : \\n" << endl;
					outputfile.write( &defaultInsertCh, static_cast<streamsize>(1ULL) );
				} else if ( eAddWhichType == 1 ) { // win
					cout << "[INFO] : Append Windows eol by the given argument : \\r\\n" << endl;
					outputfile.write( &G_R, static_cast<streamsize>(1ULL) );
					outputfile.write( &G_N, static_cast<streamsize>(1ULL) );
				} else if ( eAddWhichType == 2 ) { // unix
					cout << "[INFO] : Append Unix eol by the given argument : \\n" << endl;
					outputfile.write( &G_N, static_cast<streamsize>(1ULL) );
				} else if ( eAddWhichType == 3 ) { // mac
					cout << "[INFO] : Append Mac eol by the given argument : \\r" << endl;
					outputfile.write( &G_R, static_cast<streamsize>(1ULL) );
				}
			} else if ( nWinEOL  > 0  &&  nUnixEOL == 0  && nMacEOL == 0 ) {
				// Win
				cout << "[INFO] : Deteched WinFormat EOL , Add Windows eol : \\r\\n" << endl;
				outputfile.write( content, static_cast<streamsize>(fileLength) );
				outputfile.write( &G_R, static_cast<streamsize>(1ULL) );
				outputfile.write( &G_N, static_cast<streamsize>(1ULL) );
			} else if ( nUnixEOL > 0  &&  nWinEOL == 0   && nMacEOL == 0 ) {
				// Unix
				cout << "[INFO] : Deteched Unix EOL , Add Unix eol : \\n" << endl;
				outputfile.write( content, static_cast<streamsize>(fileLength) );
				outputfile.write( &G_N, static_cast<streamsize>(1ULL) );
			} else if ( nMacEOL  > 0  &&  nWinEOL == 0   && nUnixEOL == 0 ) {
				// Mac
				cout << "[INFO] : Deteched Mac EOL , Add Mac eol : \\r" << endl;
				outputfile.write( content, static_cast<streamsize>(fileLength) );
				outputfile.write( &G_R, static_cast<streamsize>(1ULL) );
			} else {
				// HyBrid
				cout << "[WARNING] : HyBrid End Of Line of file : \"" << filename << "\"" << endl
				     << "\t nWinEOL = "  << nWinEOL << endl
				     << "\t nUnixEOL = " << nUnixEOL << endl
				     << "\t nMacEOL = "  << nMacEOL << endl;
					 
				outputfile.write( content, static_cast<streamsize>(fileLength) );
				if ( eAddWhichType == -1 ) { // use default => unix
					cout << "[INFO] : Append default eol : \\n" << endl;
					outputfile.write( &defaultInsertCh, static_cast<streamsize>(1ULL) );
				} else if ( eAddWhichType == 1 ) { // win
					cout << "[INFO] : Append Windows eol : \\r\\n" << endl;
					outputfile.write( &G_R, static_cast<streamsize>(1ULL) );
					outputfile.write( &G_N, static_cast<streamsize>(1ULL) );
				} else if ( eAddWhichType == 2 ) { // unix
					cout << "[INFO] : Append Unix eol : \\n" << endl;
					outputfile.write( &G_N, static_cast<streamsize>(1ULL) );
				} else if ( eAddWhichType == 3 ) { // mac
					cout << "[INFO] : Append Mac eol : \\r" << endl;
					outputfile.write( &G_R, static_cast<streamsize>(1ULL) );
				}
			} 
		} else {
			// recover to origin content , do not add
			cout << "[INFO] origin content is end with " << (lastCh == G_R ? "\\r" : "\\n") << ". No need to append EOL." << endl;
			outputfile.write( content, static_cast<streamsize>(fileLength) );
		}
	}

	outputfile.flush();
	outputfile.close();

}


int main(int argc, char* argv[], char* env[])
{
	if ( argc < 2 ) {
		cout << "[ERROR] : You must give an existed file name to process " << endl;
		printUsage();
		return -1;
	} 


	// argc >= 3
	auto isDelete = true;
	string arg1( argv[1] );
	if ( arg1 == "--del" ) {
		isDelete = true;
	} else if ( arg1 == "--add" ) {
		isDelete = false;
	} else {
		cerr << "Argument is known : \"" << arg1  << "\"" << endl;
		printUsage();
		return -1;
	}

	string filename;
	int addEOLType = -1;
	if ( isDelete ) {
		filename = argv[2];
	} else {
		// --add
		string arg2( argv[2] );
		if ( arg2 == "--win" ) {
			addEOLType = 1;
		} else if ( arg2 == "--unix" ) {
			addEOLType = 2;
		} else if ( arg2 == "--mac"  ) {
			addEOLType = 3;
		} 

		if ( addEOLType >= 1 && addEOLType <= 3 ) {
			if ( argc < 4 ) {
				cerr << "Missing a filename to specified. " << endl;
				printUsage();
				return -1;
			} else {
				filename = argv[3];
			}
		} else {
			filename = argv[2];
		}
	}

	

	// read file content
	ifstream inputFile(filename.c_str() , ios::in | ios::binary);
	if ( !inputFile ) {
		cout << "[ERROR] : The given file named \""  << filename << "\" is Not existed !! Program exit ! " << endl;
		return -1;
	}

	inputFile.seekg(0, ios::end);
	unsigned long long flength = static_cast<unsigned long long>( inputFile.tellg() );
	inputFile.seekg(0, ios::beg); // rewind the file

	char *content = nullptr;
	if ( flength == 0ull ) {
		cout << "[INFO] : File is empty ! " << endl;
	} else {
		content = new char[flength];
		inputFile.read(content, static_cast<streamsize>(flength) );
	}
	inputFile.close();


	if ( isDelete ) {
		delEOL( filename, content, flength);
	} else {
		addEOL( filename, content, flength, addEOLType);
	}

	// release alloced memory
	if ( flength > 0ull ) {
		delete [] content;
		content = nullptr;
	}


	return 0;

}

