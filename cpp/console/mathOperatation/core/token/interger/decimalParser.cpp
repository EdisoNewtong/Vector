#include "decimalParser.h"

using namespace std;

DecimalParser::DecimalParser(E_PaserType tp)
	: TokenParserBase(tp)
{
}

//
// virtual 
//
DecimalParser::~DecimalParser()
{
}

// virtual 
void DecimalParser::init()
{
	m_AllAvalibleCharacters.insert( make_pair('+', CharInfo(E_ChType::E_SIGN_POSITIVE, E_CAT_OPERATOR) ) );
	m_AllAvalibleCharacters.insert( make_pair('-', CharInfo(E_ChType::E_SIGN_NEGATIVE, E_CAT_OPERATOR) ) );
	for ( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharInfo(E_ChType::E_NUMBER, E_CAT_NUMBER) ) );
	}

	m_AllAvalibleCharacters.insert( make_pair('u', CharInfo(E_ChType::E_SUFFIX, E_CAT_ALPHA) ) );
	m_AllAvalibleCharacters.insert( make_pair('U', CharInfo(E_ChType::E_SUFFIX, E_CAT_ALPHA) ) );
	m_AllAvalibleCharacters.insert( make_pair('l', CharInfo(E_ChType::E_SUFFIX, E_CAT_ALPHA) ) );
	m_AllAvalibleCharacters.insert( make_pair('L', CharInfo(E_ChType::E_SUFFIX, E_CAT_ALPHA) ) );
}


// virtual 
E_PaserType  DecimalParser::appendContent(char ch, ParsedCharInfo* pInfo)
{
	(void)ch;
	(void)pInfo;
	return E_P_DEFAULT;	
}


// virtual 
TokenInfo* DecimalParser::generateToken()
{
	auto retInfo = new TokenInfo(E_TOKEN_DECIMAL_NUMBER, E_TOKEN_DECIMAL_NUMBER);
	retInfo->setDetail(m_token);
	return retInfo;
}

