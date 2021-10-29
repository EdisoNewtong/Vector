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
void HexParser::init()
{
	m_AllAvalibleCharacters.insert( make_pair('+', CharInfo(E_ChType::E_SIGN_POSITIVE, E_CAT_OPERATOR) ) );
	m_AllAvalibleCharacters.insert( make_pair('-', CharInfo(E_ChType::E_SIGN_NEGATIVE, E_CAT_OPERATOR) ) );

	// 0x     or     0X
	m_AllAvalibleCharacters.insert( make_pair('x', CharInfo(E_ChType::E_PREFIX, E_CAT_ALPHA) ) );
	m_AllAvalibleCharacters.insert( make_pair('X', CharInfo(E_ChType::E_PREFIX, E_CAT_ALPHA) ) );

	for( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharInfo(E_ChType::E_NUMBER, E_CAT_NUMBER) ) );
	}

	for( char ch = 'a'; ch <='f'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharInfo(E_ChType::E_NUMBER, E_CAT_ALPHA) ) );
	}

	for( char ch = 'A'; ch <='F'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharInfo(E_ChType::E_NUMBER, E_CAT_ALPHA) ) );
	}

	m_AllAvalibleCharacters.insert( make_pair('u', CharInfo(E_ChType::E_SUFFIX, E_CAT_ALPHA) ) );
	m_AllAvalibleCharacters.insert( make_pair('U', CharInfo(E_ChType::E_SUFFIX, E_CAT_ALPHA) ) );
	m_AllAvalibleCharacters.insert( make_pair('l', CharInfo(E_ChType::E_SUFFIX, E_CAT_ALPHA) ) );
	m_AllAvalibleCharacters.insert( make_pair('L', CharInfo(E_ChType::E_SUFFIX, E_CAT_ALPHA) ) );
}


// virtual 
E_PaserType HexParser::appendContent(char ch, ParsedCharInfo* pInfo)
{
	(void)ch;
	(void)pInfo;

	return E_P_DEFAULT;	
}


// virtual 
TokenInfo* HexParser::generateToken()
{
	auto retInfo = new TokenInfo(E_TOKEN_HEX_NUMBER, E_TOKEN_HEX_NUMBER);
	retInfo->setDetail(m_token);
	return retInfo;
}
