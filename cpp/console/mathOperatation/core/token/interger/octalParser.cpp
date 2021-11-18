#include "octalParser.h"

using namespace std;

OctalParser::OctalParser(E_PaserType tp)
	: TokenParserBase(tp)
{
}

//
// virtual 
//
OctalParser::~OctalParser()
{
}

// virtual 
void OctalParser::init() // override
{
	// m_AllAvalibleCharacters.insert( make_pair('+', CharUtil::getCharBaseInfo('+') ) );
	// m_AllAvalibleCharacters.insert( make_pair('-', CharUtil::getCharBaseInfo('-') ) );
	
	for( char ch = '0'; ch <='7'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}

	m_AllAvalibleCharacters.insert( make_pair('u', CharUtil::getCharBaseInfo('u') ) );
	m_AllAvalibleCharacters.insert( make_pair('U', CharUtil::getCharBaseInfo('U') ) );
	m_AllAvalibleCharacters.insert( make_pair('l', CharUtil::getCharBaseInfo('l') ) );
	m_AllAvalibleCharacters.insert( make_pair('L', CharUtil::getCharBaseInfo('L') ) );

	m_tokenType = E_TOKEN_OCTAL_NUMBER;

}

// virtual 
E_PaserType  OctalParser::appendContent(ParsedCharInfo* pInfo) // override
{
	(void)pInfo;
	return E_P_DEFAULT;	
}



