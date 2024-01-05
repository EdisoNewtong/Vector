#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
using namespace std;

static const auto sG_needPrintLog = 0;

#ifndef _WIN32 
 #include <unistd.h>
#else
 #include <windows.h>
#endif



static const string G_DEFAULT_CFG_FILE { "configTable.cfg" };
static const string G_STR_COMMENT_BEGIN {"//"};
static const char G_CH_SEP {':'};
static const char G_CH_DELETE = 127; // special character with delete flag


//  command argument(s)
struct processArg
{
	string inputFile;
	bool   outputSummary;
	string cfgFile;
	bool hasOutputFlag;
	string outputFile;
	bool bOverrideFlag;
};



struct nextCandidateMap {
	// int  originalCount;
	char replaceWith;
#ifdef USE_POINTER_VERSION_DATA_STRUCT
	unordered_map<char, nextCandidateMap*> nextMap;
#else
	unordered_map<char, nextCandidateMap> nextMap;
#endif
	nextCandidateMap() : /* originalCount(0), */ replaceWith(0), nextMap() { }

#ifdef USE_POINTER_VERSION_DATA_STRUCT
	virtual ~nextCandidateMap() {
		for ( auto it = nextMap.begin(); it != nextMap.end(); ++it ) {
			if ( it->second != nullptr ) {
				delete it->second;
				it->second = nullptr;
			}
		}

		nextMap.clear();
	}
#endif

};



#ifdef USE_POINTER_VERSION_DATA_STRUCT
	using NextMap = unordered_map<char, nextCandidateMap*>;
#else
	using NextMap = unordered_map<char, nextCandidateMap>;
#endif


using NextMapPtr = NextMap*;

