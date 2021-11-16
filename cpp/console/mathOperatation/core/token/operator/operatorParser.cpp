#include <iostream>
#include "operatorParser.h"

using namespace std;

OperatorParser::OperatorParser(E_PaserType tp)
	: TokenParserBase(tp)
{
	m_opMap.clear();

	m_opMap.insert( make_pair('(', E_TOKEN_OP_OPEN_PARENTHESES ) );         // =
	m_opMap.insert( make_pair(')', E_TOKEN_OP_CLOSE_PARENTHESES ) );         // =

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
	m_opMap.insert( make_pair('>', E_TOKEN_OP_BIT_RIGHT_SHIFT ) );    // >>
	m_opMap.insert( make_pair('=', E_TOKEN_OP_ASSIGNMENT ) );         // =


}

OperatorParser::~OperatorParser()
{
}

// virtual 
void OperatorParser::init() // override
{


}

// virtual 
E_PaserType OperatorParser::appendContent(ParsedCharInfo* pInfo, list<TokenInfo*>* pTokenList) // override
{
	(void)pTokenList;
	// cout << "Inside op appendContent() . m_alreadyTravelsaledString = \"" << m_alreadyTravelsaledString << "\" , ch = " << pInfo->currentChar << " = " << static_cast<int>(pInfo->currentChar) << endl;

	if ( pInfo->baseInfo != nullptr ) {
		char ch = pInfo->baseInfo->getCh();
		int strlen = static_cast<int>( m_alreadyTravelsaledString.size() );
		if ( strlen == 1 ) {
			char previousCh = m_alreadyTravelsaledString.at(0);
			// cout << "previousCh = " << previousCh << " , m_alreadyTravelsaledString = " << m_alreadyTravelsaledString << endl;
			if ( previousCh == '/' ) {
				if ( ch == '/' ) {
					//   single-line comment <Start>   "//"
					// cout << "\t\t //" << endl;
					// m_switchFlag = 3;
					return E_P_SINGLE_LINE_COMMENT;
				} else if ( ch == '*' ) {
					//   multi-line comment <Start>    "/*"
					// cout << "\t\t /*" << endl;
					// m_switchFlag = 3;
					return E_P_MULTI_LINE_COMMENT;
				}
			} else if ( previousCh == '<' ) {
				if ( ch == '<' ) {
					//     "<<"   , Meet Terminal Condition
					return E_P_DEFAULT;
				} else {
					// TODO : throw  exception
				}
			} else if ( previousCh == '>' ) {
				if ( ch == '>' ) {
					//     ">>"   ,  Meet Terminal Condition
					return E_P_DEFAULT;
				} else {
					// TODO : throw  exception
				}
			} else {

			}
		}
	} else {

	}

	return m_type;	
}

// virtual 
TokenInfo* OperatorParser::generateToken() // override
{
	auto it = m_opMap.find( m_token.at(0) );
	auto retInfo = new TokenInfo(E_TOKEN_OPERATOR, it->second);
	retInfo->setDetail(m_token);
	return  retInfo;
}
