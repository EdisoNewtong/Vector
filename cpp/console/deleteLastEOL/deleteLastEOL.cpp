#include <iostream>
#include <fstream>
#include <string>
using namespace std;



int main(int argc, char* argv[], char* env[])
{
	if ( argc != 2 ) {
		cout << "[ERROR] : You must give an existed file name to process " << endl;
		return -1;
	}

	ifstream inputFile(argv[1], ios::in | ios::binary);
	if ( !inputFile ) {
		cout << "[ERROR] : The given file named \""  << argv[1] << " is not existed. Program exit ! " << endl;
		return -1;
	}

	inputFile.seekg(0, ios::end);
	unsigned long long flength = static_cast<unsigned long long>( inputFile.tellg() );
	inputFile.seekg(0, ios::beg);

	if ( flength == 0 ) {
		cout << "[DONE] : File Content is Empty , Do Nothing , Quit! " << endl;
		return 0;
	}

	char *content = new char[flength];
	inputFile.read(content, static_cast<streamsize>(flength) );
	inputFile.close();

	auto bSuccess = false;
	auto isReallyDeleteEOL = false;
	string processMsg; 
	processMsg += ("Can't open file \"" + string(argv[1]) + "\" for writing.");
	string deleteChars;

	if ( flength == 1ULL ) {
		//
		// 1 char
		//
		if ( content[0] == '\r' || content[0] == '\n' ) {
			// delete the last only character   from the existed file
			ofstream outFile( argv[1], ios::out | ios::trunc | ios::binary);
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
			ofstream outFile( argv[1], ios::out | ios::trunc | ios::binary);
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
				needReWrite = (content[0] != '\r');
			} else {
				// \n
				needReWrite = (content[0] != '\n');
			}

			if ( needReWrite ) {
				ofstream outFile( argv[1], ios::out | ios::trunc | ios::binary);
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
			needReWrite = (content[lastButTwoIdx] != '\n');
			minusChar   = 2;
			deleteChars = "\\r\\n";
		} else if ( content[lastCharIdx] == '\r' ) {
			needReWrite = (content[lastButOneIdx] != '\r');
			minusChar   = 1;
			deleteChars = "\\r";
		} else if ( content[lastCharIdx] == '\n' ) {
			needReWrite = (content[lastButOneIdx] != '\n');
			minusChar   = 1;
			deleteChars = "\\n";
		}

		if ( needReWrite ) {
			ofstream outFile( argv[1], ios::out | ios::trunc | ios::binary);
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
			cout << "[DONE] : Delete Last " << deleteCnt << (deleteCnt == 1 ? " byte" : " bytes") << endl;
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
