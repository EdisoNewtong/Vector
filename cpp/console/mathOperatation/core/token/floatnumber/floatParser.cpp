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
void FloatParser::init()
{
	// -1.23e-2f
	m_AllAvalibleCharacters.insert( make_pair('+', CharInfo(E_ChType::E_SIGN_POSITIVE, E_CAT_OPERATOR) ) );
	m_AllAvalibleCharacters.insert( make_pair('-', CharInfo(E_ChType::E_SIGN_NEGATIVE, E_CAT_OPERATOR) ) );
	for( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, CharInfo(E_ChType::E_NUMBER, E_CAT_NUMBER) ) );
	}
	m_AllAvalibleCharacters.insert( make_pair('.', CharInfo(E_ChType::E_DOT, E_CAT_OTHERS) ) );

	m_AllAvalibleCharacters.insert( make_pair('e', CharInfo(E_ChType::E_DOT, E_CAT_ALPHA) ) );
	m_AllAvalibleCharacters.insert( make_pair('E', CharInfo(E_ChType::E_DOT, E_CAT_ALPHA) ) );

	m_AllAvalibleCharacters.insert( make_pair('f', CharInfo(E_ChType::E_SUFFIX, E_CAT_ALPHA) ) );
	m_AllAvalibleCharacters.insert( make_pair('F', CharInfo(E_ChType::E_SUFFIX, E_CAT_ALPHA) ) );

}


// virtual 
E_PaserType  FloatParser::appendContent(char ch, ParserInfo* pInfo)
{
	(void)ch;
	(void)pInfo;
	return E_P_UNDETERMIND;	
}

