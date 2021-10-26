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
void VaribleParser::init()
{
	for( char ch = 'a'; ch <='z'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharInfo(E_ChType::E_LETTER, E_CAT_ALPHA) ) );
	}

	for( char ch = 'A'; ch <='Z'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharInfo(E_ChType::E_LETTER, E_CAT_ALPHA) ) );
	}

	for( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharInfo(E_ChType::E_NUMBER, E_CAT_NUMBER) ) );
	}

	// Add   '_'
	m_AllAvalibleCharacters.insert( make_pair('_', CharInfo(E_ChType::E_UNDERLINE, E_CAT_UNDER_LINE) ) );

}


// virtual 
E_PaserType  VaribleParser::appendContent(char ch, ParserInfo* pInfo)
{
	(void)ch;
	(void)pInfo;
	return E_P_UNDETERMIND;	
}

