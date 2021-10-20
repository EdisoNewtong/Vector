#include "varibleParser.h"

using namespace std;

VaribleParser::VaribleParser()
{
	m_AllAvalibleCharacters.clear();

	for( char ch = 'a'; ch <='z'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, E_ChType::E_LETTER) );
	}

	for( char ch = 'A'; ch <='Z'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, E_ChType::E_LETTER) );
	}

	for( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, E_ChType::E_NUMBER) );
	}
	m_AllAvalibleCharacters.insert( make_pair('_', E_ChType::E_UNDERLINE) );
}

//
// virtual 
//
VaribleParser::~VaribleParser()
{
}

