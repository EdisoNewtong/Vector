#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <exception>
using namespace std;

/*

$ spacetabconvert  --2space <1-tab   -> n-space>      --head|tail|headtail  [--force]  <filename>
$ spacetabconvert  --2tab   <n-space -> 1-tab>    --head|tail|headtail  [--force]  <filename>

*/

typedef struct requiredArg
{
	bool bSpace2Tab; // true => space -> tab;   false => tab -> space
	int  convertNumber;
	int  convertFlag; // 1:head , 2:tail 3:headtail
	bool isForce;	  // force replace if all bytes are   <space>/<tab>
	string filename;
} requiredArg;

void printUsage()
{
	static const auto usage = R"(
Usage :
	$ spacetabconvert  --2space <1-tab   -> n-space>  --head|tail|headtail  [--force]  <filename>
	$ spacetabconvert  --2tab   <n-space -> 1-tab>    --head|tail|headtail  [--force]  <filename>
)";
	cout << usage << endl;

}




bool parseArgs(int argc, char* argv[], string& error, requiredArg& reqInfo)
{
	if ( argc < 5 ) {
		error = "argc < 5";
		return false;
	}

	string arg1 = string(argv[1]);
	if ( arg1 != "--2space" &&  arg1 != "--2tab" ) {
		error = "Unknow args , valid arg must be either --2space or --2tab";
		return false;
	}
	reqInfo.bSpace2Tab = (arg1 == "--2tab");

	string arg2 = string(argv[2]);
	auto cvtnumber = 0;
	try {
		cvtnumber = std::stoi( arg2 );
	} catch ( const invalid_argument& invError ) {
        (void)invError;
		error = arg2 + " is invalid int number";
	} catch (const out_of_range& overflow ) {
        (void)overflow;
		error = arg2 + " is out of range";
	}

	if ( cvtnumber <= 0 ) {
		error = "convert number must >= 1";
		return false;
	}
	reqInfo.convertNumber = cvtnumber;


	string arg3 = argv[3];
	if ( arg3 == "--head" ) {
		reqInfo.convertFlag = 1;
	} else if ( arg3 == "--tail" ) {
		reqInfo.convertFlag = 2;
	} else if ( arg3 == "--headtail" ) {
		reqInfo.convertFlag = 3;
	} else {
		error = "convert number must >= 1";
		return false;
	}

	string arg4 = argv[4];
	if (  argc <= 5 ) {
		// filename
		if ( arg4 == "--force" ) {
			error = "arg5 must be a existed file name";
			return false;
		} else {
			reqInfo.isForce  = false;
			reqInfo.filename = arg4;
		}
	} else {
		// > 5  :   == 6
		string arg5 = argv[5];

		if ( arg4 == "--force" ) {
			reqInfo.isForce  = true;
		} else {
			error = "unknown options , available option is   --force";
			return false;
		}	

		reqInfo.filename = arg5;
	}

	return true;
}

string genReplacementStr(const string& originalstr, bool space2tab, int repNum)
{
	string sret;
	if ( space2tab ) {
		// n <space>  =>  1 <tab>
		int cnt =  static_cast<int>( originalstr.size() );
		int parts = cnt / repNum;
		int rest = cnt % repNum;
		int aftercvtnum = (rest == 0 ? parts : parts + 1);
		for ( int i = 0; i < aftercvtnum; ++i) {
			sret += '\t';
		}
	} else {
		// 1 <tab>  =>  n <space>
		int aftercvtnum = static_cast<int>( originalstr.size() ) * repNum;
		for ( int i = 0; i < aftercvtnum; ++i ) {
			sret += ' ';
		}
	}

	return sret;
}

