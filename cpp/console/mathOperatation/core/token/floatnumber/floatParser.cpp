#include "floatParser.h"

using namespace std;

FloatParser::FloatParser(E_PaserType tp)
	: TokenParserBase(tp)
{

}



//
// virtual 
//
FloatParser::~FloatParser()
{
}




// virtual 
void FloatParser::init() // override
{
	// -1.23e-2f
	m_AllAvalibleCharacters.insert( make_pair('+', CharUtil::getCharBaseInfo('+') ) );
	m_AllAvalibleCharacters.insert( make_pair('-', CharUtil::getCharBaseInfo('-') ) );
	for( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}
	m_AllAvalibleCharacters.insert( make_pair('.', CharUtil::getCharBaseInfo('.') ) );

	m_AllAvalibleCharacters.insert( make_pair('e', CharUtil::getCharBaseInfo('e') ) );
	m_AllAvalibleCharacters.insert( make_pair('E', CharUtil::getCharBaseInfo('E') ) );

	m_AllAvalibleCharacters.insert( make_pair('f', CharUtil::getCharBaseInfo('f') ) );
	m_AllAvalibleCharacters.insert( make_pair('F', CharUtil::getCharBaseInfo('F') ) );

}


// virtual 
E_PaserType  FloatParser::appendContent(ParsedCharInfo* pInfo) // override
{
	(void)pInfo;
	return E_P_DEFAULT;	
}

// virtual 
TokenInfo* FloatParser::generateToken()  // override
{
	auto retInfo = new TokenInfo(E_TOKEN_FLOAT_NUMBER, E_TOKEN_FLOAT_NUMBER);
	retInfo->setDetail(m_token);
	return retInfo;
}
