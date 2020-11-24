#include <iostream>
#include <string>
#include <utility>
#include <fstream>
#include <functional>
using namespace std;

//
// usage :
//        $ eofconvert   <filename>   --[dos][unix][mac]   [--override]
//

void printUsage()
{
	string usage = R"(usage : 
              $ eofconvert   <filename>  --[unix|linux|dos|win|mac]
           or $ eofconvert   <filename>  --[unix|linux|dos|win|mac]  --override)";

	cout << "---------------------------------------------------------------------------" << endl;
	cout << usage << endl;
	cout << "---------------------------------------------------------------------------" << endl;
}




pair<char*,unsigned long long> readFile(const string& fname)
{
	ifstream file(fname.c_str(), ios::in | ios::binary);
	if ( !file ) {
		cout << "[ERROR] : can't open file : " << fname <<  endl;
		file.close();
		printUsage();
		return make_pair(nullptr, 0UL);
	}

	char* content = nullptr;
	file.seekg(0, ios::end);
	unsigned long long filesize = file.tellg();
	file.seekg(0, ios::beg);

	if ( filesize > 0UL ) {
		try {
			// alloc memory for file content
			content = new char[filesize];
			file.read(content, static_cast<std::streamsize>(filesize) );
		} catch ( ... ) {
			content = nullptr;
		}
	} else {
		content = nullptr;
	}

	// close the opened file
	file.close();

	return make_pair(content, filesize);
}

string getNewFileName(const string& fname, const string& opt)
{
	auto pos = fname.rfind(".");
	string prefix;
	string suffix;
	if ( pos == string::npos ) {
		prefix = fname;
		suffix = "";
	} else {
		// e.g.   aaa/bbb/test.log   
		prefix = fname.substr(0,pos);  // prefix = "aaa/bbb/test"
		suffix = fname.substr(pos);    // suffix = ".log"
	}


	string finalName;
	auto tryName = prefix + opt + suffix;
	ifstream tmpFile( tryName.c_str(), ios::in);
	if ( !tmpFile ) {
		// This name can be the new file name after converting
		tmpFile.close();
		finalName = tryName;
	} else {
		// existed
		// try add number after it
		int idx = 1;
		while ( true ) {
			string tryNName = prefix + opt + std::to_string(idx) + suffix;
			ifstream tmpNFile( tryNName.c_str(), ios::in);

			if ( !tmpNFile  ) {
				// not existed
				// This name can be the new file name after converting
				tmpNFile.close();
				finalName = tryNName;
				break;
			} else {
				++idx;
			}
		}
	}

	return finalName;
}

