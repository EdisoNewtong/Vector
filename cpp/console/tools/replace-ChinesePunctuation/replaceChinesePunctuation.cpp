#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
using namespace std;

bool b_G_printProcessedLineDetail = true;

static const string G_DEFAULT_CFG_FILE { "replaceTable.cfg" };
static const string G_STR_COMMENT_BEGIN {"//"};
static const char G_CH_SEP {':'};
static const char G_CH_DELETE = 127; // special character with delete flag


//  command argument(s)
struct processArg
{
	string inputFile;
	bool   scanOnly;
	string cfgFile;
	bool hasOutputFlag;
	string outputFile;
	bool bOverrideFlag;
};


// ,    2C     =>   EF BC 8C (U+00FF0C)
// struct replacementInfo
// {
// 	char spCh;
// 	char replaceWith;
//     bool needToDelete;
// 	vector<string> whichStringToReplace;
// };


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
		// else {
		// 	// TODO
		// }
	}
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
		E_DO_DELETE
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
	static const string USAGE = R"( Default config file named "replaceTable.cfg"
----------------------------------------------------------------------------------------------------------------------------------------------
$ replaceChinesePunctuation (--nolog) ( --scanonly ) ( --cfgfile <config File Name> ) --input <input File Name>
or
$ replaceChinesePunctuation (--nolog) (--cfgfile <config File Name>) --input <input File Name> ( --output <output File Name> | --override ) 
----------------------------------------------------------------------------------------------------------------------------------------------

)";
	return USAGE;
}


bool parseArgs(int argc, char* argv[], processArg& pArg)
{
	const string KEY_WORD_SCAN_ONLY { "--scanonly" };
	const string KEY_WORD_CFG_FILE { "--cfgfile" };
	const string KEY_WORD_INPUT_FILE { "--input" };
	const string KEY_WORD_OUTPUT_FILE { "--output" };
	const string KEY_WORD_OVERRIDE { "--override" };
	const string KEY_WORD_NO_LOG { "--nolog" };

	bool hasScanOnlyKeyWord = false;
	bool hasCfgFileKeyWord = false;   int cfgArg = -1;
	bool hasInputKeyWord = false;     int inputArg = -1;
	bool hasOutputKeyWord = false;    int outputArg = -1;
	bool hasOverrideKeyWord = false;  

	for( auto i = 1; i < argc; ++i ) {
		string strArg( argv[i] );
		if ( strArg == KEY_WORD_SCAN_ONLY ) {
			hasScanOnlyKeyWord = true;
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
		} else if ( strArg == KEY_WORD_NO_LOG ) {
			b_G_printProcessedLineDetail = false;
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
	pArg.inputFile = string( argv[inputArg] );

	if ( hasCfgFileKeyWord ) {
		if ( cfgArg >= argc ) {
			cout << "[ERROR] Missing config file argument.  " << endl;
			return false;
		}

		if ( !isFileExisted( string( argv[cfgArg] ) ) ) {
			cout << "[ERROR] Config file " << string( argv[cfgArg] ) << " is not existed. " <<  endl;
			return false;
		}

		pArg.cfgFile = string( argv[cfgArg] );
	} else {
		if ( !isFileExisted( G_DEFAULT_CFG_FILE ) ) {
			cout << "[ERROR] Config file " << G_DEFAULT_CFG_FILE << " is not existed. " <<  endl;
			return false;
		}
	}
	pArg.cfgFile = ( hasCfgFileKeyWord ? string( argv[cfgArg] ) : G_DEFAULT_CFG_FILE);

	if ( hasScanOnlyKeyWord  ) {
		if ( hasOutputKeyWord || hasOverrideKeyWord ) {
			cout << "[ERROR] : " << KEY_WORD_SCAN_ONLY << "   and   " << KEY_WORD_OUTPUT_FILE << " / " << KEY_WORD_OVERRIDE << "is mutual excusive. " << endl;
			return false;
		}
	}

	pArg.scanOnly = hasScanOnlyKeyWord;
	if ( hasOutputKeyWord && hasOverrideKeyWord ) {
		cout << "[ERROR] : " <<  KEY_WORD_OUTPUT_FILE << " and " << KEY_WORD_OVERRIDE << " are mutual excusive. " << endl;
		return false;
	}

	if ( hasOutputKeyWord ) {
		if ( outputArg >= argc ) {
			cout << "[ERROR] Missing output file argument.  " << endl;
			return false;
		}
		
		if ( string(argv[outputArg]) == pArg.inputFile ) {
			cout << "[WARNING] the program will override the input file without any confirm. " << endl;
		}
		pArg.hasOutputFlag = true;
		pArg.outputFile = string(argv[outputArg]);
		pArg.bOverrideFlag = false;

	} else if ( hasOverrideKeyWord ) {
		pArg.hasOutputFlag = false;
		pArg.bOverrideFlag = true;
	} else {
		pArg.hasOutputFlag = false;
		pArg.bOverrideFlag = false;
    }

	return true;
}


void tryReplaceChinesePuncPunctuation(const processArg& arg, const NextMap& cfgList, fileCharInfo*& fileContent, size_t& fileSz, vector<int>& recordList)
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

	NextMapPtr pRootMap = const_cast<NextMapPtr>( &cfgList );
	NextMapPtr pMap = pRootMap;

	// vector<int> recordList;
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
						int processedIdx = i - reverseIdx;
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
							int processedIdx = i - reverseIdx;
							if ( processedIdx>=0 && processedIdx < iFileSz ) {
								if ( reverseIdx == (matchedCnt-1) ) {
									if ( G_CH_DELETE == repacedWithCh ) {
										fileContent[processedIdx].replaceFlag = fileCharInfo::E_DO_DELETE; // do delete
									} else {
										fileContent[processedIdx].replaceFlag = fileCharInfo::E_DO_REPLACE; // do real replace action 
										fileContent[processedIdx].replacedWithCh = repacedWithCh;

										if ( recordList.empty() ) {
											recordList.push_back(1);
										} else {
											++(recordList.back());
										}
										
									}
								} else {
									fileContent[processedIdx].replaceFlag = fileCharInfo::E_DO_DELETE; // do delete  
								}
							}
						}

						if ( arg.scanOnly ) {
							if ( G_CH_DELETE != repacedWithCh ) {
								cout << "[INFO] Replace '" << toReplacedStr << "' with  '" << repacedWithCh << "' @Line " << lineNo << ":" << columnNo << " ." << endl;
							} else {
								cout << "[INFO] Delete '" << toReplacedStr << " @Line " << lineNo << ":" << columnNo << " ." << endl;
							}
						}

						pMap = pRootMap;
						matchedCnt = 0;
						toReplacedStr = "";
					} 
					// else {
					// 	// pMap = pRootMap;
					// }
				}
			}
		}

		if ( fileBuf[i] == '\n' ) {
			++lineNo;
			columnNo = 1;

			if ( i < (fileSz-1) ) {
				// new line
				recordList.push_back( 0 );
			}
		} else {
			++columnNo;
		}
	}

	if ( fileBuf != nullptr ) {
		delete [] fileBuf;
		fileBuf = nullptr;
	}


	if ( arg.hasOutputFlag || arg.bOverrideFlag ) {
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
			    	fileOut << fileContent[i].replacedWithCh;
			    	break;
			    case fileCharInfo::E_DO_DELETE:
			    	// Do Nothing
			    	break;
			    default:
			    	break;
			}
		}

		fileOut.flush();
		fileOut.close();
	} else {
		// output --> stdout
		for ( size_t i = 0; i < fileSz; ++i ) {
			auto eNum_flag = fileContent[i].replaceFlag;
			switch ( eNum_flag ) {
			    case fileCharInfo::E_NOT_DECIDE:
			    case fileCharInfo::E_KEEP:
			    	cout << fileContent[i].originalCh;
			    	break;
			    case fileCharInfo::E_DO_REPLACE:
			    	cout << fileContent[i].replacedWithCh;
			    	break;
			    case fileCharInfo::E_DO_DELETE:
			    	// Do Nothing
			    	break;
			    default:
			    	break;
			}
		}
		cout.flush();

		f.close();
	}
}

