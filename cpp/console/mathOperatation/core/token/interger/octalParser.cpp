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
void OctalParser::init()
{
	m_AllAvalibleCharacters.insert( make_pair('+', CharInfo(E_ChType::E_SIGN_POSITIVE, E_CAT_OPERATOR) ) );
	m_AllAvalibleCharacters.insert( make_pair('-', CharInfo(E_ChType::E_SIGN_NEGATIVE, E_CAT_OPERATOR) ) );
	for( char ch = '0'; ch <='7'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharInfo(E_ChType::E_NUMBER, E_CAT_NUMBER) ) );
	}

	m_AllAvalibleCharacters.insert( make_pair('u', CharInfo(E_ChType::E_SUFFIX, E_CAT_ALPHA) ) );
	m_AllAvalibleCharacters.insert( make_pair('U', CharInfo(E_ChType::E_SUFFIX, E_CAT_ALPHA) ) );
	m_AllAvalibleCharacters.insert( make_pair('l', CharInfo(E_ChType::E_SUFFIX, E_CAT_ALPHA) ) );
	m_AllAvalibleCharacters.insert( make_pair('L', CharInfo(E_ChType::E_SUFFIX, E_CAT_ALPHA) ) );

}


// virtual 
E_PaserType  OctalParser::appendContent(char ch, ParserInfo* pInfo)
{
	(void)ch;
	(void)pInfo;
	return E_P_UNDETERMIND;	
}


// virtual 
TokenInfo* OctalParser::generateToken()
{
	auto retInfo = new TokenInfo(E_TOKEN_OCTAL_NUMBER, E_TOKEN_OCTAL_NUMBER);
	retInfo->setDetail(m_token);
	return retInfo;
}
