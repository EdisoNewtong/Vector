#include "octalParser.h"

using namespace std;

OctalParser::OctalParser()
{
}

//
// virtual 
//
OctalParser::~OctalParser()
{
}

// virtual 
void OctalParser::init()
{
	m_AllAvalibleCharacters.insert( make_pair('+', E_ChType::E_SIGN_POSITIVE) );
	m_AllAvalibleCharacters.insert( make_pair('-', E_ChType::E_SIGN_NEGATIVE) );
	for( char ch = '0'; ch <='7'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, E_ChType::E_NUMBER) );
	}

	m_AllAvalibleCharacters.insert( make_pair('u', E_ChType::E_SUFFIX) );
	m_AllAvalibleCharacters.insert( make_pair('U', E_ChType::E_SUFFIX) );
	m_AllAvalibleCharacters.insert( make_pair('l', E_ChType::E_SUFFIX) );
	m_AllAvalibleCharacters.insert( make_pair('L', E_ChType::E_SUFFIX) );

}


// virtual 
E_PaserType  OctalParser::appendContent(char ch, ParserInfo* pInfo)
{
	(void)ch;
	(void)pInfo;
	return E_UNDETERMIND;	
}
