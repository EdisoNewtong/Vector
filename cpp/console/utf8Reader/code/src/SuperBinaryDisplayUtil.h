#ifndef SUPER_BINARY_DISPLAY_UTIL_H
#define SUPER_BINARY_DISPLAY_UTIL_H

#include <fstream>
#include <string>
#include <utility>
#include <sstream>
#include <vector>
#include <list>

using namespace std;



namespace SuperBinaryDisplayUtil
{
	struct SpecialAnsiChar
	{
		char          ch;
		unsigned int  ansi_code;
		string		  hexCodeStr;
		string        displayStr;
		string        displayStr2;
	};

    // '\t',    '\n',     '\v',    '\f',     '\r',
	//                                   ch  ,                ansi_code       ,     hexStrCode,       displayStr,    displayStr2
	const SpecialAnsiChar G_CHAR_TAB = { '\t', static_cast<unsigned int>('\t'),   string("09"),    string("\\t"),  string("\\t")  };		// \t = 9
	const SpecialAnsiChar G_CHAR_GN  = { '\n', static_cast<unsigned int>('\n'),   string("0A"),    string("\\n"),  string("<LF>") };		// \t = 10  = <LF>
	const SpecialAnsiChar G_CHAR_GR  = { '\r', static_cast<unsigned int>('\r'),   string("0D"),    string("\\r"),  string("<CR>") };		// \r = 13  = <CR>
    //
    // not very Common Ansi 
    //
	const SpecialAnsiChar G_CHAR_VTAB = { '\v', static_cast<unsigned int>('\v'),   string("0B"),    string("\\v"),  string("\\v") };		// \r = 13  = <CR>
    // 0000    1100    014    12    0x0C     FF (NP form feed, new page) 换页键
	const SpecialAnsiChar G_CHAR_GNP = { '\f', static_cast<unsigned int>('\f'),   string("0C"),    string("\\f"),  string("\\f") };		// \r = 13  = <CR>

    const int SPECIAL_CHAR_CNT = 3;
    const int EOF_TYPE_CNT = 3;

    const size_t SIZE_BOM = 3;
    const unsigned int utf8_Bom_tag[SIZE_BOM] = { 0xEF, 0xBB, 0xBF };      //  ==>   U+FEFF

    const unsigned int FULL_BITS = 0xFF;
    const size_t UTF8_FULL_LEN = 4;
    const unsigned int multiBytesStartTag[UTF8_FULL_LEN] = {
        // lead byte for 3 conditions ( 2-bytes / 3-bytes / 4-bytes )
        0x06,       // 110X,XXXX  ->          tag >> 5 = 0000,0110 = 0x06 // 2 bytes
        0x0E,       // 1110,XXXX  ->          tag >> 4 = 0000,1110 = 0x0E // 3 bytes
        0x1E,       // 1111,0XXX  ->          tag >> 3 = 0001,1110 = 0x1E // 4 bytes
        // next bytes
        0x02        // 10XX,XXXX  ->          tag >> 6 = 0000,0010 = 0x02 // the bytes after the leader bytes must be like the following style    10XX,XXXX
    };

	// ??? a multi-bytes character will always occupy 1/2 space ?? that's a question
	const int C_N_WIDTH_4_MULTI_BYTES_CHARACTER_1 = 1;
	const int C_N_WIDTH_4_MULTI_BYTES_CHARACTER_2 = 2;
	const int C_N_WIDTH_4_MULTI_BYTES_CHARACTER_USED = C_N_WIDTH_4_MULTI_BYTES_CHARACTER_1;

    const bool SHOULD_LOG_OPEN_FAILED = true;
    const bool SHOULD_LOG_EMPTY_FILE = false;


	// ###########################################################################
	// ###########################################################################
	// ###########################################################################
	// ###########################################################################
	// ###########################################################################

    
    struct SingleCharacter
    {
    protected:
        SingleCharacter();
    public:
        virtual ~SingleCharacter();
        static SingleCharacter* generate_ASCII_Character(char ch, int row, int col, int eofFlag);
        static SingleCharacter* generate_MultiBytes_Character(const string& displayCharacter, unsigned int unicode32, int row, int col);


        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Field Parts
        //
        char ansi_char;
        char binary_charAry[UTF8_FULL_LEN];
        string display_str;
        string utf8HexAry[UTF8_FULL_LEN];
        unsigned short nBytes;	// 1 or 2 or 3 or 4
        unsigned int unicode32;
        string unicode32_str;

        int nRow;
        int nCol;
        int nEOF_Flag;

        int reserverd_flag; // = 0
    };

    struct FileInfo 
    {
        enum E_ENDLINE_MODE {
            E_UNKNOWN_ENDLINE = 0,
            E_UNIX = 1, E_LINUX = 1,    // \n    Unix and Linux , with the same EOF  End-Of-Line   
            E_WIN = 2,  // \r\n
            E_MAC = 3,  // \r
            E_HYBRID = 4,
        };
        
        FileInfo();
        virtual ~FileInfo();
        
        // Member Data
        string filename;
        bool isValid;
        bool isBomFile;
        bool isBinary;
        int bytesCount;
        int characterCount;
        int lineCnt;			// 行数
		int hybridLineNo;	// 多重 EOF，初次被确定的那一行
        E_ENDLINE_MODE endlineMode;
        
        int    firstBinaryByte_Idx;
        int    firstBinaryByte_Row;
        int    firstBinaryByte_Col;
        
        vector<string*> rawHexVec;
        // line Display Vector , int stored the lineNo
        list< pair<list<SingleCharacter*>*, int> > lineVec;
		// new added quote-string
        list< pair<string,int> > quoteStringList;
    };


    bool isCanInputChar(const char& ch);
    unsigned int char2HexCode(const char& ch, string* displayHexCode = nullptr);
	int getDecimalBits(int num);

	//
	// very very old version , 1 byte after another
	//
    bool readFile_V0(const string& filename, FileInfo& fInfo);

	// do <NOT> skip any letter , scan from the file's begin to the end of the file 
	//----------------------------------------------------------------------------------------------------
	//		when the parser meet multi-bytes character , it will try to judge how many bytes can make a character
	// and read subsequence n bytes all together
	//----------------------------------------------------------------------------------------------------
    bool readFile_V1(const string& filename, FileInfo& fInfo);

	// stop scanning bytes after a certain bytes has been treat as binary flag
    bool readFileRapidly(const string& filename, FileInfo& fInfo);

	// new Added
    bool readCpp_File(const string& filename, FileInfo& fInfo);
	
    void printFileInfo(const FileInfo& fileInfo, string& retStr, bool needPrintToConsole = false);

} // end namespace SuperBinaryDisplayUtil


#endif