void convert(const pair<char*,unsigned long long>& fInfo, const string& originalfname, const string& newfname, int convertFlag )
{
	constexpr size_t writeFuncSize = 3;
	static const char SC_CHAR_R = '\r';
	static const char SC_CHAR_N = '\n';

	ofstream outFile( newfname.c_str() , ios::out | ios::trunc | ios::binary);
	auto originalContent = fInfo.first;
	auto fLength = fInfo.second;

	// -1:undefined   0:unix   ,   1:dos    2:mac    3:multi
	int originalEofType =  -1;
	// for multi-eof-file type used
	int tp1st = -1;
	unsigned long long offset1st = 0UL;
	// int lineNum1 = 0;
	int tp2nd = -1;
	unsigned long long offset2nd = 0UL;
	int lineNum2 = -1;

	function<void()> eofWriteFuncAry[writeFuncSize] =  {
		// Unix
		[&]() {
			outFile.write(&SC_CHAR_N, sizeof(char) );
		},

		// Dos
		[&]() {
			outFile.write(&SC_CHAR_R, sizeof(char) );
			outFile.write(&SC_CHAR_N, sizeof(char) );
		},

		// Mac
		[&]() {
			outFile.write(&SC_CHAR_R, sizeof(char) );
		}
	};

	bool lastEOFFlag = false;
	int  lineCnt = 1;
	for ( unsigned long long i = 0UL; i < fLength; ) {
		char ch = originalContent[i];

		if ( ch == SC_CHAR_N  ) {
			//
			// current is  \n
			//
			if ( originalEofType ==  -1 ) {
				tp1st = 0;
				offset1st = i;
				lastEOFFlag = true;

				originalEofType = 0; // set eof as unix
			} else if ( originalEofType == 1 || originalEofType == 2 ) {
				// dos or mac
				tp2nd = 0;
				offset2nd = i;

				if ( lastEOFFlag ) {
					++lineCnt;
				} 

				if ( lineNum2 == -1 ) {
					lineNum2 = lineCnt;
				}
				lastEOFFlag = true;

				originalEofType = 3; // set as multi
			} else {
				// originalEofType == 3 or originalEofType == 0, the state will not changed
				if ( lastEOFFlag ) {
					++lineCnt;
				}

				lastEOFFlag = true;
			}
			

			++i;
			eofWriteFuncAry[convertFlag]();
		} else if ( ch == SC_CHAR_R ) {
			//
			// current is  \r
			//
			if ( originalEofType == -1 ) {

				if ( i+1 < fLength ) {
					if( originalContent[i+1] == SC_CHAR_N ) {
						// next char is \n , so \r\n
						tp1st = 1;
						offset1st = i;

						originalEofType = 1; // set eof as dos/win
						// move to the character after \n
						i += 2;
					} else {
						// \r?
						tp1st = 2;
						offset1st = i;

						originalEofType = 2; // set eof as mac
						++i;
					}
				} else {
					// i is the last char of the content
					tp1st = 2;
					offset1st = i;

					originalEofType = 2; // set eof as mac
					++i;
				}

				lastEOFFlag = true;
			} else {
				// originalEofType != -1
				if ( i+1 < fLength ) {
					if ( originalContent[i+1] == SC_CHAR_N ) {
						// \r\n :   guess current is win=1
						if ( originalEofType == 0 || originalEofType == 2 ) {
							tp2nd = 1;
							offset2nd = i;

							if ( lastEOFFlag ) {
								++lineCnt;
							} 

							if ( lineNum2 == -1 ) {
								lineNum2 = lineCnt;
							}

							originalEofType = 3; // set as multi
						} else {
							// originalEofType ==4 or originalEofType = 2 , the state of eof will not changed
							if ( lastEOFFlag ) {
								++lineCnt;
							} 
						}
						
						// move to the character after \n
						i += 2;
					} else {
						// \r?  : guess current is mac=2
						if ( originalEofType == 0 || originalEofType == 1 ) {
							tp2nd = 2;
							offset2nd = i;

							if ( lastEOFFlag ) {
								++lineCnt;
							} 

							if ( lineNum2 == -1 ) {
								lineNum2 = lineCnt;
							}

							originalEofType = 3; // set as multi
						} else {
							// originalEofType ==3 or originalEofType = 2 , the state of eof will not changed
							if ( lastEOFFlag ) {
								++lineCnt;
							} 
						}

						++i;
					}
				} else {
					// \r is the last character of the content : guess current is mac=2
					if ( originalEofType == 0 || originalEofType == 1 ) {
						tp2nd = 2;
						offset2nd = i;

						if ( lastEOFFlag ) {
							++lineCnt;
						} 

						if ( lineNum2 == -1 ) {
							lineNum2 = lineCnt;
						}

						originalEofType = 3; // set as multi
					} else {
						// originalEofType ==3 or originalEofType = 2 , the state of eof will not changed
						if ( lastEOFFlag ) {
							++lineCnt;
						} 
					}
					++i;
				}

				lastEOFFlag = true;
			}

			eofWriteFuncAry[convertFlag]();
		} else {
			// other char 
			outFile.write(&ch, sizeof(char) );

			++i;
			if ( lastEOFFlag ) {
				lastEOFFlag = false;
				++lineCnt;
			}
		}
	}

	auto getModeNameFunc = [](int flag) -> string {
		string retMode;
		if ( flag == 0) {
			retMode = "Unix/Linux";
		} else if ( flag == 1) {
			retMode = "Dos/Windows";
		} else if ( flag == 2) {
			retMode = "Mac";
			// retMode = "Macintosh";
		} else {
			retMode = "???";
		}
		return retMode;
	};


	cout << "Summary of   File : " << originalfname << endl;
	cout << "==================================================" << endl;
	if ( originalEofType == -1 ) {
		cout << R"(No \r \n found ), undefined  EOF)" << endl;
	} else if ( originalEofType == 0 || originalEofType == 1 || originalEofType == 2 ) {
		cout << "Originally : the file's EOF mode = " << getModeNameFunc(originalEofType) << endl;
	} else {
		//  originalEofType = 3
		cout << "[WARINIG] : EOF mode is ambiguous !!! multiple EOF was detected " << endl;
		cout << "======================================================================" << endl;
		cout << endl;
		cout << "    1st EOF mode   = " << getModeNameFunc(tp1st) << endl;
		cout << "          offset   = " << offset1st << endl;
		cout << "          line#No. = " << 1 << endl;
		cout << "    2nd EOF mode   = " << getModeNameFunc(tp2nd) << endl;
		cout << "          offset   = " << offset2nd << endl;
		cout << "          line#No. = " << lineNum2 << endl;
		cout << endl;
		cout << "======================================================================" << endl;
	}
	cout << endl;
	cout << "[INFO] : convert to EOF " << getModeNameFunc(convertFlag) << " finished : new file name is  \"" << newfname << "\" " << endl;
	cout << endl;
	cout << "==================================================" << endl;

	outFile.flush();
	outFile.close();
}