bool doCvt(const requiredArg& arginfo, string& errorMsg)
{
	ifstream file(arginfo.filename.c_str() , ios::in | ios::binary);
	if ( !file ) {
		errorMsg = "[ERROR] Can't open file : " + arginfo.filename;
		return false;
	}

    file.seekg(0, ios::end);
    size_t szOfFile = static_cast<size_t>( file.tellg() );
    file.seekg(0, ios::beg);
	if ( szOfFile == 0 ) {
		cout << "[DONE] Empty File , Do Nothing " << endl;
		return true;
	}

	char* filebuf = new char[szOfFile];
	file.read(filebuf, szOfFile);
	file.close();

	vector< pair<string,int> > lineVec;
	lineVec.clear();

	string line;
	auto previous_nextlineflag = 0; // 1: \n, 2:\r\n  3:\r   0:not decided
	for ( size_t pos = 0; pos < szOfFile;  ) {

		char byte = filebuf[pos];

		if ( byte == '\r' ) {
			auto nextpos = pos + 1;
			char nextbyte = 0;
			if ( nextpos < szOfFile ) {
				nextbyte = filebuf[nextpos];
				if ( nextbyte == '\n' ) {
					// \r\n
					previous_nextlineflag = 2;
				} else {
					previous_nextlineflag = 3;
				}	
			} else {
				// this \r is the last byte
				previous_nextlineflag = 3;
			}
		} else if ( byte == '\n' ) {
			previous_nextlineflag = 1;
		} else {
			previous_nextlineflag = 0;
			line += byte;
		}


		//
		if ( previous_nextlineflag != 0 ) {
			// meet eof
			lineVec.push_back ( make_pair(line, previous_nextlineflag) );

			if ( previous_nextlineflag == 2 ) {
				pos += 2;
			} else {
				++pos;
			}

			line.clear();
		} else {
			++pos;
			if ( pos >= szOfFile ) {
				lineVec.push_back ( make_pair(line, previous_nextlineflag) );
			}
		}
	}

	// true : n <spc>    =>   1 <tab>
	// false: 1 <tab>    =>   n <spc>
	auto isSpace2tab = arginfo.bSpace2Tab;
	auto cvtNum = arginfo.convertNumber;
	for ( auto& pr : lineVec ) {
		auto linestr = pr.first;

		if ( !linestr.empty() ) {
			char findwhat = isSpace2tab ? ' ' : '\t';
			auto foundpos = linestr.find_first_not_of(findwhat);

			string preBlank;
			string validpart;
			string postBlank;
			string replacestr;
			if ( foundpos == string::npos ) {
				// line content are full of Space/Tab
				if ( arginfo.convertFlag == 1 || arginfo.convertFlag == 3  ) {
					if ( arginfo.isForce ) {
						replacestr = genReplacementStr(linestr, isSpace2tab, cvtNum);
					} else {
						replacestr = linestr;
					}
				} else {
					replacestr = linestr;
				}
			} else {
				// find first letter
				preBlank = linestr.substr(0, foundpos);
				auto foundlast = linestr.find_last_not_of(findwhat);
				if ( foundlast == linestr.size()-1 ) {
					// find last byte
					validpart = linestr.substr(foundpos, foundlast - foundpos + 1);
					postBlank = "";
				} else {
					validpart = linestr.substr(foundpos, foundlast - foundpos + 1);
					postBlank = linestr.substr(foundlast + 1);
				}

				//
				// Head
				//
				if ( arginfo.convertFlag == 1 || arginfo.convertFlag == 3  ) {
					replacestr += genReplacementStr(preBlank, isSpace2tab, cvtNum);
				} else {
					replacestr += preBlank;
				}

				//
				// Middle
				//
				replacestr += validpart;
				//

				//
				// Tail
				//
				if ( arginfo.convertFlag == 2 || arginfo.convertFlag == 3  ) {
					replacestr += genReplacementStr(postBlank, isSpace2tab, cvtNum);
				} else {
					replacestr += postBlank;
				}
			}

			pr.first = replacestr;
		}
	}

	auto bret = false;
	ofstream replacefile(arginfo.filename.c_str() , ios::out | ios::trunc | ios::binary);

	// 1: \n, 2:\r\n  3:\r   0:not decided
	string eolary[] = {
		"",
		"\n",	// 1
		"\r\n", // 2
		"\r",	// 3
	};
	int arysz = static_cast<int>( sizeof(eolary) / sizeof(eolary[0]) );

	if ( replacefile ) {
		for ( const auto& pr : lineVec ) {
			auto linestr = pr.first;
			auto eolflag = pr.second;
			if ( !linestr.empty() ) {
				replacefile.write( linestr.c_str(), linestr.size() );
			}

			if ( eolflag >=0 && eolflag < arysz ) {
				string eolstr = eolary[eolflag];
				if ( !eolstr.empty() ) {
					replacefile.write( eolstr.c_str(), eolstr.size() );
				}
			}
		}

		replacefile.flush();
		replacefile.close();
		bret = true;
	} else {
		errorMsg = "[ERROR] : Can't override file :  " + arginfo.filename + " , Open file failed. ";
		bret = false;
	}

	delete [] filebuf;
	filebuf = nullptr;
	return bret;
}





int main(int argc, char* argv[], char* env[])
{
	(void)env;
	string errorMsg;
	requiredArg argInfo;
	if ( !parseArgs(argc, argv, errorMsg, argInfo) ) {
		cout << errorMsg << endl;
		printUsage();
		return -1;
	}

	if ( !doCvt(argInfo, errorMsg) ) {
		cout << errorMsg << endl;
		return -1;
	}

	return 0;
}