void insertInfoMap(const string& toInsertStr, NextMap* pWhichList, char toReplaceWith)
{
	int strLen = static_cast<int>( toInsertStr.size()) ;
	for( auto charIdx = 0; charIdx < strLen; ++charIdx ) {
		if ( pWhichList != nullptr ) {
			char singleCh = toInsertStr.at( charIdx );
			auto foundIt = pWhichList->find( singleCh );
			if ( foundIt == pWhichList->end() ) {
				if ( charIdx < (strLen-1) ) {
#ifdef USE_POINTER_VERSION_DATA_STRUCT
					auto pNewNextMap = new nextCandidateMap();
					pWhichList->insert( make_pair(singleCh , pNewNextMap ) );
					pWhichList = &(pNewNextMap->nextMap);
#else
					pWhichList->insert( make_pair(singleCh , nextCandidateMap() ) );
					pWhichList = &( (pWhichList->find(singleCh)->second).nextMap );
#endif
				} else {
#ifdef USE_POINTER_VERSION_DATA_STRUCT
					auto pNewNextMap = new nextCandidateMap();
					pNewNextMap->replaceWith = toReplaceWith;
					pWhichList->insert( make_pair(singleCh , pNewNextMap  ) );
					// pWhichList->find(singleCh)->second->replaceWith = toReplaceWith;
#else
					pWhichList->insert( make_pair(singleCh , nextCandidateMap() ) );
					pWhichList->find(singleCh)->second.replaceWith = toReplaceWith;
#endif
					pWhichList = nullptr;
				}
			} else {
#ifdef USE_POINTER_VERSION_DATA_STRUCT
				pWhichList = &(foundIt->second)->nextMap;
#else
				pWhichList = &(foundIt->second).nextMap;
#endif
			}
		} 
	}
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



void printSpecialDataStruct(const NextMap& data, int layer)
{
	if ( data.empty() ) {
		return;
	}

	for( auto it = data.begin(); it != data.end(); ++it ) {
		int code = static_cast<int>( it->first & 0xFF);
#ifdef USE_POINTER_VERSION_DATA_STRUCT
		NextMap nextLayer = it->second->nextMap;
#else
		NextMap nextLayer = it->second.nextMap;
#endif
		cout << string(layer,'\t') << std::uppercase << std::hex << code;
		if ( !nextLayer.empty() ) {
			cout << endl;
			printSpecialDataStruct( nextLayer, layer+1);
		} else {
#ifdef USE_POINTER_VERSION_DATA_STRUCT
			cout << "   replace  ==> '" << it->second->replaceWith << "\' " << endl;
#else
			cout << "   replace  ==> '" << it->second.replaceWith << "\' " << endl;
#endif
		}
	}
}


struct fileCharInfo
{
	enum E_REPLACE_FLAG {
		E_NOT_DECIDE = 0,
		E_KEEP, 
		E_DO_REPLACE, 
		E_DO_DELETE,
		E_DO_FORCE_DELETE,
	};

	int lineNo;
	int columnNo;
	char originalCh;

	// replace it , skip it
	//   0 : to decide
	//   1 : keep original
	//   2 : do replacement
	//   3 : skip / to be deleted
	E_REPLACE_FLAG replaceFlag = E_NOT_DECIDE;
	char replacedWithCh;
	string originalSequence;
};



bool readReplacedCfgTable(const string& cfgFileName, NextMapPtr  pCfgList, string& errorMsg )
{
    ifstream cfgFile( cfgFileName.c_str(), ios::in | ios::binary);
    if ( !cfgFile ) {
		// TODO   : format log ?
		errorMsg = "[ERROR] Can't open file \""; 
		errorMsg += cfgFileName;
		errorMsg += "\" \n";
		cfgFile.close();
		return false;
    }

	bool meetError = false;
	int lineNo = 0;
	while ( !cfgFile.eof() )
	{
		string line;
		getline( cfgFile, line);
		if ( line.empty() ) {
			lineNo++;
			continue;
		}


		int strLen =  static_cast<int>( line.size() );

		// Skip the leader <Space> / <Tab>
		int iNoneBlank;
		int successivedSpaceCnt = 0;
		char next2Space {};
		for ( iNoneBlank = 0; iNoneBlank < strLen; ++iNoneBlank ) {
			char ch = line.at(iNoneBlank);
			if ( !(ch == ' ' || ch == '\t') ) {
				next2Space = ch;
				break;
			} else {
				++successivedSpaceCnt;
			}
		}

		if (  iNoneBlank >= strLen ) {
			// all characters are <Space> or <Tab>
			lineNo++;
			continue;
		} else {
			// CAUTION : here is a special line to treat differently
			if ( successivedSpaceCnt > 1 && next2Space == ',' ) {
				line = line.substr( iNoneBlank );
			}
		}

		string::size_type pos = line.find(G_STR_COMMENT_BEGIN);
		if ( pos == string::npos ) { //    Line is not start with    //
			if ( strLen < 3 ) {
				errorMsg = "[ERROR] line.content is not valid ( length < 3 ) ";
				errorMsg += (string(", at line: ") + to_string(lineNo) + "\n");
				meetError = true;
				break;
			} else {
				char toReplaceWith;
				char spCh;
				char ch2 = line.at(1);
				char ch3 = line.at(2);
				if ( ch2 == G_CH_SEP ) {
					// to delete the matched character string
					//            e.g.       ,:＠,＃,～
					//                        ^          // if the second char is ':'
					//
					//  this will delete one of the following characters  ＠ ＃ ～ when matched
					spCh = line.at(0);
					if ( !(spCh >=32 && spCh < 127) ) {
						errorMsg = "[ERROR] seperator char is invalid. ( out of range [32,127) )";
						errorMsg += (string(", at line: ") + to_string(lineNo) + " , code = " + to_string( static_cast<int>( 0xFF & spCh) ) +  "\n");
						meetError = true;
						break;
					}

					string restStr = line.substr(2);
					string::size_type spPos;
					while ( (spPos = restStr.find(spCh)) != string::npos )
					{
						string toReplacedStr = restStr.substr(0, spPos);
						insertInfoMap(toReplacedStr,  pCfgList, G_CH_DELETE );
						restStr = restStr.substr( spPos + 1);
					}

					insertInfoMap(restStr,   pCfgList, G_CH_DELETE );
				} else if ( ch3 == G_CH_SEP ) {
					toReplaceWith = line.at(0);

					if ( !( toReplaceWith >=32 &&  toReplaceWith < 127 ) ) {
						errorMsg = "[ERROR] toReplacedWith char is invalid. ( out of range [32,127) )";
						errorMsg += (string(", at line: ") + to_string(lineNo) + " , code = " + to_string( static_cast<int>( 0xFF & toReplaceWith) ) +  "\n");
						meetError = true;
						break;
					}

					spCh = line.at(1);
					if ( !( spCh >=32 &&  spCh < 127 ) ) {
						errorMsg = "[ERROR] seperator char is invalid. ( out of range [32,127) )";
						errorMsg += (string(", at line: ") + to_string(lineNo) + " , code = " + to_string( static_cast<int>( 0xFF & spCh) ) +  "\n");
						meetError = true;
						break;
					}

					string restStr = line.substr(3);
					string::size_type spPos;
					while ( (spPos = restStr.find(spCh)) != string::npos )
					{
						string toReplacedStr = restStr.substr(0, spPos);
						insertInfoMap(toReplacedStr, pCfgList, toReplaceWith);
						restStr = restStr.substr( spPos + 1);
					}

					insertInfoMap(restStr, pCfgList, toReplaceWith);

				} else {
					errorMsg = "[ERROR] ':' is neither at index [1] nor at index [2] ";
					errorMsg += (string(", at line: ") + to_string(lineNo) + "\n");
					meetError = true;
					break;
				}
			}
		} else if ( pos == 0 ) {
			// Skip Comment Line
			// TODO
		} else {
			// TODO   : format log ?
			errorMsg = "[ERROR] Comment is not at the beginning of file \"";
			errorMsg += cfgFileName;
			errorMsg += (string("\" , at line: ") + to_string(lineNo) + "\n");
			meetError = true;
			break;
		}

		lineNo++;
	}

	cfgFile.close();
	return !meetError;
}



bool isFileExisted(const string& fileName)
{
	ifstream file2Check( fileName.c_str(), ios::in );
	if ( !file2Check ) {
		file2Check.close();
		return false;
	}

	file2Check.close();
	return true;
}


string printUsage()
{
	static const string USAGE = R"(Usage :  
----------------------------------------------------------------------------------------------------------------------------------------------
$ replaceChinesePunctuation  [--verbose] [--cfgfile <config File Name>] --input <input File Name>
or
$ replaceChinesePunctuation  [--verbose] [--cfgfile <config File Name>] --input <input File Name> [ --output <output File Name> | --override ] 
----------------------------------------------------------------------------------------------------------------------------------------------

)";
	return USAGE;
}


