#include "floatParser.h"

using namespace std;

FloatParser::FloatParser()
{
	// -1.23e-2f
	m_AllAvalibleCharacters.clear();

	m_AllAvalibleCharacters.insert( make_pair('+', E_ChType::E_SIGN_POSITIVE) );
	m_AllAvalibleCharacters.insert( make_pair('-', E_ChType::E_SIGN_NEGATIVE) );
	for( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, E_ChType::E_NUMBER) );
	}
	m_AllAvalibleCharacters.insert( make_pair('.', E_ChType::E_DOT) );

	m_AllAvalibleCharacters.insert( make_pair('e', E_ChType::E_DOT) );
	m_AllAvalibleCharacters.insert( make_pair('E', E_ChType::E_DOT) );

	m_AllAvalibleCharacters.insert( make_pair('f', E_ChType::E_SUFFIX) );
	m_AllAvalibleCharacters.insert( make_pair('F', E_ChType::E_SUFFIX) );
}



//
// virtual 
//
FloatParser::~FloatParser()
{
}
