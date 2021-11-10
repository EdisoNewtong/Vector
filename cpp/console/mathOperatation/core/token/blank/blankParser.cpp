#include "blankParser.h"


using namespace std;

BlankParser::BlankParser(E_PaserType tp)
	: TokenParserBase(tp)
{

}

// virtual 
BlankParser::~BlankParser()
{

}

// virtual 
void BlankParser::init() // override
{
	//
	m_AllAvalibleCharacters.insert(  make_pair(' ',  CharUtil::getCharBaseInfo(' ')  ) );
	m_AllAvalibleCharacters.insert(  make_pair('\t', CharUtil::getCharBaseInfo('\t') ) );
	m_AllAvalibleCharacters.insert(  make_pair('\r', CharUtil::getCharBaseInfo('\r') ) );
	m_AllAvalibleCharacters.insert(  make_pair('\n', CharUtil::getCharBaseInfo('\n') ) );
}


// virtual 
E_PaserType  BlankParser::appendContent(ParsedCharInfo* pInfo, list<TokenInfo*>* pTokenList) // override
{
	char ch = pInfo->baseInfo->getCh();
	auto pBaseInfo = isInsideCharSet(ch);
	if ( pBaseInfo == nullptr ) {
		return E_P_DEFAULT;	
	} else {
		m_token += ch;
	}

	return m_type;
}

// virtual 
TokenInfo* BlankParser::generateToken() // override
{
	auto retInfo = new TokenInfo(E_TOKEN_BLANK, E_TOKEN_BLANK);
	retInfo->setDetail(m_token);
	return retInfo;
}
