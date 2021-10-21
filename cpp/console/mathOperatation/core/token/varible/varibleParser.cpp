#include "varibleParser.h"

using namespace std;

VaribleParser::VaribleParser()
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
		m_AllAvalibleCharacters.insert( make_pair(ch, E_ChType::E_LETTER) );
	}

	for( char ch = 'A'; ch <='Z'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, E_ChType::E_LETTER) );
	}

	for( char ch = '0'; ch <='9'; ++ch ) {
		m_AllAvalibleCharacters.insert( make_pair(ch, E_ChType::E_NUMBER) );
	}

	//
	// Add   "_"
	//
	m_AllAvalibleCharacters.insert( make_pair('_', E_ChType::E_UNDERLINE) );

}


// virtual 
E_PaserType  VaribleParser::appendContent(char ch)
{
	(void)ch;
	return E_UNDETERMIND;	
}

