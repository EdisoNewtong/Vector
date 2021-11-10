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
	if ( pInfo->baseInfo != nullptr ) {

		char ch = pInfo->baseInfo->getCh();
		int strlen = static_cast<int>( this->m_alreadyTravelsaledString.size() );
		if ( strlen == 1 ) {
			char previousCh = this->m_alreadyTravelsaledString.at(0);
			if ( previousCh == '/' ) {
				if ( ch == '/' ) {
					//   single-line comment <Start>   "//"
					return E_P_SINGLE_LINE_COMMENT;
				} else if ( ch == '*' ) {
					//   multi-line comment <Start>    "/*"
					return E_P_MULTI_LINE_COMMENT;
				}
			} else if ( previousCh == '<' ) {
				if ( ch == '<' ) {
					//     "<<"
				}

			} else if ( previousCh == '>' ) {
				if ( ch == '>' ) {
					//     ">>"
				}
			}
		}
	}

	(void)pTokenList;
	return E_P_DEFAULT;	
}

// virtual 
TokenInfo* OperatorParser::generateToken() // override
{
	auto it = m_opMap.find( m_token.at(0) );
	auto retInfo = new TokenInfo(E_TOKEN_OPERATOR, it->second);
	retInfo->setDetail(m_token);
	return  retInfo;
}
