#include "varibleParser.h"

using namespace std;

VaribleParser::VaribleParser(E_PaserType tp)
	: TokenParserBase(tp)
{
}

//
// virtual 
//
VaribleParser::~VaribleParser()
{
}





// virtual 
void VaribleParser::init() // override
{
	for( char ch = 'a'; ch <='z'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}

	for( char ch = 'A'; ch <='Z'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}

	for( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharUtil::getCharBaseInfo(ch) ) );
	}

	// Add   '_'
	m_AllAvalibleCharacters.insert( make_pair('_', CharUtil::getCharBaseInfo('_') ) );

}


// virtual 
E_PaserType  VaribleParser::appendContent(ParsedCharInfo* pInfo, list<TokenInfo*>* pTokenList) // override
{
	char ch = pInfo->baseInfo->getCh();
	auto pBaseInfo = getInsideCharSetBaseInfo(ch);
	if ( pBaseInfo == nullptr ) {
		return E_P_DEFAULT;
	}

	(void)pTokenList;

	return m_type;
}

// virtual 
TokenInfo* VaribleParser::generateToken() // override
{
	auto retInfo = new TokenInfo(E_TOKEN_VARIBLE, E_TOKEN_VARIBLE);
	retInfo->setDetail(m_token);
	return retInfo;
}