int main(int argc, char* argv[], char* env[])
{
	// cout << "argc = " << argc << endl;
	if ( !(argc >= 3 && argc <= 4) ) {
		cout << "[ERROR] : the argc must >= 3 && argc <= 4 " <<  endl;
		printUsage();
		return -1;
	}

	string filename = argv[1];
	auto pr = readFile(filename);
	auto contentOfFile = pr.first;
	if ( pr.second == 0UL ) {
		cout << "[INFO] Nothing can be action on  <Empty File> | Bye" << endl;
		return -1;
	} else if ( contentOfFile == nullptr ) {
		cout << "[ERROR] The content of file is nullptr or memory is not enough | Bye " << endl;
		return -1;
	}


	// parse is    --override
	auto overrideFlag = false;
	if( argc == 4 ) {
		string arg3 = argv[3];
		if ( arg3 == "--override" ) {
			overrideFlag = true;
		} else {
			cout << "[ERROR] : Argument Parse Error arg[3] is not  \"--override\"   " << endl;
			printUsage();
			return -1;
		}
	}


	//
	// Open and Read File , parse the 2nd arg and the 3rd arg 
	auto optError = false;
	string option = argv[2];
	if ( option == "--unix" || option == "--linux"  ) {
		auto newfname = (overrideFlag ? filename : getNewFileName(filename, option))  ;
		convert(pr, filename, newfname, 0);
	} else if ( option == "--dos" || option == "--win" ) {
		auto newfname = (overrideFlag ? filename : getNewFileName(filename, option));
		convert(pr, filename, newfname, 1);
	} else if ( option == "--mac" ) {
		auto newfname = (overrideFlag ? filename : getNewFileName(filename, option));
		convert(pr, filename, newfname, 2);
	} else {
		cout << "[ERROR] : invalid convert options : " << argv[2] <<  endl;
		cout << "          The option must be one of following choices" << endl;
		cout << "        1. --dos" << endl;
		cout << "        2. --unix" << endl;
		cout << "        3. --mac" << endl;
		cout << "        4. --win" << endl;
		cout << "        5. --linux" << endl;
		printUsage();

		optError = true;
	}

	if( contentOfFile != nullptr ) {
		delete [] contentOfFile;
	}


	if ( optError ) {
		return -1;
	}

	return 0;
}