bool parseArgs(int argc, char* argv[], processArg& arg, const string& runingProgramAbsPath )
{
	const string KEY_WORD_PRINT_VERBOSE { "--verbose" };
	const string KEY_WORD_CFG_FILE { "--cfgfile" };
	const string KEY_WORD_INPUT_FILE { "--input" };
	const string KEY_WORD_OUTPUT_FILE { "--output" };
	const string KEY_WORD_OVERRIDE { "--override" };

	bool hasPrintVerbose = false;
	bool hasCfgFileKeyWord = false;   int cfgArg = -1;
	bool hasInputKeyWord = false;     int inputArg = -1;
	bool hasOutputKeyWord = false;    int outputArg = -1;
	bool hasOverrideKeyWord = false;  

	for( auto i = 1; i < argc; ++i ) {
		string strArg( argv[i] );
		if ( strArg == KEY_WORD_PRINT_VERBOSE ) {
			hasPrintVerbose = true;	
		} else if ( strArg == KEY_WORD_CFG_FILE ) {
			hasCfgFileKeyWord = true;
			cfgArg = i+1;
		} else if ( strArg == KEY_WORD_INPUT_FILE ) {
			hasInputKeyWord = true;
			inputArg = i+1;
		} else if ( strArg == KEY_WORD_OUTPUT_FILE ) {
			hasOutputKeyWord = true;
			outputArg = i+1;
		} else if ( strArg == KEY_WORD_OVERRIDE ) {
			hasOverrideKeyWord = true;
		} 
	}

	// check input file
	if ( !hasInputKeyWord ) {
		cout << "[ERROR] Missing input keyword.  " << endl;
		return false;
	}

	if ( inputArg >= argc ) {
		cout << "[ERROR] Missing input file argument.  " << endl;
		return false;
	}

	if ( !isFileExisted( string( argv[inputArg] ) ) ) {
		cout << "[ERROR] Input file  " << string( argv[inputArg] ) << " is not existed. " << endl;
		return false;
	}
	arg.inputFile = string( argv[inputArg] );

	if ( hasCfgFileKeyWord ) {
		if ( cfgArg >= argc ) {
			cout << "[ERROR] Missing config file argument.  " << endl;
			return false;
		}

		if ( !isFileExisted( string( argv[cfgArg] ) ) ) {
			cout << "[ERROR] Config file " << string( argv[cfgArg] ) << " is not existed. " <<  endl;
			return false;
		}

		arg.cfgFile = string( argv[cfgArg] );
	} else {
		if ( !isFileExisted( runingProgramAbsPath + G_DEFAULT_CFG_FILE ) ) {
			cout << "[ERROR] Config file " << G_DEFAULT_CFG_FILE << " is not existed. " <<  endl;
			return false;
		}

		arg.cfgFile = ( hasCfgFileKeyWord ? string( argv[cfgArg] ) : (runingProgramAbsPath + G_DEFAULT_CFG_FILE) );
	}


	arg.outputSummary = hasPrintVerbose;
	if ( hasOutputKeyWord && hasOverrideKeyWord ) {
		cout << "[ERROR] : " <<  KEY_WORD_OUTPUT_FILE << " and " << KEY_WORD_OVERRIDE << " are mutual excusive. " << endl;
		return false;
	}

	if ( hasOutputKeyWord ) {
		if ( outputArg >= argc ) {
			cout << "[ERROR] Missing output file argument.  " << endl;
			return false;
		}
		
		if ( string(argv[outputArg]) == arg.inputFile ) {
			cout << "[WARNING] the program will override the input file without any confirm. " << endl;
		}
		arg.hasOutputFlag = true;
		arg.outputFile = string(argv[outputArg]);
		arg.bOverrideFlag = false;

	} else if ( hasOverrideKeyWord ) {
		arg.hasOutputFlag = false;
		arg.bOverrideFlag = true;
	} else {
		arg.hasOutputFlag = false;
		arg.bOverrideFlag = false;
    }

	return true;
}


