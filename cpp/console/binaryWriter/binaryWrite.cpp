#include <iostream>
#include <fstream>
#include <string>
using namespace std;


void printUsage()
{
	auto msg = R"abc(usage : 
--------------------------------------------------
       input \r    as    Carriage-Return (CR)
       input \n    as    Line-Feed       (LF)
       input \r\n  as    Windows End of Line
       input \t    as    Tab
       input 0x??  as    Hex-Char , ?? must be one of the following group : [0-9a-FA-F]
       input @@END@@   to save the file and quit
-------------------------------------------------)abc";

	cout << msg << endl;
}


int main(int argc, char* argv[], char* env[])
{
	static const string ENDSTR = "@@END@@";

	static const string SN  = "\\n";
	static const string SR  = "\\r"; 
	static const string ST  = "\\t"; 
	static const string SRN = "\\r\\n"; 
	static const string HEX_PREFIX = "0x";
	// static const string SZERO = "\\0"; 

	const char   CN = '\n';
	const char   CR = '\r';
	const char   CT = '\t';
	// const char   CZERO = '\0';
	const char   CEOF = '#';
	char chTmp = 0; 
	// (void)chTmp;

	printUsage();

	string fname;
	cout << "Please input a binary file name to edit : ";
	cin  >> fname;

	while( true ) {
		string choice;
		ifstream f(fname.c_str(), ios::in);
		if ( f ) {
			cout << "[WARNING] : " << fname << " is already existed, override it ? (y/n)";
			cin >> choice;
			if ( choice == "Y" || choice == "y" ) {
				break;
			} else {
				bool isvalid = ( choice == "N" || choice == "n" );
				if ( !isvalid ) {
					cout << "[WARNING] : invalid input (not y/n) , treat as  No " << endl;
				}

				cout << "Please input a New file name : ";
				cin >> fname;
			}
		} else {
			break;
		}
	}

	cout << "\t[INFO] , Now begin write the content into File : <" << fname << ">" << endl << endl;

	// must add this code
	cin.read(&chTmp, sizeof(chTmp) );

	ofstream fout(fname.c_str() , ios::out | ios::trunc | ios::binary);
	while ( true ) {
		string someBuf;
		cout << "Please input some buf , for Quit to press  " << ENDSTR << " : ";
		// getline(cin, someBuf, CEOF);
		(void)CEOF;
		getline(cin, someBuf);
		if ( someBuf == SN ) {
			// cout << "Meet  \\n" << endl;
			fout.write(&CN, sizeof(CN) );
		} else if ( someBuf == SR ) {
			// cout << "Meet  \\r" << endl;
			fout.write(&CR, sizeof(CR) );
		} else if ( someBuf == ST ) {
			// cout << "Meet  \\t" << endl;
			fout.write(&CT, sizeof(CT) );
		} else if ( someBuf == SRN ) {
			// cout << "Meet  \r\n" << endl;
			fout.write(&CR, sizeof(CR) );
			fout.write(&CN, sizeof(CN) );
		} else if ( someBuf.find(HEX_PREFIX) == 0 ) {
			auto sz = someBuf.size();
			if ( sz == 3 || sz == 4 ) {
				auto atMost2Chars = someBuf.substr(2);
				// int       stoi( const std::string& str, std::size_t* pos = 0, int base = 10 );
				try {
					int code = stoi(atMost2Chars, 0, 16);
					char i_ch = static_cast<char>(0xFF & code);
					// write 1 hex base byte
					fout.write( &i_ch, sizeof(i_ch) );
				} catch (...) {
					cout << "[ERROR] : can't cast  \"" << atMost2Chars << "\" to hex Char "<< endl;
				}
			} else {
				cout << "[ERROR] : the hex code must be like 0x? or 0x??" << endl;
				continue;
			}
		} else {
			// cout << "someBuf = \"" << someBuf << "\"" << endl;
			auto bNeedQuit = (someBuf == ENDSTR );
			if ( !bNeedQuit ) {
				fout.write( someBuf.c_str(), someBuf.size() );
			} else {
				// meet quit
				cout << "Save and Quit" << endl;
				break;
			}
		}

		// cin.read(&chTmp, sizeof(chTmp) );
		fout.flush();
	}

	fout.flush();
	fout.close();
	cout << endl;
	cout << "==================================================" << endl;
	cout << "[INFO] : Write file " << fname << " Finish , Bye! " << endl;
	cout << "==================================================" << endl;

	return 0;
}



