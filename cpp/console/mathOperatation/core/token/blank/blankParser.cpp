#include "blankParser.h"

using namespace std;

BlankParser::BlankParser()
{

}

// virtual 
BlankParser::~BlankParser()
{

}

// virtual 
void BlankParser::init()
{
	m_AllAvalibleCharacters.insert(  make_pair(' ',  E_SPACE) );
	m_AllAvalibleCharacters.insert(  make_pair('\t', E_TAB) );
	m_AllAvalibleCharacters.insert(  make_pair('\r', E_NEW_LINE_R) );
	m_AllAvalibleCharacters.insert(  make_pair('\n', E_NEW_LINE_N) );
}


// virtual 
E_PaserType  BlankParser::appendContent(char ch, ParserInfo* pInfo)
{
	auto pr = isValidChar(ch);
	if ( !pr.second ) {
		return E_UNDETERMIND;	
	}

	return E_P_BLANK;
}