void tryReplaceChinesePuncPunctuation(const processArg& arg, const NextMap& cfgList, fileCharInfo*& fileContent, size_t& fileSz, vector<fileCharInfo>& recordList)
{
	const string inputfile = arg.inputFile;
	ifstream f( inputfile.c_str(), ios::in | ios::binary);
	if ( !f ) {
		cout << "[ERROR] : Can't open input file : " << inputfile << endl;
		return;
	}

	f.seekg(0, ios::end);
	fileSz = static_cast<size_t>( f.tellg() );
	f.seekg(0, ios::beg);

    if ( fileSz == 0 ) {
		f.close();
		cout << "[INFO] : File's length = 0, Do Nothing. " << endl;
		return;
    }

	char* fileBuf = new char[fileSz];
	fileContent  = new fileCharInfo[fileSz];
	if ( fileBuf == nullptr ||  fileContent == nullptr ) {
		cout << "[ERROR] Memory is not enough to read a big file. " << endl;
		return;
	}

	// read the whole file content into memory
	f.read( fileBuf, fileSz);

	int lineNo = 1;
	int columnNo = 1;
	int matchedCnt = 0;
	string toReplacedStr;
	int replacementActionCnt = 0;
	int deleteActionCnt = 0;

	NextMapPtr pRootMap = const_cast<NextMapPtr>( &cfgList );
	NextMapPtr pMap = pRootMap;

	recordList.clear();
	int iFileSz = static_cast<int>( fileSz );
	for( size_t i = 0; i < fileSz; ++i )
	{
		char targetCh = fileBuf[i];
		fileContent[i].originalCh = targetCh;
		fileContent[i].lineNo = lineNo;
		fileContent[i].columnNo = columnNo;

		if ( pMap != nullptr ) {
			auto it = pMap->find( targetCh );
			if ( it == pMap->end() ) {
				fileContent[i].replaceFlag = fileCharInfo::E_KEEP; // keep original 
                if ( matchedCnt > 0 ) {
					// restore from original state , Keep it as original
					for( int reverseIdx = 1; reverseIdx <=matchedCnt; ++reverseIdx ) {
						int processedIdx = static_cast<int>(i) - reverseIdx;
						if ( processedIdx>=0 && processedIdx < iFileSz ) {
							fileContent[processedIdx].replaceFlag = fileCharInfo::E_KEEP;
						}
					}
                }

                pMap = pRootMap;
				matchedCnt = 0;
				toReplacedStr = "";
			} else {
				++matchedCnt;
				toReplacedStr += targetCh;

#ifdef USE_POINTER_VERSION_DATA_STRUCT
				char repacedWithCh = it->second->replaceWith;
				pMap = &(it->second->nextMap);
#else
				char repacedWithCh = it->second.replaceWith;
				pMap = &(it->second.nextMap);
#endif

				if ( pMap!=nullptr ) {
					// all matched
					if ( pMap->empty() ) {
						for( int reverseIdx = 0; reverseIdx < matchedCnt; ++reverseIdx ) {
							int processedIdx = static_cast<int>(i) - reverseIdx;
							if ( processedIdx>=0 && processedIdx < iFileSz ) {
								if ( reverseIdx == (matchedCnt-1) ) {
									if ( G_CH_DELETE == repacedWithCh ) {
										fileContent[processedIdx].replaceFlag = fileCharInfo::E_DO_FORCE_DELETE; // Do Delete
										fileContent[processedIdx].originalSequence = toReplacedStr;
																										   //
										recordList.push_back( fileContent[processedIdx] );

										++deleteActionCnt;
									} else {
										fileContent[processedIdx].replaceFlag = fileCharInfo::E_DO_REPLACE; // do real replace action 
										fileContent[processedIdx].replacedWithCh = repacedWithCh;
										fileContent[processedIdx].originalSequence = toReplacedStr;

										recordList.push_back( fileContent[processedIdx] );
										++replacementActionCnt;

										// if ( recordList.empty() ) {
										// 	recordList.push_back( *(fileContent[processedIdx]) );
										// } else {
										// 	++(recordList.back());
										// }
										
									}
								} else {
									fileContent[processedIdx].replaceFlag = fileCharInfo::E_DO_DELETE; // Do Delete  
								}
							}
						}

						pMap = pRootMap;
						matchedCnt = 0;
						toReplacedStr = "";
					} 
				}
			}
		}

		if ( fileBuf[i] == '\n' ) {
			++lineNo;
			columnNo = 1;

			/*
			if ( i < (fileSz-1) ) {
				recordList.push_back( 0 );
			}
			*/
		} else {
			++columnNo;
		}
	}

	if ( fileBuf != nullptr ) {
		delete [] fileBuf;
		fileBuf = nullptr;
	}

	if (  arg.hasOutputFlag || arg.bOverrideFlag ) {
		string outputfileName;
		// output -> a given file name
		if ( arg.bOverrideFlag ) {
			// if override the existed file , close file handler first
			f.close();

			outputfileName = arg.inputFile;
		} else {
			outputfileName = arg.outputFile;

			f.close();
		}

		ofstream fileOut( outputfileName.c_str(), ios::out | ios::trunc | ios::binary);
		if ( !fileOut ) {
			cout << "[ERROR] Can't open output file : " << outputfileName << endl;
			fileOut.close();
		} 

		// Open file successful
		for ( size_t i = 0; i < fileSz; ++i ) {
			auto eNum_flag = fileContent[i].replaceFlag;
			switch ( eNum_flag ) {
				case fileCharInfo::E_NOT_DECIDE:
				case fileCharInfo::E_KEEP:
					fileOut << fileContent[i].originalCh;
					break;
				case fileCharInfo::E_DO_REPLACE:
					{
						if ( fileContent[i].replacedWithCh != 0 ) {
							// Do Replace
							fileOut << fileContent[i].replacedWithCh;
						} else {
							// Keep Original
							fileOut << fileContent[i].originalCh;
						}
					}
					break;
				case fileCharInfo::E_DO_DELETE:
				case fileCharInfo::E_DO_FORCE_DELETE:
					// Do Nothing , <Skip> it without writing
					break;
				default:
					break;
			}
		}

		if ( !(replacementActionCnt > 0 || deleteActionCnt > 0) ) {
			cout << "[INFO] Nothing matched to be replaced . Duplicate the given file to a new file :  " << outputfileName << endl;
		} else {
			int total = replacementActionCnt + deleteActionCnt;
			cout << "[INFO] There " << (total>1 ? "are " : "is ") << total << " change(s) to process [Replace/Delete] " << endl;
		}

		fileOut.flush();
		fileOut.close();
	} else {
		// output --> stdout
		if ( replacementActionCnt > 0 || deleteActionCnt > 0 ) {
			for ( size_t i = 0; i < fileSz; ++i ) {
				auto eNum_flag = fileContent[i].replaceFlag;
				switch ( eNum_flag ) {
					case fileCharInfo::E_NOT_DECIDE:
					case fileCharInfo::E_KEEP:
						cout << fileContent[i].originalCh;
						break;
					case fileCharInfo::E_DO_REPLACE:
						{
							if ( fileContent[i].replacedWithCh != 0) {
								// Do Replace
								cout << fileContent[i].replacedWithCh;
							} else {
								// Keep Original
								cout << fileContent[i].originalCh;
							}
						}
						break;
					case fileCharInfo::E_DO_DELETE:
					case fileCharInfo::E_DO_FORCE_DELETE:
						// Do Nothing , <Skip> it without writing
						break;
					default:
						break;
				}
			}
			cout.flush();
		} else {
			cout << "[INFO] Nothing matched to be replaced . Keep Original. " << endl;
		}

		f.close();
	}

}

