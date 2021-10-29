#include "multiLineCommentParser.h"

using namespace std;


MultiLineCommentParser::MultiLineCommentParser(E_PaserType tp)
	: TokenParserBase(tp)
	, m_warningCnt(0)
{
}

// virtual 
MultiLineCommentParser::~MultiLineCommentParser()
{
}

// virtual 
void MultiLineCommentParser::init()
{
	m_AllAvalibleCharacters.insert( make_pair('/', CharInfo(E_ChType::E_COMMENT_CHAR_1, E_CAT_OTHERS) ) );
	m_AllAvalibleCharacters.insert( make_pair('*', CharInfo(E_ChType::E_COMMENT_CHAR_2, E_CAT_OTHERS) ) );
}

// virtual 
E_PaserType  MultiLineCommentParser::appendContent(char ch, ParsedCharInfo* pInfo)
{
    if ( pInfo->isLastChar ) {
	    m_token += ch;
		// Check is End Flag
	} else {
		if ( ch == '*' ) {
			m_token += ch;
			if ( pInfo->previousChar == '/' ) {
				/*  
				   in side  / *
				*/
				++m_warningCnt;
			} 
		} else if ( ch == '/' ) {
			if ( pInfo->previousChar == '*' ) {
				//   */
				//   Set End Flag
				m_token += ch;
			} else {
				m_token += ch;
			}
		} else {
			m_token += ch;
		}
	}

	return E_P_DEFAULT;	
}

// virtual 
TokenInfo* MultiLineCommentParser::generateToken()
{
	auto retInfo = new TokenInfo(E_TOKEN_MULTI_COMMENT, E_TOKEN_MULTI_COMMENT);
	retInfo->setDetail(m_token);
	return retInfo;
}


