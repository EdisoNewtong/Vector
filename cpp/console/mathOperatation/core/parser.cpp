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
	, m_currentParser(nullptr)
	, m_currentPaserType( E_UNDETERMIND )
{
	m_parserMap.clear();

    //
	m_parserMap.insert( make_pair(E_P_DEFAULT, new TokenParserBase() ) );
	// 
	m_parserMap.insert( make_pair(E_P_DECIMAL, new DecimalParser() ) );
	m_parserMap.insert( make_pair(E_P_OCTAL,   new OctalParser() ) );
	m_parserMap.insert( make_pair(E_P_HEX,     new HexParser() ) );
	//
	m_parserMap.insert( make_pair(E_P_FLOAT,  new FloatParser() ) );
	//
	m_parserMap.insert( make_pair(E_P_VARIBLE, new VaribleParser() ) );
	//
 	m_parserMap.insert( make_pair(E_P_SINGLE_LINE_COMMENT, new SingleLineCommentParser() ) );
 	m_parserMap.insert( make_pair(E_P_MULTI_LINE_COMMENT,  new MultiLineCommentParser()  ) );
    //
 	m_parserMap.insert( make_pair(E_P_BLANK,  new BlankParser() ) );

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

	// set current parser
	m_currentParser = m_parserMap[E_P_DEFAULT];
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





int Parser::doParse()
{
	
	size_t idx;
	for ( idx = 0; idx < m_size; ++idx )
	{
		char ch = m_buf[idx];
		auto guessType = m_currentParser->appendContent(ch);
		if ( guessType != E_UNDETERMIND ) {
			m_currentParser =  m_parserMap[guessType];
		}
		
	}



	return 1;
}