int main(int argc, char* argv[], char* env[])
{
    string runningPath = getBinaryPath(string(argv[0]), env);
	if ( argc < 2 ) {
		cout << printUsage() << endl;
		return -1;
	}

	processArg argObj;
	auto bArgParsedFlag = parseArgs(argc, argv, argObj, runningPath);
	if ( !bArgParsedFlag ) {
		cout << "[ERROR] Parse argument Failed. " << endl;
		cout << printUsage() << endl;
		return -1;
	} else {
		auto printArgDetail = 0;
		if ( printArgDetail ) {
			cout << "inputFile = " << argObj.inputFile << endl;
			cout << "outputSummary = " << argObj.outputSummary << endl;
			cout << "cfgFile = " << argObj.cfgFile << endl;
			cout << "hasOutputFlag = " << argObj.hasOutputFlag << endl;
			cout << "outputFile = " << argObj.outputFile << endl;
			cout << "bOverrideFlag = " << argObj.bOverrideFlag << endl;
		}
	}

	string errorMsg;
#ifdef USE_POINTER_VERSION_DATA_STRUCT
	NextMapPtr pCfgList = new NextMap();
#else
	NextMap cfgList;
	NextMapPtr pCfgList = &cfgList;
#endif

	auto bReadCfgRet = readReplacedCfgTable( argObj.cfgFile, pCfgList, errorMsg);
	if ( !bReadCfgRet ) {
		cout << "[ERROR] : " << errorMsg << endl;
		return -1;
	} else {
		if ( sG_needPrintLog  ) {
			int layer = 0;
#ifdef USE_POINTER_VERSION_DATA_STRUCT
			printSpecialDataStruct(*pCfgList, layer);
#else
			printSpecialDataStruct(cfgList, layer);
#endif
		}
	}

	size_t fileSz = 0;
	fileCharInfo* pFileContent = nullptr;
	vector<fileCharInfo> lineRecord;
#ifdef USE_POINTER_VERSION_DATA_STRUCT
	tryReplaceChinesePuncPunctuation( argObj, *pCfgList, pFileContent, fileSz, lineRecord);
#else
	tryReplaceChinesePuncPunctuation( argObj, cfgList, pFileContent, fileSz, lineRecord);
#endif

	if ( argObj.outputSummary ) {
		cout << endl;

		if ( lineRecord.empty() ) {
			cout << "[INFO] Replacement List is !! Empty !! ." << endl;
		}

		int iCountInsideLine = 0;
		int lineNo = 0;
		  // first  : lineNo
		  // second : matched count
		vector< pair<int,int> > lineCnt;
		for( const auto& e : lineRecord ) {
			if ( e.lineNo != lineNo ) {
				cout << e.lineNo << ". " << endl;
				// line changed
				iCountInsideLine = 1;
				if ( lineNo == 0 ) {
					// 1st init 
					lineCnt.push_back( make_pair(e.lineNo, iCountInsideLine) );
				} else {
					lineCnt.push_back( make_pair(lineNo, iCountInsideLine) );
				}

				lineNo = e.lineNo;

				if ( e.replaceFlag == fileCharInfo::E_DO_REPLACE ) {
					cout << "\t" << iCountInsideLine << ". \"" << e.originalSequence << "\" " << "Do Replace @"  << lineNo << ":" << e.columnNo << " -> '"<< e.replacedWithCh << "'" <<  endl;
				} else {
					cout << "\t" << iCountInsideLine << ". \"" << e.originalSequence << "\" " <<  "Do Force Delete @" << lineNo << ":" << e.columnNo << endl;
				}
			} else {
				// line not changed
				if ( !lineCnt.empty() ) {
					++(lineCnt.back().second);

					if ( e.replaceFlag == fileCharInfo::E_DO_REPLACE ) {
						cout << "\t" << (lineCnt.back().second) << ". \"" << e.originalSequence << "\" " << "Do Replace @"  << lineNo << ":" << e.columnNo << " -> '"<< e.replacedWithCh << "'" <<  endl;
					} else {
						cout << "\t" << (lineCnt.back().second) << ". \"" << e.originalSequence << "\" " <<  "Do Force Delete @" << lineNo << ":" << e.columnNo << endl;
					}
				}
			}
		}
	}

	if ( fileSz > 0  &&  pFileContent != nullptr ) {
		delete [] pFileContent;
		pFileContent = nullptr;
	}

#ifdef USE_POINTER_VERSION_DATA_STRUCT
	if ( pCfgList != nullptr ) {
		delete pCfgList;
		pCfgList = nullptr;
	}
#endif 

	return 0;
}

