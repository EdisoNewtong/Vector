#include <iostream>
#include <fstream>
#include <string>
using namespace std;


void printUsage()
{
	cout << R"(Usage :
	$ deleteLastEOL [-f] <filename>
)" << endl;
}

int main(int argc, char* argv[], char* env[])
{
	if ( argc < 2 ) {
		cout << "[ERROR] : You must give an existed file name to process " << endl;
		printUsage();
		return -1;
	} else if ( argc >= 4 ) {
		cout << "[ERROR] : Too many arguments " << endl;
		printUsage();
		return -1;
	}

	auto forceDeleteOption = false;
	string filename;
	if ( argc == 2 ) {
		filename = argv[1];
	} else if ( argc == 3 ) {
		string option(argv[1]);
		if ( option != "-f" ) {
			cout << "[ERROR] : Unknow option " << option << endl;
			printUsage();
			return -1;
		} else {
			forceDeleteOption = true;
			cout << "[INFO] : Use Force Delete Mode " << endl;
		}
		filename = argv[2];
	}


	


	ifstream inputFile(filename.c_str() , ios::in | ios::binary);
	if ( !inputFile ) {
		cout << "[ERROR] : The given file named \""  << filename << "\" is Not existed !! Program exit ! " << endl;
		return -1;
	}

	inputFile.seekg(0, ios::end);
	unsigned long long flength = static_cast<unsigned long long>( inputFile.tellg() );
	inputFile.seekg(0, ios::beg);

	if ( flength == 0 ) {
		cout << "[DONE] : File Content cantain 0 byte , Do Nothing !!! Quit! " << endl;
		return 0;
	}

	char *content = new char[flength];
	inputFile.read(content, static_cast<streamsize>(flength) );
	inputFile.close();

	auto bSuccess = false;
	auto isReallyDeleteEOL = false;
	string processMsg; 
	processMsg += ("Can't open file \"" + filename + "\" for writing.");
	string deleteChars;

	if ( flength == 1ULL ) {
		//
		// 1 char
		//
		if ( content[0] == '\r' || content[0] == '\n' ) {
			// delete the last only character   from the existed file
			ofstream outFile( filename.c_str(), ios::out | ios::trunc | ios::binary);
			if ( outFile ) {
				// write nothing into the file , leave an empty file
				outFile.flush();
				outFile.close();
				deleteChars = ((content[0] == '\r') ? "\\r" : "\\n");

				isReallyDeleteEOL = true;
				bSuccess = true;
			}
		} else {
			bSuccess = true;
		}
	} else if ( flength == 2ULL ) {
		//
		// 2 chars
		//
		if ( content[0] == '\r' && content[1] == '\n' ) {
			// windows version
			ofstream outFile( filename.c_str(), ios::out | ios::trunc | ios::binary);
			if ( outFile ) {
				// write nothing into the file
				outFile.close();
				deleteChars = "\\r\\n";

				bSuccess = true;
				isReallyDeleteEOL = true;
			} 
		} else if ( content[1] == '\r' || content[1] == '\n' ) {
			auto needReWrite = false;
			if ( content[1] == '\r' ) {
				needReWrite = forceDeleteOption || (content[0] != '\r');
			} else {
				// \n
				needReWrite = forceDeleteOption || (content[0] != '\n');
			}

			if ( needReWrite ) {
				ofstream outFile( filename.c_str(), ios::out | ios::trunc | ios::binary);
				if ( outFile ) {
					// detele the last byte 
					outFile.write( content, static_cast<streamsize>(1) );
					outFile.flush();
					outFile.close();

					deleteChars = ((content[1] == '\r') ? "\\r" : "\\n");
					bSuccess = true;
					isReallyDeleteEOL = true;
				} 
			} else {
				bSuccess = true;
			}
		} else {
			bSuccess = true;
		}
	} else {
		// >= 3ULL
		auto needReWrite = false;
		unsigned long long lastCharIdx   = flength - 1; 
		unsigned long long lastButOneIdx = flength - 2; 
		unsigned long long lastButTwoIdx = flength - 3;
		unsigned long long minusChar = 0;
		if( content[lastButOneIdx] == '\r' && content[lastCharIdx] == '\n' ) {
			needReWrite = forceDeleteOption || (content[lastButTwoIdx] != '\n');
			minusChar   = 2;
			deleteChars = "\\r\\n";
		} else if ( content[lastCharIdx] == '\r' ) {
			needReWrite = forceDeleteOption || (content[lastButOneIdx] != '\r');
			minusChar   = 1;
			deleteChars = "\\r";
		} else if ( content[lastCharIdx] == '\n' ) {
			needReWrite = forceDeleteOption || (content[lastButOneIdx] != '\n');
			minusChar   = 1;
			deleteChars = "\\n";
		}

		if ( needReWrite ) {
			ofstream outFile( filename.c_str(), ios::out | ios::trunc | ios::binary);
			if ( outFile ) {
				// detele the last byte 
				outFile.write( content, static_cast<streamsize>(flength - minusChar) );
				outFile.flush();
				outFile.close();

				isReallyDeleteEOL = true;
				bSuccess = true;
			}
		} else {
			bSuccess = true;
		}
	}

	if ( bSuccess ) {
		if ( isReallyDeleteEOL ) {
			int deleteCnt = static_cast<int>((deleteChars.size() / 2));
			cout << "[Over-Write] : Delete Last " << deleteCnt << (deleteCnt == 1 ? " byte" : " bytes") << " => " << deleteChars << " ==========> : \"" << filename << "\"" << endl;
		} else {
			cout << "[DONE] : No Need to Process" << endl;
		}
	} else {
		cout << "[ERROR] : " << processMsg << endl;
	}

	// release alloced memory
	delete [] content;
	content = nullptr;
	return 0;

}
