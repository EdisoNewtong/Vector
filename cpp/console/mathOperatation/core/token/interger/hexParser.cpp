#include "hexParser.h"

using namespace std;

HexParser::HexParser(E_PaserType tp)
	: TokenParserBase(tp)
{
}

//
// virtual 
//
HexParser::~HexParser()
{
}


// virtual 
void HexParser::init() // override
{
	m_AllAvalibleCharacters.insert( make_pair('+', CharUtil::getCharBaseInfo('+') ) );
	m_AllAvalibleCharacters.insert( make_pair('-', CharUtil::getCharBaseInfo('-') ) );

	// 0x     or     0X
	m_AllAvalibleCharacters.insert( make_pair('x', CharUtil::getCharBaseInfo('x') ) );
	m_AllAvalibleCharacters.insert( make_pair('X', CharUtil::getCharBaseInfo('X')  ) );

	for( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}

	for( char ch = 'a'; ch <='f'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}

	for( char ch = 'A'; ch <='F'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}

	m_AllAvalibleCharacters.insert( make_pair('u', CharUtil::getCharBaseInfo('u') ) );
	m_AllAvalibleCharacters.insert( make_pair('U', CharUtil::getCharBaseInfo('U') ) );
	m_AllAvalibleCharacters.insert( make_pair('l', CharUtil::getCharBaseInfo('l') ) );
	m_AllAvalibleCharacters.insert( make_pair('L', CharUtil::getCharBaseInfo('L') ) );

	m_tokenType = E_TOKEN_HEX_NUMBER;
}


// virtual 
E_PaserType HexParser::appendContent(ParsedCharInfo* pInfo) // override
{
	(void)pInfo;
	return E_P_DEFAULT;	
}



