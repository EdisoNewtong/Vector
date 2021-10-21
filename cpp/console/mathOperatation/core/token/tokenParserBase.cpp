#include "tokenParserBase.h"
#include "parserException.h"

using namespace std;

TokenParserBase::TokenParserBase()
	: m_alreadyTravelsaledString("")
{
	m_AllAvalibleCharacters.clear();


	//
	// Char Set
	//
	m_CharSet.clear();
	for ( char ch = 'a'; ch <= 'z'; ++ch ) {
		m_CharSet.insert(  make_pair(ch, E_LETTER) );
	}

	for ( char ch = 'A'; ch <= 'Z'; ++ch ) {
		m_CharSet.insert(  make_pair(ch, E_LETTER) );
	}

	for ( char ch = '0'; ch <= '9'; ++ch ) {
		m_CharSet.insert(  make_pair(ch, E_NUMBER) );
	}
	m_CharSet.insert(  make_pair('.', E_DOT) );
	m_CharSet.insert(  make_pair('_', E_UNDERLINE) );

	m_CharSet.insert(  make_pair('+', E_ADD) );
	m_CharSet.insert(  make_pair('-', E_MINUS) );
	m_CharSet.insert(  make_pair('*', E_MULTIPLY) );
	m_CharSet.insert(  make_pair('/', E_DIVIDE) );
	m_CharSet.insert(  make_pair('%', E_MOD) );

	m_CharSet.insert(  make_pair('&', E_BIT_AND) );
	m_CharSet.insert(  make_pair('|', E_BIT_OR) );
	m_CharSet.insert(  make_pair('^', E_BIT_XOR) );
	m_CharSet.insert(  make_pair('~', E_BIT_NOT) );

	m_CharSet.insert(  make_pair('<', E_LESS_THAN) );
	m_CharSet.insert(  make_pair('>', E_GREATER_THAN) );

	m_CharSet.insert(  make_pair('(', E_OPEN_PARENTHESES) );
	m_CharSet.insert(  make_pair(')', E_CLOSE_PARENTHESES) );

	m_CharSet.insert(  make_pair(';', E_SEMICOLON) );
	m_CharSet.insert(  make_pair('=', E_ASSIGN) );

	m_CharSet.insert(  make_pair(' ',  E_SPACE) );
	m_CharSet.insert(  make_pair('\t', E_TAB) );
	m_CharSet.insert(  make_pair('\r', E_NEW_LINE_R) );
	m_CharSet.insert(  make_pair('\n', E_NEW_LINE_N) );


}


// virtual 
TokenParserBase::~TokenParserBase()
{
}


// virtual 
void TokenParserBase::init()
{

}

// virtual 
E_PaserType  TokenParserBase::appendContent(char ch)
{
	auto pr = isValidChar(ch);
	if ( !pr.second ) {
		ParserExpection	e(E_ExceptionCode::E_UNKNOWN_CHAR);

		string info;
		info += ch;
		e.setDetail(info);

		throw e;
	}

	m_alreadyTravelsaledString += ch;
	E_ChType type =	(pr.first)->second;
	if (   type == E_SPACE
		|| type == E_TAB
		|| type == E_NEW_LINE_R
		|| type == E_NEW_LINE_N
	) {
		return E_P_BLANK;
	} else {

	}
	
	return E_UNDETERMIND;	
}



pair< std::unordered_map<char, E_ChType>::iterator, bool> TokenParserBase::isValidChar(char ch)
{
	auto it = m_CharSet.find(ch);
    auto b = it != m_CharSet.end();
	return std::make_pair(it,b);
}


void TokenParserBase::clearTravelsaled()
{
	m_alreadyTravelsaledString.clear();
}

