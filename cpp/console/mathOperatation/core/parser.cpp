#include "parser.h"

#include "tokenParserBase.h"

#include "decimalParser.h"
#include "octalParser.h"
#include "hexParser.h"

#include "floatParser.h"

#include "varibleParser.h"

#include "operatorParser.h"

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
	, m_currentPaserType( E_P_DEFAULT )
	, m_pInfo()
{
	m_tokenList.clear();

	m_parserMap.clear();

    //
	m_parserMap.insert( make_pair(static_cast<int>(E_P_DEFAULT), new TokenParserBase(E_P_DEFAULT) ) );
	// 
	m_parserMap.insert( make_pair(static_cast<int>(E_P_DECIMAL), new DecimalParser(E_P_DECIMAL) ) );
	m_parserMap.insert( make_pair(static_cast<int>(E_P_OCTAL),   new OctalParser(E_P_OCTAL) ) );
	m_parserMap.insert( make_pair(static_cast<int>(E_P_HEX),     new HexParser(E_P_HEX) ) );
	//
	m_parserMap.insert( make_pair(static_cast<int>(E_P_FLOAT),  new FloatParser(E_P_FLOAT) ) );
	//
	m_parserMap.insert( make_pair(static_cast<int>(E_P_VARIBLE), new VaribleParser(E_P_VARIBLE) ) );
	//
 	m_parserMap.insert( make_pair(static_cast<int>(E_P_OPERATOR),  new OperatorParser(E_P_OPERATOR) ) );
	//
 	m_parserMap.insert( make_pair(static_cast<int>(E_P_SINGLE_LINE_COMMENT), new SingleLineCommentParser(E_P_SINGLE_LINE_COMMENT) ) );
 	m_parserMap.insert( make_pair(static_cast<int>(E_P_MULTI_LINE_COMMENT),  new MultiLineCommentParser(E_P_MULTI_LINE_COMMENT)  ) );
    //
 	m_parserMap.insert( make_pair(static_cast<int>(E_P_BLANK),  new BlankParser(E_P_BLANK) ) );
	

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

    m_defaultParser = m_parserMap[ static_cast<int>(E_P_DEFAULT) ]; // set default parser
	m_currentParser = m_defaultParser;                            // set current parser
}

// virtual 
Parser::~Parser()
{
	// clear parser map
	for( auto it = m_parserMap.begin(); it != m_parserMap.end(); ++it )
	{
		auto parser = it->second;
		if ( parser != nullptr ) {
			delete parser;
			it->second = nullptr;
		}
	}
	m_parserMap.clear();

	// clear token list
	for ( auto& pToken : m_tokenList ) {
		delete pToken;
		pToken = nullptr;
	}
	m_tokenList.clear();
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
	m_pInfo.pos.nCharIdx = idx;
    m_pInfo.isLastChar = (idx == m_size-1);

	//  Set Ch BaseInfo
	m_pInfo.baseInfo = CharUtil::getCharBaseInfo(ch);

	if ( m_pInfo.hasPreviousChar ) {
		if ( m_pInfo.previousChar == '\r' ) {
			if ( ch == '\n' ) {
				//  \r \n
				++m_pInfo.pos.nCol;
			} else {
				/*
				  string s = "Hello World";  \r
				  ++a;
				*/
				++m_pInfo.pos.nLine;
				m_pInfo.pos.nCol = 1;
			}
		} else if ( m_pInfo.previousChar == '\n' ) {
			++m_pInfo.pos.nLine;
			m_pInfo.pos.nCol = 1;
		} else {
			++m_pInfo.pos.nCol;
		}
	} else {
		++m_pInfo.pos.nCol;
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

		if ( idx == 0 ) {
			m_defaultParser->markBeginTag(&m_pInfo);
		}

		auto guessType = m_currentParser->appendContent(&m_pInfo, &m_tokenList);
		if ( guessType != m_currentPaserType ) {
			if ( guessType == E_P_DEFAULT ) {
				// check the char is inside the whole charSet , otherwise will throw an exception
				m_defaultParser->commonCheck(ch, &m_pInfo);
			}

			// save previous token
			auto previousParser = m_currentParser;
			// Do < Switch > Parser
			m_currentParser = m_parserMap[ static_cast<int>(guessType) ];

			// switch Parser
			if ( m_currentPaserType == E_P_DEFAULT ) {
				m_currentParser->transferToken( m_defaultParser );
			} else {
				auto pGenerateToken = previousParser->generateToken();
				if ( m_tokenList.empty() ) {
					m_tokenList.push_back( pGenerateToken );
				} else {
					// TODO
					// Check previous is valid or not
					checkPreviousTokenIsValid();
				}
			}

			m_currentPaserType = guessType;
		}


		// treat this char as previous char
		m_pInfo.previousChar = ch;
		m_pInfo.hasPreviousChar = true; 
	}


	//
	// TODO : Endless Logic Check
	//

	return 1;
}


bool Parser::checkPreviousTokenIsValid()
{
	return true;
}
