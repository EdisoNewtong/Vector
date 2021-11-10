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
void DecimalParser::init() // override
{
	m_AllAvalibleCharacters.insert( make_pair('+', CharUtil::getCharBaseInfo('+') ) );
	m_AllAvalibleCharacters.insert( make_pair('-', CharUtil::getCharBaseInfo('-') ) );
	for ( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}

	m_AllAvalibleCharacters.insert( make_pair('u', CharUtil::getCharBaseInfo('u') ) );
	m_AllAvalibleCharacters.insert( make_pair('U', CharUtil::getCharBaseInfo('U') ) );
	m_AllAvalibleCharacters.insert( make_pair('l', CharUtil::getCharBaseInfo('l') ) );
	m_AllAvalibleCharacters.insert( make_pair('L', CharUtil::getCharBaseInfo('L') ) );
}


// virtual 
E_PaserType  DecimalParser::appendContent(ParsedCharInfo* pInfo, list<TokenInfo*>* pTokenList) // override
{
	(void)pInfo;
	(void)pTokenList;
	return E_P_DEFAULT;	
}


// virtual 
TokenInfo* DecimalParser::generateToken() // override
{
	auto retInfo = new TokenInfo(E_TOKEN_DECIMAL_NUMBER, E_TOKEN_DECIMAL_NUMBER);
	retInfo->setDetail(m_token);
	return retInfo;
}

