#include "operatorParser.h"

using namespace std;

OperatorParser::OperatorParser(E_PaserType tp)
	: TokenParserBase(tp)
{
	m_opMap.clear();
	m_opMap.insert( make_pair('+', E_TOKEN_OP_ADD ) );
	m_opMap.insert( make_pair('-', E_TOKEN_OP_MINUS ) );
	m_opMap.insert( make_pair('*', E_TOKEN_OP_MULTIPLY ) );
	m_opMap.insert( make_pair('/', E_TOKEN_OP_DIVIDE ) );
	m_opMap.insert( make_pair('%', E_TOKEN_OP_MOD) );

	m_opMap.insert( make_pair('&', E_TOKEN_OP_BIT_AND ));
	m_opMap.insert( make_pair('|', E_TOKEN_OP_BIT_OR  ));
	m_opMap.insert( make_pair('^', E_TOKEN_OP_BIT_XOR ));
	m_opMap.insert( make_pair('~', E_TOKEN_OP_BIT_NOT ));

	m_opMap.insert( make_pair('<', E_TOKEN_OP_BIT_LEFT_SHIFT ) );     // <<
	m_opMap.insert( make_pair('>', E_TOKEN_OP_BIT_RIGHT_SHIFT ) );  // >>
	m_opMap.insert( make_pair('=', E_TOKEN_OP_ASSIGNMENT ) );        // =
}

OperatorParser::~OperatorParser()
{
}

// virtual 
void OperatorParser::init()
{


}

// virtual 
E_PaserType OperatorParser::appendContent(char ch, ParserInfo* pInfo)
{
	(void)ch;
	(void)pInfo;
	return E_P_UNDETERMIND;	
}

// virtual 
TokenInfo* OperatorParser::generateToken()
{
	auto it = m_opMap.find( m_token.at(0) );
	auto retInfo = new TokenInfo(E_TOKEN_OPERATOR, it->second);
	retInfo->setDetail(m_token);
	return  retInfo;
}
