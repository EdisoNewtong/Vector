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
void BlankParser::init()
{
	m_AllAvalibleCharacters.insert(  make_pair(' ',  CharInfo(E_ChType::E_SPACE, E_CAT_SEPERATOR) ) );
	m_AllAvalibleCharacters.insert(  make_pair('\t', CharInfo(E_ChType::E_TAB, E_CAT_SEPERATOR)) );
	m_AllAvalibleCharacters.insert(  make_pair('\r', CharInfo(E_ChType::E_NEW_LINE_R, E_CAT_SEPERATOR) ) );
	m_AllAvalibleCharacters.insert(  make_pair('\n', CharInfo(E_ChType::E_NEW_LINE_N, E_CAT_SEPERATOR) ) );
}


// virtual 
E_PaserType  BlankParser::appendContent(char ch, ParserInfo* pInfo)
{
	auto pr = isValidChar(ch);
	if ( !pr.second ) {
		return E_P_UNDETERMIND;	
	}

	return E_P_BLANK;
}

// virtual 
TokenInfo* BlankParser::generateToken()
{
	auto retInfo = new TokenInfo(E_TOKEN_BLANK, E_TOKEN_BLANK);
	retInfo->setDetail(m_token);
	return retInfo;
}
