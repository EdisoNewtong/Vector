#include "hexParser.h"

using namespace std;

HexParser::HexParser()
{
}

//
// virtual 
//
HexParser::~HexParser()
{
}


// virtual 
void HexParser::init()
{
	m_AllAvalibleCharacters.insert( make_pair('+', E_ChType::E_SIGN_POSITIVE) );
	m_AllAvalibleCharacters.insert( make_pair('-', E_ChType::E_SIGN_NEGATIVE) );

	// 0x     or     0X
	m_AllAvalibleCharacters.insert( make_pair('x', E_ChType::E_PREFIX) );
	m_AllAvalibleCharacters.insert( make_pair('X', E_ChType::E_PREFIX) );

	for( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, E_ChType::E_NUMBER) );
	}

	for( char ch = 'a'; ch <='f'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, E_ChType::E_NUMBER) );
	}

	for( char ch = 'A'; ch <='F'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, E_ChType::E_NUMBER) );
	}

	m_AllAvalibleCharacters.insert( make_pair('u', E_ChType::E_SUFFIX) );
	m_AllAvalibleCharacters.insert( make_pair('U', E_ChType::E_SUFFIX) );
	m_AllAvalibleCharacters.insert( make_pair('l', E_ChType::E_SUFFIX) );
	m_AllAvalibleCharacters.insert( make_pair('L', E_ChType::E_SUFFIX) );
}


// virtual 
E_PaserType HexParser::appendContent(char ch, ParserInfo* pInfo)
{
	(void)ch;
	(void)pInfo;

	return E_UNDETERMIND;	
}

