#include "parser.h"

using namespace std;

Parser::Parser(const char* buf, std::streamsize sz)
	: m_buf(buf)
	, m_size(sz)
{
	m_AllAvalibleCharacters.clear();

	for ( char ch = 'a'; ch <= 'z'; ++ch ) {
		m_AllAvalibleCharacters.insert(  make_pair(ch, E_LETTER) );
	}

	for ( char ch = 'A'; ch <= 'Z'; ++ch ) {
		m_AllAvalibleCharacters.insert(  make_pair(ch, E_LETTER) );
	}

	for ( char ch = '0'; ch <= '9'; ++ch ) {
		m_AllAvalibleCharacters.insert(  make_pair(ch, E_NUMBER) );
	}
	m_AllAvalibleCharacters.insert(  make_pair('.', E_DOT) );
	m_AllAvalibleCharacters.insert(  make_pair('_', E_UNDERLINE) );

	m_AllAvalibleCharacters.insert(  make_pair('+', E_ADD) );
	m_AllAvalibleCharacters.insert(  make_pair('-', E_MINUS) );
	m_AllAvalibleCharacters.insert(  make_pair('*', E_MULTIPLY) );
	m_AllAvalibleCharacters.insert(  make_pair('/', E_DIVIDE) );
	m_AllAvalibleCharacters.insert(  make_pair('%', E_MOD) );

	m_AllAvalibleCharacters.insert(  make_pair('&', E_BIT_AND) );
	m_AllAvalibleCharacters.insert(  make_pair('|', E_BIT_OR) );
	m_AllAvalibleCharacters.insert(  make_pair('^', E_BIT_XOR) );
	m_AllAvalibleCharacters.insert(  make_pair('~', E_BIT_NOT) );

	m_AllAvalibleCharacters.insert(  make_pair('<', E_LESS_THAN) );
	m_AllAvalibleCharacters.insert(  make_pair('>', E_GREATER_THAN) );

	m_AllAvalibleCharacters.insert(  make_pair('(', E_OPEN_PARENTHESES) );
	m_AllAvalibleCharacters.insert(  make_pair(')', E_CLOSE_PARENTHESES) );

	m_AllAvalibleCharacters.insert(  make_pair(';', E_SEMICOLON) );
	m_AllAvalibleCharacters.insert(  make_pair('=', E_ASSIGN) );

	m_AllAvalibleCharacters.insert(  make_pair(' ',  E_SPACE) );
	m_AllAvalibleCharacters.insert(  make_pair('\t', E_TAB) );
	m_AllAvalibleCharacters.insert(  make_pair('\r', E_NEW_LINE_R) );
	m_AllAvalibleCharacters.insert(  make_pair('\n', E_NEW_LINE_N) );
	
}

// virtual 
Parser::~Parser()
{
}

/*

a-z  A-Z  0-9  _

=

+   -   *   /  %

&   |   ^    ~

<<    >>    

( )

;

<Space>   <Tab>   \r \n

*/




/*

[+-] 123456    [uU][lL][lL]
[+-] 0[0-9]+   [uU][lL][lL]
[+-] 0x  or 0X[0-9A-F]+    [uU][lL][lL]

[+-] [0-9]*.[0-9]*     fF
[+-] [0-9]*.[0-9]* e (int number)     fF


*/

int Parser::doParse()
{
	for ( std::streamsize idx = 0; idx < m_size; ++idx )
	{
		char ch = m_buf[idx];
		(void)ch;
		
	}

	return 1;
}


