#include <iostream> 
#include <iomanip>
// #include <cassert>

#include "SuperBinaryDisplayUtil.h"


namespace SuperBinaryDisplayUtil
{


bool isCanInputChar(const char& ch)
{
	// 0111 1111		0177		127			0x7F				DEL (delete) 删除   ,  can't be saved into a pure text file
    return    (ch >= 32 && ch <= 126)  ||   (ch == G_CHAR_TAB.ch)   ||    (ch == G_CHAR_GN.ch)   ||   (ch == G_CHAR_GR.ch)  || (ch == G_CHAR_VTAB.ch) || (ch == G_CHAR_GNP.ch);
}

unsigned int char2HexCode(const char& ch, string* displayHexCode /* = nullptr */ )
{
    unsigned int retCode = (FULL_BITS & ch);
    if( displayHexCode!=nullptr ) {
		stringstream fmtstr;
		fmtstr << std::hex << std::uppercase << setw(2) << setfill('0') << retCode;
        *displayHexCode = fmtstr.str();
    }

    return retCode;
}


int getDecimalBits(int num)
{
	if ( num == 0 ) {
		return 1;
	}

	auto isPositive = (num > 0);
	auto positive_num =  isPositive ? num : (-num);
	// if num is negative , u must take the sign '-'   into account , so + 1
	auto nBits = (isPositive ? 0 : 1);
	do {
		++nBits;
		positive_num /= 10;
	} while( positive_num!=0 );

	return nBits;
}



//////////////////////////////////////////////////
// Start : struct   SingleCharacter   Part
//////////////////////////////////////////////////
// protected:
SingleCharacter::SingleCharacter() 
    : ansi_char(0) 
    // , binary_charAry{ 0, 0, 0, 0 }
    , display_str("")
    // , utf8HexAry{ "","","","" }
    , nBytes(0U)
    , unicode32(0U) 
    , unicode32_str("")
    , nRow(0)
    , nCol(0)
    , nEOF_Flag(0)
    , reserverd_flag(0) 
{ 
    for ( size_t i = 0; i < UTF8_FULL_LEN; ++i ) { 
        utf8HexAry[i] = ""; binary_charAry[i] = 0; 
    }  
}

// virtual 
SingleCharacter::~SingleCharacter() 
{ 
}


// static 
SingleCharacter* SingleCharacter::generate_ASCII_Character(char ch, int row, int col, int eofFlag) 
{ 
    SingleCharacter* pRet = new SingleCharacter();
    string hexStrCode;

    pRet->ansi_char = ch;
    pRet->binary_charAry[0] = ch;
    auto ucode = char2HexCode(ch, &hexStrCode);
    if( eofFlag != 0 ) {
		pRet->display_str = (ch == G_CHAR_GN.ch ? G_CHAR_GN.displayStr : G_CHAR_GR.displayStr);

        pRet->utf8HexAry[0] = hexStrCode;
        if ( eofFlag == 2 ) { // check is E_WIN mode , must modified the matched enum value if E_WIN != 2
            pRet->utf8HexAry[1] = G_CHAR_GN.hexCodeStr;  // append "\\n" as the 2nd letter; , because \r\n
        }
    } else {
        if( ch == G_CHAR_TAB.ch ) {
            pRet->display_str = G_CHAR_TAB.displayStr;
        } else if ( ch == G_CHAR_VTAB.ch ) {
            pRet->display_str = G_CHAR_VTAB.displayStr;
        } else if ( ch == G_CHAR_GNP.ch ) {
            pRet->display_str = G_CHAR_GNP.displayStr;
        } else {
            pRet->display_str = ch;
        }

        pRet->utf8HexAry[0] = hexStrCode;
    }

    pRet->nBytes = 1; // must be 1
    pRet->unicode32 = ucode;
    pRet->unicode32_str = pRet->utf8HexAry[0];  // ascii Char , Unicode_str is same as utf8HexAry's 1st element

    pRet->nRow = row; 
    pRet->nCol = col;  
    pRet->nEOF_Flag = eofFlag; 
    pRet->reserverd_flag = 0;

    return pRet;
}


// static 
SingleCharacter* SingleCharacter::generate_MultiBytes_Character(const string& displayCharacter, unsigned int unicode32, int row, int col) 
{
    SingleCharacter* pRet = new SingleCharacter();
    stringstream fmtstr;
    int u32DisplayLen = 0;

    pRet->display_str = displayCharacter;
    pRet->nBytes = static_cast<unsigned short>( displayCharacter.size() );
    for( size_t idx = 0; idx < displayCharacter.size(); ++idx) {
        pRet->binary_charAry[idx] = displayCharacter.at(idx);
    }

    // assign hex Array
    for (unsigned short i = 0; i < pRet->nBytes; ++i ) {
         char2HexCode( displayCharacter.at(i), &pRet->utf8HexAry[i]);
    }
    pRet->unicode32 = unicode32;
    
    // format Unicode 32 string
    u32DisplayLen = pRet->nBytes==2 ? 4 : (pRet->nBytes==3 ? 6 : 8);
    fmtstr << "U+" << std::hex << std::uppercase << setw(u32DisplayLen) << setfill('0') << unicode32;
    pRet->unicode32_str = fmtstr.str();

    pRet->nRow = row;
    pRet->nCol = col;
    pRet->nEOF_Flag = 0; // will never be  \r or \n , so must be 0
    pRet->reserverd_flag = 0;

    return pRet;
}

//////////////////////////////////////////////////
// End : struct   SingleCharacter   Part
//////////////////////////////////////////////////






//////////////////////////////////////////////////
// Start : struct   FileInfo   Part
//////////////////////////////////////////////////
FileInfo::FileInfo()
    : filename("")
    , isValid(false)
    , isBomFile(false)
    , isBinary(false)
    , bytesCount(0)
    , characterCount(0)
    , lineCnt(1) // 即使是2进制文件，也至少有1行
    , hybridLineNo(0)
    , endlineMode(E_UNKNOWN_ENDLINE)
    , firstBinaryByte_Idx(0)
    , firstBinaryByte_Row(-1)
    , firstBinaryByte_Col(-1)
{  
    // Commonly there must be at least one line in a file unless the file is empty file ( it contains 0 bytes )
    lineVec.push_back( make_pair(new list<SingleCharacter*>(), 1) );
}

// virtual
FileInfo:: ~FileInfo()
{
    for ( auto it = rawHexVec.begin(); it != rawHexVec.end(); ++it ) { delete (*it); (*it) = nullptr; }
    for ( auto it = lineVec.begin(); it != lineVec.end(); ++it ) { 
        list<SingleCharacter*>* pInner_list = it->first;
        if ( pInner_list != nullptr ) { 
            // delete inner word
            for ( auto it_inner = pInner_list->begin(); it_inner != pInner_list->end(); ++it_inner ) { 
                delete (*it_inner); 
                (*it_inner) = nullptr; 
            } 
            pInner_list->clear(); 

            // release list memory
            delete pInner_list; 
            pInner_list = nullptr; 
        }
    }

    rawHexVec.clear();
    lineVec.clear();
}
//////////////////////////////////////////////////
// End : struct   FileInfo   Part
//////////////////////////////////////////////////




/*
 ****************************************************************************************************
						UTF8  Core Core Core Principle
 ****************************************************************************************************
 
UTF-8 就是使用变长字节表示,顾名思义，就是使用的字节数可变，这个变化是根据 Unicode 编号的大小有关，编号小的使用的字节就少，编号大的使用的字节就多。使用的字节个数从 1 到 4 个不等。
UTF-8 的编码规则是：
 
1. 对于1个字节(单字节)的符号:
	 字节的第一位设为 0，
	 后面的7位为这个符号的 Unicode 码，
	 因此对于英文字母，UTF-8 编码和 ASCII 码是相同的。
 
2. 对于n字节的符号 （n>1）:
   第一个字节的前 n 位都设为 1，  ( 也就是说 至少有2个11  , 即  肯定是以下3种的其中之一的情况
	<1>.    110X,XXXX | 10XX,XXXX |								(2 bytes to build an Non-Ansi Character) , Valid bits = 5 + 6 = 11		   -> 1 byte  + 3 bits (need 2 bytes) ( so unicode32 = U+XXXX   )
	<2>.    1110,XXXX | 10XX,XXXX | 10XX,XXXX |					(3 bytes to build an Non-Ansi Character) , Valid bits = 4 + 6 + 6 = 16     -> 2 bytes		   (need 2 bytes) ( so unicode32 = U+XXXX   )
	<3>.    1111,0XXX | 10XX,XXXX | 10XX,XXXX | 10XX,XXXX |		(4 bytes to build an Non-Ansi Character) , Valid bits = 3 + 6 + 6 + 6 = 21 -> 2 bytes + 5 bits (need 3 bytes) ( so unicode32 = U+XXXXXX )
 
	因为Unicode 最多使用4个字节表示一个文字 )
 
  第 n+1 位设为 0
		第2~4个字节的前两位一律设为 10
	 	剩下的没有提及的二进制位，全部为这个符号的 Unicode 码 。
 
 ****************************************************************************************************
*/

// very very old version
bool readFile_V0(const string& filename, FileInfo& fInfo)
{
    ifstream file(filename.c_str() , ios::in | ios::binary);
    fInfo.filename = filename;
    if ( !file ) {
        fInfo.lineCnt = 0;
        if( SHOULD_LOG_OPEN_FAILED ) {
            cout << "Sorry , Can't open file : " << filename << endl;
        }
        return false;
    }
    fInfo.isValid = true;
    

    // bool treatEOFAsByte = true; (void)
    // bool treatRNAs1Byte = true;

    char* fileContentBuf = nullptr;
    size_t szOfFile = 0;
    file.seekg(0, ios::end);
    szOfFile = static_cast<size_t>(file.tellg());
    file.seekg(0, ios::beg);
    fInfo.bytesCount = szOfFile;
    
    if ( szOfFile == 0 ) {
        if( SHOULD_LOG_EMPTY_FILE ){ 
            cout << "The File named  \"" << filename << "\" has 0 byte , It's an empty file. Nothing to read !" << endl;
        }
        fInfo.lineCnt = 0;
        return true;
    } else {
        // alloc and read all byte from file and fill into Buf-Array
        fileContentBuf = new char[szOfFile];
        file.read(fileContentBuf, szOfFile);
    }
    
    if ( szOfFile >= SIZE_BOM ) {
        auto equalFlag = true;
        for (size_t i = 0; i < SIZE_BOM; ++i) {
            unsigned int code = (FULL_BITS & fileContentBuf[i]);
            if ( code != utf8_Bom_tag[i] ) {
                equalFlag = false;
                break;
            }
        }
        fInfo.isBomFile = equalFlag;
    } else {
        // bytes no more than SIZE_BOM
        fInfo.isBomFile = false;
    }
    
    
    stringstream fmtstr;
    if ( fInfo.isBomFile ) {
        // realszOfFile = szOfFile - SIZE_BOM;
        for ( size_t i = 0; i < SIZE_BOM; ++i ) {
            unsigned int code = utf8_Bom_tag[i];
            fmtstr.clear();
            fmtstr.str("");
            fmtstr << std::hex << std::uppercase << setw(2) << setfill('0') << code;
            
            fInfo.rawHexVec.push_back( new string(fmtstr.str()));
        }
    } 
    
    char ch = 0;
    unsigned int code = 0;
    int  nRestBytesForCh = 0;
    size_t lead_idx = 0;
    size_t lead_row = 0;
    size_t lead_col = 0;
    unsigned int u32code = 0;
    
    string singleCharacter;
    int last_EOF_flag    = 0; // 0: undefined or    the last character is not EOF ,  1:Unix 2:Win 3:Mac , will never be 4(HYBIRD) ,because every line has a specific and unambiguous EOF
    
    int nRow = 1;
    int nCol = 1;
    
    // isCurrentLetter_line1stLetter : indicate that the current letter is the 1st letter of a new line
    bool isCurrentLetter_line1stLetter = false;
	string hexCode;

    for ( size_t idx = 0; idx < szOfFile; ++idx )
    {
        if( fInfo.isBomFile && idx < SIZE_BOM ) {
            continue;
        }

        // Read Character and calc it's decimal code
        ch = fileContentBuf[idx];
        code = (FULL_BITS & ch);

        // covert it to  Hex-Code and then Push to Array
        fmtstr.clear();
        fmtstr.str("");
        fmtstr << std::hex << std::uppercase << setw(2) << setfill('0') << code;

		// Push
		string* pFmtHexCode = new string(fmtstr.str() );
		hexCode = *pFmtHexCode;
        fInfo.rawHexVec.push_back(pFmtHexCode);
        
        if ( !fInfo.isBinary ) {
            if ( nRestBytesForCh == 0 ) {
                int current_EOF_flag = 0;
                bool isASCiiCode = ((code >> 7) == 0);    // 1st bit is 1 or 0
                if ( isASCiiCode ) {
                    // ASCII byte
                    nRestBytesForCh = 0;    // 1-1 = 0
                    singleCharacter = "";
                    // E_SPECIAL_CHAR_TAG curCharTagFlag = E_SPECIAL_CHAR_TAG::E_OTHER;
                    // fInfo.isBinary = !isCanInputChar(ch, &singleCharacter, &curCharTagFlag);
                    fInfo.isBinary = !isCanInputChar(ch);
                    
                    if ( !fInfo.isBinary ) {
                        if ( ch == G_CHAR_TAB.ch || ch == G_CHAR_GN.ch || ch == G_CHAR_GR.ch ) {
                            // Current is Special Character , must be one of the following 3   [ \t  \r \n ]
                            switch ( ch )
                            {
                            case '\t': // Current is <Tab>
                                {
                                    //////////////////////////////////////////////////
                                    //      current letter is <Tab>(\t)
                                    //////////////////////////////////////////////////
                                    if ( fInfo.endlineMode == FileInfo::E_UNKNOWN_ENDLINE ) {
                                        // do-nothing
                                    } else {
                                        // End-Line-Mode has been set already
                                        if ( last_EOF_flag != 0 ) {
                                            isCurrentLetter_line1stLetter = true;
                                        } else {
                                            // last_EOF_flag == 0 ,   do nothing ,  the previous letter is not EOF
                                            // maybe  a\t
                                        }
                                    }
                                }
                                break;
                            case '\n': // Current is <\n>
                                {
                                    //////////////////////////////////////////////////
                                    //      current letter is \n
                                    //////////////////////////////////////////////////
                                    // \n not take into account
                                    if ( fInfo.endlineMode == FileInfo::E_UNKNOWN_ENDLINE ) {
                                        fInfo.endlineMode = FileInfo::E_UNIX;
                                        current_EOF_flag = 1; // 1 means current is Unix EOF mode
                                    } else {
                                        // End-Line-Mode has been set already
                                        // ============= Core conception =============
                                        // !!! no need to check !!! whether the previous letter is \r or not 
                                        // because check \r\n at the case E_SPECIAL_CHAR_TAG::E_SR
                                        // ============= Core conception =============
                                        if ( fInfo.endlineMode != FileInfo::E_UNIX ) {
                                            fInfo.endlineMode = FileInfo::E_HYBRID;
                                        } else {
                                            // last EOF mode is E_UNIX , keep it 
                                        }
                                        
                                        if( last_EOF_flag != 0 ) {
                                            // previous is EOF : 
                                            // maybe : 
                                            //         \n
                                            //         \n   <== current letter
                                            /////////////////////////////
                                            //         \r\n
                                            //         \n   <== current letter
                                            /////////////////////////////
                                            isCurrentLetter_line1stLetter = true;
                                        }
                                        current_EOF_flag = 1; // 1 means current is Unix EOF mode
                                    }
                                }
                                break;
                            case '\r': // Current is <\r>
                                {
                                    //////////////////////////////////////////////////
                                    //      current letter is \r       !!!!! Complex Logic !!!!!
                                    //////////////////////////////////////////////////
                                    size_t nextIdx = idx + 1;
                                    if( fInfo.endlineMode == FileInfo::E_UNKNOWN_ENDLINE ) {
                                        if ( nextIdx < szOfFile ) {
                                            if ( fileContentBuf[nextIdx] == G_CHAR_GN.ch ) { // next letter is \n
                                                // so 1st \r\n
                                                fInfo.endlineMode = FileInfo::E_WIN;
                                                ++idx;  // Skip  \n , next for-loop will pointer to the char next to (\r\n)
                                                current_EOF_flag = 2;  // 2 means current is Win EOF mode
                                            } else {
                                                // the next letter is not \n , so not  \r\n
                                                // e.g.
                                                //          \r
                                                //          a
                                                fInfo.endlineMode = FileInfo::E_MAC;
                                                current_EOF_flag = 3;  // 3 means current is Mac EOF mode
                                            }
                                        } else {
                                            // the current idx is the !!!<Last>!!! letter of the file, 
                                            // a Very-Very-Very Special Logic
                                            fInfo.endlineMode = FileInfo::E_MAC;
                                            current_EOF_flag = 3; // 3 means current is Mac EOF mode
                                        }
                                    } else {
                                        // End-Line-Mode  has been set already
                                        // Must check the next letter is \n or not , and then Skip (\r\n) if necessary
                                        if ( nextIdx < szOfFile ) {
                                            if ( fileContentBuf[nextIdx] == G_CHAR_GN.ch ) { 
                                                // \r\n
                                                // treat as EOF WIN , check is HYBIRD ???
                                                if( fInfo.endlineMode != FileInfo::E_WIN ) {
                                                    // treat as WIN , but previous EOF mode is not win 
                                                    fInfo.endlineMode = FileInfo::E_HYBRID;
                                                } else {
                                                    // last EOF mode is E_WIN , keep it
                                                }

                                                // special logic     
                                                // e.g. 
                                                //       \r\n
                                                //       \r\n
                                                // Core , is there some bug ???
                                                if( last_EOF_flag != 0 ) {
                                                    isCurrentLetter_line1stLetter = true;
                                                }

                                                ++idx;  // !!! Do Skip \n !!!, next for-loop will pointer to the char next to (\r\n)
                                                current_EOF_flag = 2; // 2 means current is Win EOF mode
                                            } else {
                                                // treat as EOF MAC , check is HYBIRD or not
                                                // current is \r , but the next letter is valid but not \n
                                                if ( fInfo.endlineMode != FileInfo::E_MAC ) {
                                                    fInfo.endlineMode = FileInfo::E_HYBRID;
                                                } else {
                                                    // last EOF mode is E_MAC , keep it
                                                }

                                                if( last_EOF_flag != 0 ) {
                                                    isCurrentLetter_line1stLetter = true;
                                                }
                                                current_EOF_flag = 3; // 3 means current is EOF is Mac
                                            }
                                        } else {
                                            // \r is the last letter in the file
                                            if ( fInfo.endlineMode != FileInfo::E_MAC ) {
                                                fInfo.endlineMode = FileInfo::E_HYBRID;
                                            }

                                            if( last_EOF_flag != 0 ) {
                                                isCurrentLetter_line1stLetter = true;
                                            }
                                            current_EOF_flag = 3; // 3 means current is MAC EOF mode
                                        }
                                    }
                                }
                                break;
                            default:
                                break;
                            }
                        } else {
                            //
                            //  current letter is the one that  excurisive [ \t \r \n ]
                            //  tagFlag == E_SPECIAL_CHAR_TAG::E_OTHER    |      32 ~   126 or 127
                            //
                            if( last_EOF_flag != 0 ) {
                                isCurrentLetter_line1stLetter = true;
                            }
                        }
                    } else {
                        // Ascii code can't be input , so file is binary : Do Nothing
                        lead_idx = idx; lead_row = nRow; lead_col = nCol;

						fInfo.firstBinaryByte_Idx = lead_idx;
						fInfo.firstBinaryByte_Row = lead_row;
						fInfo.firstBinaryByte_Col = lead_col;
                    }
                } else {
                    //    nRestBytesForCh == 0
                    // <Non-Ascii 1st Lead> Byte   , the 1st Lead multi-byte   , multi bytes ???
                    // Calculate  How Many bytes in it
                    singleCharacter = "";
                    u32code = 0;
                    if ( (code >> 5) == multiBytesStartTag[0] ) {
                        nRestBytesForCh = 1;    // 2 - 1 = 1
                        lead_idx = idx; lead_row = nRow; lead_col = nCol;
                        
                        singleCharacter.push_back(ch);
                        u32code = (code & (FULL_BITS >> 3));
                    } else if ( (code >> 4) == multiBytesStartTag[1] ) {
                        nRestBytesForCh = 2;    // 3 - 1 = 2
                        lead_idx = idx; lead_row = nRow; lead_col = nCol;
                        
                        singleCharacter.push_back(ch);
                        u32code = (code & (FULL_BITS >> 4));
                    } else if ( (code >> 3) == multiBytesStartTag[2] ) {
                        nRestBytesForCh = 3;    // 4 - 1 = 3
                        lead_idx = idx; lead_row = nRow; lead_col = nCol;
                        
                        singleCharacter.push_back(ch);
                        u32code = (code & (FULL_BITS >> 5));
                    } else {
                        fInfo.isBinary = true;
                        
                        fInfo.firstBinaryByte_Idx = idx;
                        fInfo.firstBinaryByte_Row = nRow;
                        fInfo.firstBinaryByte_Col = nCol;
                    }
                    
                    if( last_EOF_flag != 0 ) {
                        isCurrentLetter_line1stLetter = true;
                    }
                }


                if( !fInfo.isBinary )  {
                    if( isCurrentLetter_line1stLetter ) {
                        ++nRow; nCol = 1;

                        // alloc a new line
                        list<SingleCharacter*>* pNewline = new list<SingleCharacter*>();
                        fInfo.lineVec.push_back( make_pair(pNewline,nRow) );

                        if( isASCiiCode ) {
							auto plineDisplayVec = fInfo.lineVec.back().first;
                            if( plineDisplayVec!=nullptr ) {
                                if ( current_EOF_flag !=0 ) {
                                    // this line is a line without any content , only <Enter> at the end
									// auto emtpySCobj = SingleCharacter::generate_empty_Character(nRow, nCol);
                                    // plineDisplayVec->push_back( emtpySCobj );
									auto eol_SCObj = SingleCharacter::generate_ASCII_Character(ch, nRow, nCol, current_EOF_flag);
                                    plineDisplayVec->push_back( eol_SCObj );

									// Push addtional \n after \n if WIN EOL mode
									if ( current_EOF_flag == static_cast<int>(FileInfo::E_WIN)  ) {
										// The EOL of Win is \r\n , push \n after \r
										auto eol_SCObj_win = SingleCharacter::generate_ASCII_Character(G_CHAR_GN.ch, nRow, (nCol+1), current_EOF_flag);
										plineDisplayVec->push_back( eol_SCObj_win );
									}

									// do not increase nCol
									//  ++nCol;
                                } else {
									// head's  non normal asii letter
									auto normal_asii_obj = SingleCharacter::generate_ASCII_Character(ch, nRow, nCol, current_EOF_flag);
                                    plineDisplayVec->push_back( normal_asii_obj);

									++nCol;
                                }
                            }

							if( current_EOF_flag == 0 ) {
								++fInfo.characterCount;
							}
                        } else {
                            // Non Ansi  , wait for the UTF-8 Parse finished and then push
                        }

                        isCurrentLetter_line1stLetter = false;  // must unset flag
                    } else {
                        // not new-line
                        if( isASCiiCode ) {
							auto plineDisplayVec = fInfo.lineVec.back().first;
                            if( plineDisplayVec!=nullptr ) {
								auto normal_asii_obj = SingleCharacter::generate_ASCII_Character(ch, nRow, nCol, current_EOF_flag);
								plineDisplayVec->push_back( normal_asii_obj );

								// Push addtional \n after \n if WIN EOL mode
								if ( current_EOF_flag == static_cast<int>(FileInfo::E_WIN)  ) {
									// The EOL of Win is \r\n , push \n after \r
									auto eol_SCObj_win = SingleCharacter::generate_ASCII_Character(G_CHAR_GN.ch, nRow, (nCol+1), current_EOF_flag);
									plineDisplayVec->push_back( eol_SCObj_win );
								}
                            }

							if( current_EOF_flag == 0 ) {
								++fInfo.characterCount;
								++nCol;
							}
                        } else {
                            // Non Ansi  , wait for the following UTF-8 bytes Parse finished and then push
                        }
                    }
                }

                // save as the last EOF flag
                last_EOF_flag = current_EOF_flag;
            } else {
                //
                // nRestBytesForCh!=0   : Multi-Bytes Unicode Character       maybe == 1 or 2 or 3
                //
                // Check  :   the bytes after the leader bytes must be like the following style    10XX,XXXX
                
                if ( (code >> 6) == multiBytesStartTag[3] ) {
                    --nRestBytesForCh;
                    // singleCharacter += ch;
                    singleCharacter.push_back(ch);
                    u32code <<= 6;
                    u32code |= (code & (FULL_BITS >> 2));
                    if ( nRestBytesForCh == 0 ) {
                        //
                        // MultiByte Character is assembled
                        //
                        auto plineDisplayVec = fInfo.lineVec.back().first;
                        if( plineDisplayVec!=nullptr ) {
                            if( isCurrentLetter_line1stLetter ) {
                                ++nRow; nCol = 1;
                                isCurrentLetter_line1stLetter = false;
                            }

							auto multi_character = SingleCharacter::generate_MultiBytes_Character(singleCharacter, u32code, nRow, nCol);
							plineDisplayVec->push_back( multi_character );
						}

                        // ++nCol;
						nCol += C_N_WIDTH_4_MULTI_BYTES_CHARACTER_USED;
                        ++fInfo.characterCount;
                    }
                } else {
                    fInfo.isBinary = true;
                    
                    fInfo.firstBinaryByte_Idx = idx;
                    fInfo.firstBinaryByte_Row = nRow;
                    fInfo.firstBinaryByte_Col = nCol;
                }
            }
        } else {
            // Binary File , do-not do the following  parse
        }
    } // end for
    
    if ( !fInfo.isBinary   &&  nRestBytesForCh > 0 ) {
        // the MultiBytes Unicode Character is !!!<Not>!!! an <Entire> Character , there must be some missed byte(s)
        fInfo.isBinary = true;
        
        fInfo.firstBinaryByte_Idx = lead_idx;
        fInfo.firstBinaryByte_Row = lead_row;
        fInfo.firstBinaryByte_Col = lead_col;

        // fInfo.lineCnt = 1;  // ???
    } else {
        fInfo.lineCnt = fInfo.lineVec.size();
    }

    if( fileContentBuf!=nullptr ) {
        delete [] fileContentBuf;
        fileContentBuf = nullptr;
    }

    return true;
}

// do <NOT> skip any letter , scan from the head to the tail
bool readFile_V1(const string& filename, FileInfo& fInfo)
{
    ifstream file(filename.c_str(), ios::in | ios::binary);
    fInfo.filename = filename;
    if ( !file ) {
        fInfo.lineCnt = 0;
        if( SHOULD_LOG_OPEN_FAILED ){ 
            cout << "Sorry , Can't open file : " << filename << endl;
        }
        return false;
    }
    fInfo.isValid = true;
    
	bool treatEOFAsByte = true;
	bool treatRNAs1Byte = true;
    char* fileContentBuf = nullptr;
    size_t szOfFile = 0;
    file.seekg(0, ios::end);
    szOfFile = static_cast<size_t>(file.tellg());
    file.seekg(0, ios::beg);
    fInfo.bytesCount = szOfFile;
    
    if ( szOfFile == 0 ) {
        if( SHOULD_LOG_EMPTY_FILE ){ 
            cout << "The File named  \"" << filename << "\" has 0 byte , It's an empty file. Nothing to read !" << endl;
        }

        fInfo.lineCnt = 0;
        return true;
    } else {
        // alloc and read all byte from file and fill into Buf-Array
        fileContentBuf = new char[szOfFile];
        file.read(fileContentBuf, szOfFile);
    }
    
    if ( szOfFile >= SIZE_BOM ) {
        auto equalFlag = true;
        for (size_t i = 0; i < SIZE_BOM; ++i) {
            unsigned int code = char2HexCode(fileContentBuf[i], nullptr);
            if ( code != utf8_Bom_tag[i] ) {
                equalFlag = false;
                break;
            }
        }
        fInfo.isBomFile = equalFlag;
    } else {
        // bytes no more than SIZE_BOM
        fInfo.isBomFile = false;
    }
    
    
    stringstream fmtstr;
    if ( fInfo.isBomFile ) {
        // realszOfFile = szOfFile - SIZE_BOM;
        for ( size_t i = 0; i < SIZE_BOM; ++i ) {
            unsigned int code = utf8_Bom_tag[i];
            fmtstr.clear();
            fmtstr.str("");
            fmtstr << std::hex << std::uppercase << setw(2) << setfill('0') << code;
            
            fInfo.rawHexVec.push_back( new string(fmtstr.str()));
        }
    } 
    
    char ch = 0;
    unsigned int code = 0;
    size_t lead_idx = 0;
    size_t lead_row = 0;
    size_t lead_col = 0;
    unsigned int u32code = 0;
    
    string singleCharacter;
    int last_EOF_flag    = 0; // 0: undefined or    the last character is not EOF ,  1:Unix 2:Win 3:Mac , will never be 4(HYBIRD) ,because every line has a specific and unambiguous EOF
    
    int nRow = 1;
    int nCol = 1;
    
	string hexCode;

    for ( size_t idx = 0; idx < szOfFile; ++idx )
    {
        if( fInfo.isBomFile && idx < SIZE_BOM ) {
            continue;
        }

        // Read Character and calc it's decimal code
        ch = fileContentBuf[idx];
        code = char2HexCode(ch, nullptr);
        // covert it to  Hex-Code and then Push to Array
        fmtstr.clear();
        fmtstr.str("");
        fmtstr << std::hex << std::uppercase << setw(2) << setfill('0') << code;
		// Push
		string* pFmtHexCode = new string(fmtstr.str() );
		hexCode = *pFmtHexCode;
        fInfo.rawHexVec.push_back(pFmtHexCode);

		int current_EOF_flag = 0;

		if ( !fInfo.isBinary ) {
			bool isASCiiCode = ((code >> 7) == 0);    // 1st bit is 1 or 0
			if ( isASCiiCode ) {
				// ASCII byte
				singleCharacter.clear();
				auto canInputAscii = isCanInputChar(ch);
				if( canInputAscii ) {
					if ( last_EOF_flag != 0 ) {
						++nRow; nCol = 1;

                        list<SingleCharacter*>* pNewline = new list<SingleCharacter*>();
                        fInfo.lineVec.push_back( make_pair(pNewline,nRow) );
					}

					auto plineDisplayVec = fInfo.lineVec.back().first;
					if ( ch== G_CHAR_GR.ch || ch == G_CHAR_GN.ch ) {
						if( ch == G_CHAR_GR.ch ) {
							// \r
							if( (idx + 1) < szOfFile ) {
								if( fileContentBuf[idx+1] == G_CHAR_GN.ch ) {
									// so \r\n
									++idx;	// must skip \n , read next
									current_EOF_flag = 2;	// Win EOL mode
									// treat \r\n as 1 or 2 EOL byte(s) ? 
									if ( treatEOFAsByte ) {
										fInfo.characterCount += (treatRNAs1Byte ? 1 : 2);
									}
								} else {
									current_EOF_flag = 3;	// Mac EOL mode

									if ( treatEOFAsByte ) {
										fInfo.characterCount += 1;
									}
								}
							} else {
								// \r is the last letter of the file content
								current_EOF_flag = 3;	// Mac EOL mode and this is the last letter
								if ( treatEOFAsByte ) {
									fInfo.characterCount += 1;
								}
							}
						} else {
							// \n
							current_EOF_flag = 1;	// Unix EOL mode

								if ( treatEOFAsByte ) {
									fInfo.characterCount += 1;
								}
						}

						// decide the file EOF mode 
						if( fInfo.endlineMode == FileInfo::E_UNKNOWN_ENDLINE ) {
							fInfo.endlineMode = static_cast<FileInfo::E_ENDLINE_MODE>(current_EOF_flag);
						} else if( static_cast<int>(fInfo.endlineMode) != current_EOF_flag ) {
							fInfo.hybridLineNo = nRow;
							fInfo.endlineMode = FileInfo::E_HYBRID;
						}

						auto eol_SCObj = SingleCharacter::generate_ASCII_Character(ch, nRow, nCol, current_EOF_flag);
						if( plineDisplayVec!=nullptr ) {
							plineDisplayVec->push_back( eol_SCObj );
							if ( current_EOF_flag == static_cast<int>(FileInfo::E_WIN)  ) {
								// The EOL of Win is \r\n , push \n after \r
								auto eol_SCObj_win = SingleCharacter::generate_ASCII_Character(G_CHAR_GN.ch, nRow, (nCol+1), current_EOF_flag);
								plineDisplayVec->push_back( eol_SCObj_win );
							}
						}

						// meet \r\n do not increase  characterCount  and  nCol
					} else {
						// normal ANSI letter 
						auto normal_asii_obj = SingleCharacter::generate_ASCII_Character(ch, nRow, nCol, current_EOF_flag);
						if( plineDisplayVec!=nullptr ) {
							plineDisplayVec->push_back( normal_asii_obj );
						}

						++fInfo.characterCount;
						++nCol;
					}
				} else {
					fInfo.isBinary = true;
					lead_idx = idx; lead_row = nRow; lead_col = nCol;
				}
			} else {
				if( last_EOF_flag != 0 ) {
					++nRow; nCol = 1;

					list<SingleCharacter*>* pNewline = new list<SingleCharacter*>();
					fInfo.lineVec.push_back( make_pair(pNewline,nRow) );
				}

				auto plineDisplayVec = fInfo.lineVec.back().first;
				// <Non-Ascii 1st Lead> Byte   , the 1st Lead multi-byte   , multi bytes ???
				// Calculate  How Many bytes in it
				singleCharacter.clear(); 
				u32code = 0;
				bool errorFlag = false;
				SingleCharacter* pMultiBytesCharacter = nullptr;
				if ( (code >> 5) == multiBytesStartTag[0] ) {
					// 2-bytes make a non-Ansi-Character
					if ( (idx + 1) < szOfFile ) {
						size_t next_2_2 = idx + 1;
						char char_2_2 = fileContentBuf[next_2_2];
						unsigned int code_2_2 = char2HexCode(char_2_2, nullptr);
						singleCharacter.push_back(ch);

						if( (code_2_2 >> 6) == multiBytesStartTag[3] ) {
							singleCharacter.push_back(char_2_2);
							// set the 1st byte
							u32code = (code & (FULL_BITS >> 3));
							// set the 2nd byte
							u32code <<= 6;
							u32code |= (code_2_2 & (FULL_BITS >> 2));	

							pMultiBytesCharacter = SingleCharacter::generate_MultiBytes_Character(singleCharacter, u32code, nRow, nCol);

							idx += 1;	// skip the 2nd byte , read the next byte after 2nd byte
							nCol += C_N_WIDTH_4_MULTI_BYTES_CHARACTER_USED;
							++fInfo.characterCount;
						} else {
							errorFlag = true;
						}
					} else {
						// only the lead byte , at the last 
						errorFlag = true;
					}
				} else if ( (code >> 4) == multiBytesStartTag[1] ) {
					// 3-bytes make a non-Ansi-Character
					if( (idx + 2) < szOfFile ) {
						size_t idx_next_23[2]  = { (idx+1),  (idx+2)   };
						char   char_next_23[2] = { fileContentBuf[idx_next_23[0]],   fileContentBuf[idx_next_23[1]]   };
						unsigned int code_next_23[2] = {   char2HexCode(char_next_23[0], nullptr),    char2HexCode(char_next_23[1], nullptr)   };

						if (     (code_next_23[0] >> 6) == multiBytesStartTag[3]     
							 &&  (code_next_23[1] >> 6) == multiBytesStartTag[3] 
						   ) 
						{
							singleCharacter.push_back(ch);
							singleCharacter.push_back( char_next_23[0] );
							singleCharacter.push_back( char_next_23[1] );

							// set the 1st byte
							u32code = (code & (FULL_BITS >> 4));
							// set the 2nd byte
							u32code <<= 6;
							u32code |= (code_next_23[0] & (FULL_BITS >> 2));	
							// set the 3rd byte
							u32code <<= 6;
							u32code |= (code_next_23[1] & (FULL_BITS >> 2));	

							pMultiBytesCharacter = SingleCharacter::generate_MultiBytes_Character(singleCharacter, u32code, nRow, nCol);

							idx += 2;
							nCol += C_N_WIDTH_4_MULTI_BYTES_CHARACTER_USED;
							++fInfo.characterCount;
						} else {
							errorFlag = true;
						}
					} else {
						errorFlag = true;
					}
					
				} else if ( (code >> 3) == multiBytesStartTag[2] ) {
					// 4-bytes make a non-Ansi-Character
					if( (idx + 3) < szOfFile ) {
						size_t idx_next_234[3]  = { (idx+1),  (idx+2), (idx+3)  };
						char   char_next_234[3] = { fileContentBuf[idx_next_234[0]],   fileContentBuf[idx_next_234[1]],    fileContentBuf[idx_next_234[2]]   };
						unsigned int code_next_234[3] = { char2HexCode(char_next_234[0], nullptr),    char2HexCode(char_next_234[1], nullptr),      char2HexCode(char_next_234[2], nullptr)   };

						if (     (code_next_234[0] >> 6) == multiBytesStartTag[3]     
							 &&  (code_next_234[1] >> 6) == multiBytesStartTag[3]     
							 &&  (code_next_234[2] >> 6) == multiBytesStartTag[3]  
						   ) 
						{
							singleCharacter.push_back(ch);
							singleCharacter.push_back( char_next_234[0] );
							singleCharacter.push_back( char_next_234[1] );
							singleCharacter.push_back( char_next_234[2] );

							// set the 1st byte
							u32code = (code & (FULL_BITS >> 5));
							// set the 2nd byte
							u32code <<= 6;
							u32code |= (code_next_234[0] & (FULL_BITS >> 2));	
							// set the 3rd byte
							u32code <<= 6;
							u32code |= (code_next_234[1] & (FULL_BITS >> 2));	
							// set the 4th byte
							u32code <<= 6;
							u32code |= (code_next_234[2] & (FULL_BITS >> 2));	

							pMultiBytesCharacter = SingleCharacter::generate_MultiBytes_Character(singleCharacter, u32code, nRow, nCol);

							idx += 3;
							nCol += C_N_WIDTH_4_MULTI_BYTES_CHARACTER_USED;
							++fInfo.characterCount;
						} else {
							errorFlag = true;
						}
					} else {
						errorFlag = true;
					}
				} else {
					errorFlag = true;
				}

				if ( errorFlag ) {
					lead_idx = idx; lead_row = nRow; lead_col = nCol;
					fInfo.isBinary = true;
				} else {
					if ( plineDisplayVec!=nullptr  &&   pMultiBytesCharacter!=nullptr  ) {
						plineDisplayVec->push_back( pMultiBytesCharacter);
					}
				}
			}

			// save as the last EOF flag
			last_EOF_flag = current_EOF_flag;
		}

    } // end for
    
    if (  fInfo.isBinary  ) {
        fInfo.firstBinaryByte_Idx = lead_idx;
        fInfo.firstBinaryByte_Row = lead_row;
        fInfo.firstBinaryByte_Col = lead_col;
    } else {
        fInfo.lineCnt = fInfo.lineVec.size();
    }

    if( fileContentBuf!=nullptr ) {
        delete [] fileContentBuf;
        fileContentBuf = nullptr;
    }

    return true;
}


// stop scanning bytes after a certain bytes has been treat as binary flag
// collect only Multi-Bytes Character
bool readFileRapidly(const string& filename, FileInfo& fInfo)
{
    ifstream file(filename.c_str(), ios::in | ios::binary);
    fInfo.filename = filename;
    if ( !file ) {
		fInfo.lineCnt = 0;
        if( SHOULD_LOG_OPEN_FAILED ) {
            cout << "Sorry , Can't open file : " << filename << endl;
        }
        return false;
    }
    fInfo.isValid = true;
    
    char* fileContentBuf = nullptr;
    size_t szOfFile = 0;
    file.seekg(0, ios::end);
    szOfFile = static_cast<size_t>(file.tellg());
    file.seekg(0, ios::beg);
    fInfo.bytesCount = szOfFile;
    
    if ( szOfFile == 0 ) {
        if( SHOULD_LOG_EMPTY_FILE ){ 
            cout << "The File named  \"" << filename << "\" has 0 byte , It's an empty file. Nothing to read !" << endl;
        }

        fInfo.lineCnt = 0;
        return true;
    } else {
        // alloc and read all byte from file and fill into Buf-Array
        fileContentBuf = new char[szOfFile];
        file.read(fileContentBuf, szOfFile);
    }
    
    if ( szOfFile >= SIZE_BOM ) {
        auto equal_3_Flag = true;
        for (size_t i = 0; i < SIZE_BOM; ++i) {
            unsigned int code = char2HexCode(fileContentBuf[i] , nullptr);
            if ( code != utf8_Bom_tag[i] ) {
                equal_3_Flag = false;
                break;
            }
        }
        fInfo.isBomFile = equal_3_Flag;
    } else {
        // bytes no more than SIZE_BOM
        fInfo.isBomFile = false;
    }
    
    char ch = 0;
    unsigned int code = 0;
    size_t lead_idx = 0;
    size_t lead_row = 0;
    size_t lead_col = 0;
    unsigned int u32code = 0;
    
    string singleCharacter;
    int last_EOF_flag    = 0; // 0: undefined or    the last character is not EOF ,  1:Unix 2:Win 3:Mac , will never be 4(HYBIRD) ,because every line has a specific and unambiguous EOF
    
    int nRow = 1;
    int nCol = 1;
    
    for ( size_t idx = 0; idx < szOfFile; ++idx )
    {
        if( fInfo.isBomFile && idx < SIZE_BOM ) {
            continue;
        }

        // Read Character and calc it's decimal code
        ch = fileContentBuf[idx];
        code = char2HexCode(ch, nullptr);

		int current_EOF_flag = 0;
		bool isASCiiCode = ((code >> 7) == 0);    // 1st bit is 1 or 0
		if ( isASCiiCode ) {
			// ASCII byte
			singleCharacter.clear();
			auto canInputAscii = isCanInputChar(ch);

			if( !canInputAscii ) {
				fInfo.isBinary = true;
				lead_idx = idx; lead_row = nRow; lead_col = nCol;
				break;
			}
			
			if ( last_EOF_flag != 0 ) {
				++nRow; nCol = 1;
			}

			if ( ch == G_CHAR_GR.ch  || ch == G_CHAR_GN.ch ) {
				if( ch == G_CHAR_GR.ch ) {
					// \r
					if( (idx + 1) < szOfFile ) {
						if( fileContentBuf[idx+1] == G_CHAR_GN.ch ) {
							// so \r\n
							++idx;	// must skip \n , read next
							current_EOF_flag = 2;	// Win EOL mode
						} else {
							current_EOF_flag = 3;	// Mac EOL mode
						}
					} else {
						// \r is the last letter of the file content
						current_EOF_flag = 3;	// Mac EOL mode and this is the last letter
					}
				} else {
					// \n
					current_EOF_flag = 1;	// Unix EOL mode
				}

				// decide the file EOF mode 
				if( fInfo.endlineMode == FileInfo::E_UNKNOWN_ENDLINE ) {
					fInfo.endlineMode = static_cast<FileInfo::E_ENDLINE_MODE>(current_EOF_flag);
				} else if( static_cast<int>(fInfo.endlineMode) != current_EOF_flag ) {
					fInfo.endlineMode = FileInfo::E_HYBRID;
					fInfo.hybridLineNo = nRow;
				}
			} else {
				// normal ANSI letter
				++nCol;
			}

		} else {
			if( last_EOF_flag != 0 ) {
				++nRow; nCol = 1;
			}

			//    nRestBytesForCh == 0
			// <Non-Ascii 1st Lead> Byte   , the 1st Lead multi-byte   , multi bytes ???
			// Calculate  How Many bytes in it
			singleCharacter.clear(); 
			u32code = 0;
			bool errorFlag = false;
			SingleCharacter* pMultiBytesCharacter = nullptr;
			if ( (code >> 5) == multiBytesStartTag[0] ) {
				// 2-bytes make a non-Ansi-Character
				if ( (idx + 1) < szOfFile ) {
					size_t next_2_2 = idx + 1;
					char char_2_2 = fileContentBuf[next_2_2];
					unsigned int code_2_2 = char2HexCode(char_2_2, nullptr);
					singleCharacter.push_back(ch);

					if( (code_2_2 >> 6) == multiBytesStartTag[3] ) {
						singleCharacter.push_back(char_2_2);
						// set the 1st byte
						u32code = (code & (FULL_BITS >> 3));
						// set the 2nd byte
						u32code <<= 6;
						u32code |= (code_2_2 & (FULL_BITS >> 2));	

						pMultiBytesCharacter = SingleCharacter::generate_MultiBytes_Character(singleCharacter, u32code, nRow, nCol);

						++idx;	// skip the 2nd byte , read the next byte after 2nd byte
						nCol += C_N_WIDTH_4_MULTI_BYTES_CHARACTER_USED;
					} else {
						errorFlag = true;
					}
				} else {
					errorFlag = true;
				}
			} else if ( (code >> 4) == multiBytesStartTag[1] ) {
				// 3-bytes make a non-Ansi-Character
				if( (idx + 2) < szOfFile ) {
					size_t idx_next_23[2]  = { (idx+1),  (idx+2)   };
					char   char_next_23[2] = { fileContentBuf[idx_next_23[0]],   fileContentBuf[idx_next_23[1]]   };
					unsigned int code_next_23[2] = {   char2HexCode(char_next_23[0], nullptr),    char2HexCode(char_next_23[1], nullptr)   };

					if (     (code_next_23[0] >> 6) == multiBytesStartTag[3]     
						 &&  (code_next_23[1] >> 6) == multiBytesStartTag[3] 
					   ) 
					{
						singleCharacter.push_back(ch);
						singleCharacter.push_back( char_next_23[0] );
						singleCharacter.push_back( char_next_23[1] );

						// set the 1st byte
						u32code = (code & (FULL_BITS >> 4));
						// set the 2nd byte
						u32code <<= 6;
						u32code |= (code_next_23[0] & (FULL_BITS >> 2));	
						// set the 3rd byte
						u32code <<= 6;
						u32code |= (code_next_23[1] & (FULL_BITS >> 2));	

						pMultiBytesCharacter = SingleCharacter::generate_MultiBytes_Character(singleCharacter, u32code, nRow, nCol);

						idx += 2;
						nCol += C_N_WIDTH_4_MULTI_BYTES_CHARACTER_USED;
					} else {
						errorFlag = true;
					}

				} else {
					errorFlag = true;
				}
			} else if ( (code >> 3) == multiBytesStartTag[2] ) {
				// 4-bytes make a non-Ansi-Character
				if( (idx + 3) < szOfFile ) {
					size_t idx_next_234[3]  = { (idx+1),  (idx+2), (idx+3)  };
					char   char_next_234[3] = { fileContentBuf[idx_next_234[0]],   fileContentBuf[idx_next_234[1]],    fileContentBuf[idx_next_234[2]]   };
					unsigned int code_next_234[3] = { char2HexCode(char_next_234[0], nullptr),    char2HexCode(char_next_234[1], nullptr),      char2HexCode(char_next_234[2], nullptr)   };

					if (     (code_next_234[0] >> 6) == multiBytesStartTag[3]     
						 &&  (code_next_234[1] >> 6) == multiBytesStartTag[3]     
						 &&  (code_next_234[2] >> 6) == multiBytesStartTag[3]  
					   ) 
					{

						singleCharacter.push_back(ch);
						singleCharacter.push_back( char_next_234[0] );
						singleCharacter.push_back( char_next_234[1] );
						singleCharacter.push_back( char_next_234[2] );

						// set the 1st byte
						u32code = (code & (FULL_BITS >> 5));
						// set the 2nd byte
						u32code <<= 6;
						u32code |= (code_next_234[0] & (FULL_BITS >> 2));	
						// set the 3rd byte
						u32code <<= 6;
						u32code |= (code_next_234[1] & (FULL_BITS >> 2));	
						// set the 4th byte
						u32code <<= 6;
						u32code |= (code_next_234[2] & (FULL_BITS >> 2));	

						pMultiBytesCharacter = SingleCharacter::generate_MultiBytes_Character(singleCharacter, u32code, nRow, nCol);

						idx += 3;
						nCol += C_N_WIDTH_4_MULTI_BYTES_CHARACTER_USED;
					} else {
						errorFlag = true;
					}
				} else {
					errorFlag = true;
				}
			} else {
				errorFlag = true;
			}


			if ( errorFlag ) {
				lead_idx = idx; lead_row = nRow; lead_col = nCol;
				fInfo.isBinary = true;
			} else {
				if( !fInfo.lineVec.empty() ) {
					auto& plineDisplayObj = fInfo.lineVec.back();
					auto plineDisplayVec = plineDisplayObj.first;
					auto lastLineNo = plineDisplayObj.second;

					if ( plineDisplayVec!=nullptr  &&   pMultiBytesCharacter!=nullptr  ) {
						int cur_character_nRow = pMultiBytesCharacter->nRow;
						if( cur_character_nRow!=lastLineNo ) {
							list<SingleCharacter*>* pNewline = new list<SingleCharacter*>();
							fInfo.lineVec.push_back( make_pair(pNewline,cur_character_nRow) );

							plineDisplayVec = fInfo.lineVec.back().first;
						} 

						if( plineDisplayVec!=nullptr ) {
							plineDisplayVec->push_back(pMultiBytesCharacter);
						}
					}
				}
			}

		}

		// save as the last EOF flag
		last_EOF_flag = current_EOF_flag;

		if( fInfo.isBinary )  {
			break;
		}
    } // end for


	if( fInfo.isBinary ) {
		fInfo.firstBinaryByte_Idx = lead_idx;
		fInfo.firstBinaryByte_Row = lead_row;
		fInfo.firstBinaryByte_Col = lead_col;
	}
    
    if( fileContentBuf!=nullptr ) {
        delete [] fileContentBuf;
        fileContentBuf = nullptr;
    }

    return true;
}






void printFileInfo(const FileInfo& fileInfo, string& retStr, bool needPrintToConsole /* = false */)
{
     stringstream outstr;
     if ( !fileInfo.isValid ) {
         outstr << "File named  :  " << fileInfo.filename << "  not existed or open failed" << endl;
         retStr = outstr.str();
         if ( needPrintToConsole ) {
             cout << retStr << endl;
         }
         return;
     }

     bool treatrnAsOneEOL = true; // treat \r\n as one character  , For Qt Use
     static const auto BINARY_WRAP = 16;
     outstr << "==================================================" << endl;
     outstr << "isBom = " << (fileInfo.isBomFile ? "true" : "false") << endl;
     outstr << "isBinary = " << (fileInfo.isBinary ? "true" : "false") << endl;
     outstr << "bytesCount = " << fileInfo.bytesCount << endl;
     outstr << "characterCount = " << fileInfo.characterCount << endl;
     string enum_str;
     switch (fileInfo.endlineMode)
     {
         case FileInfo::E_UNKNOWN_ENDLINE:
             enum_str = "E_UNKNOWN_ENDLINE";
             break;
         case FileInfo::E_WIN:
             enum_str = "E_WIN";
             break;
         case FileInfo::E_UNIX:
             enum_str = "E_UNIX";
             break;
         case FileInfo::E_MAC:
             enum_str = "E_MAC";
             break;
         case FileInfo::E_HYBRID:
             enum_str = "E_HYBRID";
             break;
         default:
             enum_str = "E_UNKNOWN_ENDLINE";
             break;
     }
     outstr << "endlineMode = " << enum_str << endl;
     outstr << "==================================================" << endl;

     if ( fileInfo.bytesCount == 0 ) {
         outstr << endl; 
         outstr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl << endl;
         outstr << "\t0 bytes in  this the file : " << fileInfo.filename << endl;
         outstr << "\tNothing to output " << endl << endl;
         outstr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl; 
         outstr << endl; 
	 } else {
		 if ( fileInfo.isBinary ) {
			 size_t i = 0;
			 for ( auto hexCode : fileInfo.rawHexVec ) {
				 if ( hexCode != nullptr ) {
					 outstr << "| " << (*hexCode) << " ";
					 ++i;

					 if ( i!=0   &&  (i % BINARY_WRAP) == 0 ) {
						 outstr << "| => " << std::dec << (i-BINARY_WRAP+1) << " ~ " << i << endl;
					 }
				 }
			 }

			 // outstr << "|";
			 size_t rest = (i % BINARY_WRAP);
			 if (  rest!= 0 ) {
				 if ( rest == 1 ) {
					 outstr << "| => " << std::dec << i;
				 } else {
					 outstr << "| => " << std::dec << (i - rest + 1) << " ~ " << i;
				 }
			 }
			 // otherwise it has already printed at previous for-loop
			 
			 outstr << endl;
		} else {
			if ( fileInfo.isBomFile ) {
				// Bom Header Part
				outstr << "=============== Bom Header ===============" << endl;
				size_t rawHexSz = fileInfo.rawHexVec.size();
				for ( size_t bomIdx = 0; bomIdx < SIZE_BOM; ++bomIdx ) {
					if ( bomIdx < rawHexSz ) {
						outstr << "| " << fileInfo.rawHexVec.at(bomIdx) << " ";
					}
				}
				outstr << "|" << endl;
				outstr << endl;
			}

			int nlinesBits = getDecimalBits(fileInfo.lineCnt);
			// the format like this : 
			//                                                                                                    <space>                     <space> 
			// "line : XXX "| a | b | c | d |  , so the count of the letter inside the quoto =  "line :".size() +    1      +    nlinesBits    + 1
			const string strlinefmt("line : ");
			int total_width =  static_cast<int>(strlinefmt.size())   +  nlinesBits + 1;

			// flag of print lineNo     front / tail ?
			size_t iCharacterCount = 0;
			auto lineLineAtFront =  true;
			for ( auto it = fileInfo.lineVec.begin(); it != fileInfo.lineVec.end(); ++it ) 
			{
				// vector< vector<SingleCharacter*>* > lineVec;
				auto singleLine = it->first;
				auto lineNo = it->second;

				if ( singleLine !=nullptr ) {
					//////////////////////////////////////////////////
					// For every single ling, There are 3 Steps to Do
					//////////////////////////////////////////////////
					// 1.  Displayed String 
					//
					if ( lineLineAtFront ) {
						outstr << setw(total_width) << setfill(' ') << "";
					}

					for ( auto pWord = singleLine->begin(); pWord != singleLine->end(); ++pWord ) 
					{
						auto pSingleCharacter = *pWord;
						if(  pSingleCharacter!=nullptr ) {
							switch ( pSingleCharacter->nBytes ) 
							{
							case 1:
								{
									if( pSingleCharacter->nEOF_Flag == 0 ) {
										// not		\r		\n		\r\n
										if ( pSingleCharacter->ansi_char == G_CHAR_TAB.ch || pSingleCharacter->ansi_char == G_CHAR_VTAB.ch ||  pSingleCharacter->ansi_char == G_CHAR_GNP.ch ) {
											outstr << "| " << pSingleCharacter->display_str << " "; // with one 1 space
										} else {
											outstr << "|  " << pSingleCharacter->display_str << " ";
										}
									} else {
										// \r \n
										outstr << "| " << pSingleCharacter->display_str << " "; 
									}
								}
								break;
							case 2:
								{
									 //  |   XX    |    , treat as 1 width
									 //  | 0A | 0B |
									 //  outstr << "|" << setw(4) << setfill(' ') << "" << (*pWord) << setw(4) << setfill(' ') << "";
									 outstr << "|" << setw(4) << setfill(' ') << "" << pSingleCharacter->display_str << setw(4) << setfill(' ') << "";
								}
								break;
							case 3:
								{
								   // |      ??      |  , treat as 2 width
								   // | 0A | 0B | 0C |
								   outstr << "|" << setw(6) << setfill(' ') << "" << pSingleCharacter->display_str << setw(6) << setfill(' ') << "";
								}
								break;
							case 4:
								{
									// |         ??        | , treat as 2 width
									// | 0A | 0B | 0C | 0D |
									outstr << "|" << setw(9) << setfill(' ') << "" << pSingleCharacter->display_str << setw(8) << setfill(' ') << "";
								}
								break;
							default:
								// assert() ???
								break;
							}
						}
					}
					outstr << "|";
					
					// Add Cursor Info for This Line and total Index
					if ( !singleLine->empty() ) {
						auto beforeSz = iCharacterCount + 1;
						auto sz = singleLine->size();
						iCharacterCount += sz;
						if ( treatrnAsOneEOL && sz >= 2 ) {
							// to calc character count for special logic
							auto itLastOne = singleLine->begin();
							auto itLastButOne = itLastOne;
							std::advance( itLastOne, singleLine->size() -1 );
							std::advance( itLastButOne , singleLine->size() -2 );

							auto lastOne = *itLastOne;
							auto lastButOne = *itLastButOne;
							if ( lastOne!=nullptr && lastButOne!=nullptr ) {
								if ( lastOne->display_str == G_CHAR_GN.displayStr &&  lastButOne->display_str == G_CHAR_GR.displayStr ) {
									// \r\n
									iCharacterCount -= 1;
								}
							}
						}
						outstr << " => " << std::dec << beforeSz << " ~ " << std::dec << iCharacterCount << ",Tot:" << std::dec << (iCharacterCount - beforeSz +1);
					}

					outstr << endl; // line displayed string is over  , press <Enter> to new-line

					// 2.  UTF-8 Byte(s) Array print
					if( lineLineAtFront ) {
						outstr << strlinefmt << setw(nlinesBits) << lineNo << " ";
					}

					for ( auto pWord = singleLine->cbegin(); pWord != singleLine->cend(); ++pWord ) 
					{
						auto pSingleCharacter = *pWord;
						if ( pSingleCharacter!=nullptr ) {
							// bool isWinEOF = false;
							for ( size_t j = 0; j < pSingleCharacter->nBytes; ++j ) {
								outstr << "| " << pSingleCharacter->utf8HexAry[j] << " ";
								// if ( pSingleCharacter->nBytes==1    &&     pSingleCharacter->nEOF_Flag == FileInfo::E_WIN ) {
								//     isWinEOF = true;
								// }
							}

							// if( pSingleCharacter!=nullptr &&  isWinEOF ) {
							//     outstr << "| " << pSingleCharacter->utf8HexAry[1] << " ";
							// }
						}
					}
					outstr << "|";  // print line tail flag
					if( !lineLineAtFront ) {
						outstr << " : line "<< lineNo;
					}



					outstr << endl; // line utf-8 string is over  , press <Enter> to new-line

					if ( lineLineAtFront ) {
						outstr << setw(total_width) << setfill(' ') << "";
					}
					// 3. Unicode 32 print
					for ( auto pWord = singleLine->cbegin(); pWord != singleLine->cend(); ++pWord ) 
					{
						auto pSingleCharacter = *pWord;
						if ( pSingleCharacter!=nullptr ) {
							int u8BytesCnt = pSingleCharacter->nBytes;
							switch ( u8BytesCnt  )
							{
							case 1:
								{
									outstr << "| " << pSingleCharacter->unicode32_str << " ";
									// if( pSingleCharacter->nEOF_Flag == static_cast<int>(FileInfo::E_WIN) ) {
									// 	outstr << "| " << G_CHAR_GN.hexCodeStr << " ";
									// }
								}
								break;
							case 2:
							case 3:
							case 4:
								{
									 //      9                 14                     19
									 // | AB | FF |     | AB | CD | EF |     | AB | CD | EF | AB |
									 // | U+AABBC |     |              |     |                   |
									 if ( pSingleCharacter->unicode32_str.find("U+") != string::npos ) {
										 // 这个长度是 显示的字符串的长度  "U+XXXX".len = 6,  "U+XXXXXX".len = 8
										 size_t u32sz = pSingleCharacter->unicode32_str.size();
										 // size_t u32sz = pSingleCharacter->nBytes;
										 size_t total = (u8BytesCnt == 2 ? 9 : (u8BytesCnt == 3 ? 14 : 19));
										 size_t rest = abs(static_cast<int>(total - u32sz));
										 size_t left, right;
										 left = right = 0;
										 if ( rest % 2 == 0 ) {
											 left = right = (rest / 2);
										 } else {
											 left = rest / 2;
											 right = left + 1;
										 }
										 outstr << "|" << setw(left) << setfill(' ') << "" << pSingleCharacter->unicode32_str << setw(right) << setfill(' ') << "";
									 } else {
										 outstr << "| " << pSingleCharacter->unicode32_str << " ";
									 }
								}
								break;
							default:
								break;
							}
						}
					}
					outstr << "|";  // print line tail flag
					outstr << endl;

					outstr << endl;
				}
			}
		}
	}

    retStr = outstr.str();
    
    if ( needPrintToConsole ) {
        cout << retStr  << endl;
    }

}



bool readCpp_File(const string& filename, FileInfo& fInfo)
{
    ifstream file(filename.c_str(), ios::in);
    fInfo.filename = filename;
    if ( !file ) {
		fInfo.lineCnt = 0;
        if( SHOULD_LOG_OPEN_FAILED ) {
            cout << "Sorry , Can't open file : " << filename << endl;
        }
        return false;
    }
    fInfo.isValid = true;


	const char SPACE = ' ';
	const char CR =  '\r';
	const char LF =  '\n';
	const char BACH_SPASH = '\\';

	const string QUOTE = "\"";
	const string S_COMMENT_1 = "//";
	const string S_COMMENT_2_BEG = "/*";
	const string S_COMMENT_2_END = "*/";

	string::size_type NOT_FOUND = string::npos;

	int commentType = 0;
	int lineNo = 0;
	fInfo.quoteStringList.clear();

	while( !file.eof() ) {
		string line;
		getline( file, line, LF);
		++lineNo;
		
		if( line.empty() ) {
			continue;
		} else {
			if( line.at( line.size()-1) == CR ) {
				line = line.substr(0, line.size()-1);
				if( line.empty() ) {
					continue;
				}
			}
		}


		auto spFlag = 0;

		string::size_type q_begin = string::npos;
		string::size_type q_end = string::npos;
		string::size_type comment_1 = string::npos;
		string::size_type comment_2_beg = string::npos;
		string::size_type comment_2_end = string::npos;

		if( commentType == 2 ) {
			// try the find the paired     */
			
			auto comment_2_end_next = line.find(S_COMMENT_2_END);
			if( comment_2_end_next == NOT_FOUND ) {
				continue;
			} else {
				line = line.substr(comment_2_end_next+2);
				spFlag = 5;
				commentType = 0;
			}
		}

		if( commentType == 0 ) {
			auto firstValid = line.find_first_not_of(SPACE);
			if( firstValid == NOT_FOUND ) {
				// all character is <Space>
				continue;
			} else {
				line = line.substr(firstValid);
				if( line.empty() ) {
					continue;
				}
			}

			q_begin = line.find(QUOTE);
			comment_1 = line.find(S_COMMENT_1);
			comment_2_beg = line.find(S_COMMENT_2_BEG);
			comment_2_end = line.find(S_COMMENT_2_END);

			if( q_begin == NOT_FOUND ) {
				if( comment_1!=NOT_FOUND || comment_2_beg!=NOT_FOUND ) {
					// FOUND Comment  // or /*   or  BOTH
					if( comment_1!=NOT_FOUND && comment_2_beg==NOT_FOUND ) {
						// Found Only //
						continue;
					} else if( comment_2_beg!=NOT_FOUND && comment_1==NOT_FOUND ) {
						// Found Only /*
						if( comment_2_end == NOT_FOUND ) {
							commentType = 2;
							continue;	// try to find the end of */
						} else {
							// FOUND  end of  */
							commentType = 0;	// fond */ at the same line
							continue;
						}
					} else {
						//  BOTH  //  and   /*
						if( comment_1 < comment_2_beg ) {
							continue;
						} else {
							//			/*   before    //
							if( comment_2_end == NOT_FOUND ) {
								commentType = 2;
								continue;	// skip current line , try to find the end of */
							} else {
								// FOUND  end of  */
								commentType = 0;	// fond */ at the same line , but no  "\""
								continue;
							}
						}
					}
				}
			} else {
				//////////////////////////////////////////////////
				// Has FOUND "\""
				//////////////////////////////////////////////////

				if( comment_1==NOT_FOUND && comment_2_beg==NOT_FOUND ) {
					// Only with  "..."
					spFlag = 1;
				} else {
					// With // or /*   or     Both  // and /*
					if( comment_1!=NOT_FOUND && comment_2_beg==NOT_FOUND ) {
						// "\""   Only with  //
						spFlag = 2;

						if( comment_1 < q_begin ) {
							continue;
						} 
					} else if( comment_2_beg!=NOT_FOUND && comment_1==NOT_FOUND ) {
						// "\""    Only with     /*
						spFlag = 3;
						if( comment_2_beg < q_begin ) {
							if( comment_2_end == NOT_FOUND ) {
								commentType = 2;
								continue;
							}
						}
					} else {
						// Both  "..." , // , /*
						spFlag = 4;

						if( comment_1 < q_begin || comment_2_beg < q_begin ) {
							if( comment_1 < comment_2_beg ) {
								continue;
							} else {
								// comment_2_beg < comment_1
								if( comment_2_end == NOT_FOUND ) {
									commentType = 2;
									continue;
								}
							}
						}
					}
				}
			}
		}

		if( spFlag!=0 ) {
			int withoutEndOfQuoteFlag = 0;

			if( spFlag==5 ) {
				// Found matched    */
				q_begin = line.find(QUOTE);
				if( q_begin == NOT_FOUND ) {
					continue;
				} else {
					cout << filename << " : Something Wrong @Line : " << lineNo << ", Loop after string Logic is diff = "<< std::dec << (-1) << endl;
					continue;
				}
			} else if( spFlag == 4 ) {
				// Very Very Special  "\""  and  //  and  /*
				cout << filename << " : Something Wrong @Line : " << lineNo << ", Very Very Very Special Logic = "<< std::dec << (-1) << endl;
				continue;
			} else if( spFlag == 3 || spFlag == 2 || spFlag == 1 ) {
				// 3 =>       "\""    Only with     /*
				// 2 =>       "\""    Only with     //
				// 1 =>       "..."    

				while( true  ) {
					q_end = line.find(QUOTE, q_begin+1);
					if( q_end == NOT_FOUND ) {
						withoutEndOfQuoteFlag = 1;
						break;
					}

					// FOUND matched end of      "\""
					bool isEmptyStr = (q_end == (q_begin+1));
					if( !isEmptyStr ) {
						// "..."		Not empty string
						int checkPos1 = q_end-1;
						int checkPos2 = q_end-2;
						// special condition    "abc\"def"
						// very very special condition    "abc\\"
						while( line.at(checkPos1) == BACH_SPASH 
						  && (checkPos2>=0 && checkPos2 < static_cast<int>(line.size()) && line.at(checkPos2)!=BACH_SPASH)
						) {
							q_end = line.find(QUOTE, q_end+1);
							if( q_end == NOT_FOUND ) {
								withoutEndOfQuoteFlag = 2;
								break;	// break inner
							}
						}

						if( withoutEndOfQuoteFlag == 2 ) {
							// cout << "Something Wrong @Line : " << lineNo << endl;
							// break outside while
							break;
						}
					}

					string innerString = line.substr(q_begin+1,  q_end-q_begin-1);
					if( !innerString.empty() ) {
						fInfo.quoteStringList.push_back( make_pair(innerString, lineNo) );
					}

					if( spFlag == 2 && q_end > comment_1 ) {
						//     ""   //
						// "...//..."   inside
						comment_1 = line.find(S_COMMENT_1, q_end+1);
						if( comment_1 == NOT_FOUND ) {
							spFlag = 1;
						}
					} else if( spFlag == 3 && q_end > comment_2_beg ) {
						//     ""   /*
						comment_2_beg = line.find(S_COMMENT_2_BEG, q_end+1);
						if( comment_2_beg  == NOT_FOUND ) {
							spFlag = 1;
						}
					}

					//
					// find next begin
					//
					q_begin = line.find(QUOTE, q_end+1);
					if( q_begin == NOT_FOUND) {
						break;
					} else {
						if( spFlag == 2 && q_begin > comment_1 ) {
							// q_begin has been comment
							break;
						} else if( spFlag == 3 && q_begin > comment_2_beg ) {
							withoutEndOfQuoteFlag = 3;
							commentType = 2;
							break;
						}
					}
				} // end while  pick all  "..."

				if( withoutEndOfQuoteFlag!=0 ) {
					if( withoutEndOfQuoteFlag == 3) {
						// Found matched end of    end of */
						continue;
					} else {
						cout << filename << " : Something Wrong @Line : " << lineNo  << " withoutEndOfQuoteFlag = " << std::dec << withoutEndOfQuoteFlag << endl;
						continue;
					}
				}
			} // end of  ==3 || ==2 || ==1
		}

	}

	return true;
}






} // end namespace  SuperBinaryDisplayUtil
