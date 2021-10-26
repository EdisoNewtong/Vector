#include "operatorParser.h"

using namespace std;

OperatorParser::OperatorParser(E_PaserType tp)
	: TokenParserBase(tp)
{
}

OperatorParser::~OperatorParser()
{
}

// virtual 
void OperatorParser::init()
{
	m_AllAvalibleCharacters.insert( make_pair('+', CharInfo(E_ChType::E_ADD, E_CAT_OPERATOR) ) );
	m_AllAvalibleCharacters.insert( make_pair('-', CharInfo(E_ChType::E_MINUS, E_CAT_OPERATOR) ) );
	m_AllAvalibleCharacters.insert( make_pair('*', CharInfo(E_ChType::E_MULTIPLY, E_CAT_OPERATOR) ) );
	m_AllAvalibleCharacters.insert( make_pair('/', CharInfo(E_ChType::E_DIVIDE, E_CAT_OPERATOR) ) );
	m_AllAvalibleCharacters.insert( make_pair('%', CharInfo(E_ChType::E_MOD, E_CAT_OPERATOR) ) );

	m_AllAvalibleCharacters.insert( make_pair('&', CharInfo(E_ChType::E_BIT_AND, E_CAT_OPERATOR) ));
	m_AllAvalibleCharacters.insert( make_pair('|', CharInfo(E_ChType::E_BIT_OR, E_CAT_OPERATOR)  ));
	m_AllAvalibleCharacters.insert( make_pair('^', CharInfo(E_ChType::E_BIT_XOR, E_CAT_OPERATOR) ));
	m_AllAvalibleCharacters.insert( make_pair('~', CharInfo(E_ChType::E_BIT_NOT, E_CAT_OPERATOR) ));

	m_AllAvalibleCharacters.insert( make_pair('<', CharInfo(E_ChType::E_LESS_THAN, E_CAT_OPERATOR) ) );     // <<
	m_AllAvalibleCharacters.insert( make_pair('>', CharInfo(E_ChType::E_GREATER_THAN, E_CAT_OPERATOR) ) );  // >>
	m_AllAvalibleCharacters.insert( make_pair('=', CharInfo(E_ChType::E_ASSIGN, E_CAT_ASSIGNMENT) ) );        // =
}

// virtual 
E_PaserType OperatorParser::appendContent(char ch, ParserInfo* pInfo)
{
	(void)ch;
	(void)pInfo;
	return E_P_UNDETERMIND;	
}