int main(int argc, char* argv[])
{
	if ( argc < 2 ) {
		cout << printUsage() << endl;
		return -1;
	}

	processArg pArg;
	auto bArgParsedFlag = parseArgs(argc, argv, pArg);
	if ( !bArgParsedFlag ) {
		cout << "[ERROR] Parse argument Failed. " << endl;
		cout << printUsage() << endl;
		return -1;
	}

	string errorMsg;
#ifdef USE_POINTER_VERSION_DATA_STRUCT
	NextMapPtr pCfgList = new NextMap();
#else
	NextMap cfgList;
	NextMapPtr pCfgList = &cfgList;
#endif

	auto bReadCfgRet = readReplacedCfgTable( pArg.cfgFile, pCfgList, errorMsg);
	if ( !bReadCfgRet ) {
		cout << "[ERROR] : " << errorMsg << endl;
		return -1;
	} else {
		auto printLog = 0; 
		if ( printLog ) {
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
	vector<int> lineRecord;
#ifdef USE_POINTER_VERSION_DATA_STRUCT
	tryReplaceChinesePuncPunctuation( pArg, *pCfgList, pFileContent, fileSz, lineRecord);
#else
	tryReplaceChinesePuncPunctuation( pArg, cfgList, pFileContent, fileSz, lineRecord);
#endif
	if ( b_G_printProcessedLineDetail ) {
		cout << endl << endl;
		for( size_t idx = 0; idx < lineRecord.size(); ++idx ) {
			cout << "Line " << (idx+1) << ", hit count is [" << lineRecord.at(idx) << "]. " << endl;
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
 