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
	// +    -    *    /    %    &    |    ^    ~    <<    >>    (    )    =      // Operator Type
	m_AllAvalibleCharacters.insert(  make_pair('+', CharUtil::getCharBaseInfo('+')  ) );
	m_AllAvalibleCharacters.insert(  make_pair('-', CharUtil::getCharBaseInfo('-') ) );
	m_AllAvalibleCharacters.insert(  make_pair('*', CharUtil::getCharBaseInfo('*') ) );
	m_AllAvalibleCharacters.insert(  make_pair('/', CharUtil::getCharBaseInfo('/') ) );
	m_AllAvalibleCharacters.insert(  make_pair('%', CharUtil::getCharBaseInfo('%') ) );

	m_AllAvalibleCharacters.insert(  make_pair('&', CharUtil::getCharBaseInfo('&')  ) );
	m_AllAvalibleCharacters.insert(  make_pair('|', CharUtil::getCharBaseInfo('|') ) );
	m_AllAvalibleCharacters.insert(  make_pair('^', CharUtil::getCharBaseInfo('^') ) );
	m_AllAvalibleCharacters.insert(  make_pair('~', CharUtil::getCharBaseInfo('~') ) );

	m_AllAvalibleCharacters.insert(  make_pair('<', CharUtil::getCharBaseInfo('<') ) );
	m_AllAvalibleCharacters.insert(  make_pair('>', CharUtil::getCharBaseInfo('>') ) );

	m_AllAvalibleCharacters.insert(  make_pair('(', CharUtil::getCharBaseInfo('(') ) );
	m_AllAvalibleCharacters.insert(  make_pair(')', CharUtil::getCharBaseInfo(')') ) );

	m_AllAvalibleCharacters.insert(  make_pair('=', CharUtil::getCharBaseInfo('=') ) );

	m_tokenType = E_TOKEN_OPERATOR;
}

// virtual 
E_PaserType OperatorParser::appendContent(ParsedCharInfo* pInfo) // override
{
	char curCh = pInfo->currentChar;
	
	int strlen = static_cast<int>( m_alreadyTravelsaledString.size() );
	if ( strlen == 1 ) {
		char previousCh = m_alreadyTravelsaledString.at(0);
		// cout << "previousCh = " << previousCh << " , m_alreadyTravelsaledString = " << m_alreadyTravelsaledString << endl;
		if ( previousCh == '/' ) {
			if ( curCh == '/' ) {
				//   single-line comment <Start>   "//"
				m_alreadyTravelsaledString += curCh;      // m_alreadyTravelsaledString == "//"
				m_switchFlag = E_TOKEN_CONVERT_TO_OTHER;
				return E_P_SINGLE_LINE_COMMENT;
			} else if ( curCh == '*' ) {
				//   multi-line comment <Start>    "/*"
				m_alreadyTravelsaledString += curCh;      // m_alreadyTravelsaledString == "/*"
				m_switchFlag = E_TOKEN_CONVERT_TO_OTHER;
				return E_P_MULTI_LINE_COMMENT;
			} else {
				// 
				// e.g.   abc/d
				//            ^    
				//  previous '/'   =>  Divide Operator
				//          Do Re-Parse 'd'
				m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
				return E_P_DEFAULT;
			}
		} else if ( previousCh == '<' ) {
			if ( curCh == '<' ) {
				//     "<<"   , Meet Terminal Condition
				m_alreadyTravelsaledString += curCh;
				m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT;
				return E_P_DEFAULT;
			} else {
				// TODO : throw  exception
			}
		} else if ( previousCh == '>' ) {
			if ( curCh == '>' ) {
				//     ">>"   ,  Meet Terminal Condition
				m_alreadyTravelsaledString += curCh;
				m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT;
				return E_P_DEFAULT;
			} else {
				// TODO : throw  exception
			}
		} else {
			// Rest possibility is OK
		}
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


// virtual 
bool OperatorParser::isEnd(ParsedCharInfo* pInfo) // override;
{
	auto sz = static_cast<int>( m_alreadyTravelsaledString.size() );
	if ( sz == 1 ) {
		char lastCh =  m_alreadyTravelsaledString.at(0);
		if (lastCh == '<'  ||  lastCh == '>') {
			// Need  subsequent  '<'   or  subsequent '>'
			return false;
		} else {
			if ( lastCh == '/' ) {
				// means  '/' =>  divide operator
				return pInfo->isLastChar;
			} else {
				//  lastCh  !=  '/'
				return true;
			}
		}
	} else if ( sz == 2 ) {
		return    m_alreadyTravelsaledString == "<<"
			   || m_alreadyTravelsaledString == ">>";
	} else {
		// sz must in the range [1 , 2]
		return false;
	}
}

