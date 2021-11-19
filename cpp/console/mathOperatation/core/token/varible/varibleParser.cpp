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

	m_tokenType = E_TOKEN_VARIBLE;
}


// virtual 
E_PaserType  VaribleParser::appendContent(ParsedCharInfo* pInfo) // override
{
	auto curCh = pInfo->currentChar;
	auto pBaseInfo = getInsideCharSetBaseInfo(curCh);
	if ( pBaseInfo != nullptr ) {
		m_alreadyTravelsaledString += curCh;
		m_endInfo = pInfo->position;
	} else {
		/*
		   e.g.

		       abc+
                  ^
			   Re-Parse   '+'
		*/

		m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
		return E_P_DEFAULT;
	}

	return m_type;
}




// virtual
bool VaribleParser::isEnd(ParsedCharInfo* pInfo) // override
{
	auto curCh = pInfo->currentChar;
	auto pBaseInfo = getInsideCharSetBaseInfo(curCh);
	if ( pBaseInfo == nullptr ) {
		return true;
	} 

	// pBaseInfo != nullptr;
	return pInfo->isLastChar;

}
