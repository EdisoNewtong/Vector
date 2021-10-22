#include "parser.h"

#include "tokenParserBase.h"

#include "decimalParser.h"
#include "octalParser.h"
#include "hexParser.h"

#include "floatParser.h"

#include "varibleParser.h"

#include "singleLineCommentParser.h"
#include "multiLineCommentParser.h"

#include "blankParser.h"


using namespace std;

Parser::Parser()
	: m_buf(nullptr)
	, m_size(0)
    // Parser
    , m_defaultParser(nullptr)
	, m_currentParser(nullptr)
	, m_currentPaserType( E_UNDETERMIND )
	, m_pInfo()
{
	m_parserMap.clear();

    //
	m_parserMap.insert( make_pair(static_cast<int>(E_P_DEFAULT), new TokenParserBase() ) );
	// 
	m_parserMap.insert( make_pair(static_cast<int>(E_P_DECIMAL), new DecimalParser() ) );
	m_parserMap.insert( make_pair(static_cast<int>(E_P_OCTAL),   new OctalParser() ) );
	m_parserMap.insert( make_pair(static_cast<int>(E_P_HEX),     new HexParser() ) );
	//
	m_parserMap.insert( make_pair(static_cast<int>(E_P_FLOAT),  new FloatParser() ) );
	//
	m_parserMap.insert( make_pair(static_cast<int>(E_P_VARIBLE), new VaribleParser() ) );
	//
 	m_parserMap.insert( make_pair(static_cast<int>(E_P_SINGLE_LINE_COMMENT), new SingleLineCommentParser() ) );
 	m_parserMap.insert( make_pair(static_cast<int>(E_P_MULTI_LINE_COMMENT),  new MultiLineCommentParser()  ) );
    //
 	m_parserMap.insert( make_pair(static_cast<int>(E_P_BLANK),  new BlankParser() ) );

	//
	// init
	//
	for( auto it = m_parserMap.begin(); it != m_parserMap.end(); ++it )
	{
		auto parser = it->second;
		if ( parser != nullptr ) {
			parser->init();
		}
	}

    m_defaultParser = m_parserMap[static_cast<int>(E_P_DEFAULT)]; // set default parser
	m_currentParser = m_defaultParser;                            // set current parser
}

// virtual 
Parser::~Parser()
{
	for( auto it = m_parserMap.begin(); it != m_parserMap.end(); ++it )
	{
		auto parser = it->second;
		if ( parser != nullptr ) {
			delete parser;
			it->second = nullptr;
		}
	}
	m_parserMap.clear();
	
}


void Parser::setContent(const char* buf, size_t sz)
{
	m_buf = buf;
	m_size = sz;
}


/*

a-z  A-Z  0-9  _

=

+   -      *   /  %

&   |   ^    ~

<<    >>    

( )

;

<Space>   <Tab>   \r \n

*/


void Parser::processLineInfo(char ch, size_t idx)
{
    //
	// set index and flag
    //
	m_pInfo.nCharIdx = idx;
    m_pInfo.isLastChar = (idx == m_size-1);

	if ( m_pInfo.hasPreviousChar ) {
		if ( m_pInfo.previousChar == '\r' ) {
			if ( ch != '\n' ) {
				++m_pInfo.nLine;
				m_pInfo.nCol = 1;
			} else {
				++m_pInfo.nCol;
			}
		} else if ( m_pInfo.previousChar == '\n' ) {
			++m_pInfo.nLine;
			m_pInfo.nCol = 1;
		} else {
			++m_pInfo.nCol;
		}
	} else {
		++m_pInfo.nCol;
	}

}



int Parser::doParse()
{
	size_t idx;
	for ( idx = 0; idx < m_size; ++idx )
	{
		char ch = m_buf[idx];
		// common process
		processLineInfo(ch,idx);

		// if ( m_currentParser != nullptr ) {
			auto guessType = m_currentParser->appendContent(ch, &m_pInfo);
			if ( guessType != m_currentPaserType ) {
				if ( guessType == E_UNDETERMIND ) {
					m_defaultParser->commonCheck(ch, &m_pInfo);
				}

				m_currentParser = m_parserMap[ static_cast<int>(guessType) ];
				m_currentPaserType = guessType;
			}
		// }

		m_pInfo.previousChar = ch;
		m_pInfo.hasPreviousChar = true; 
	}

	return 1;
}


