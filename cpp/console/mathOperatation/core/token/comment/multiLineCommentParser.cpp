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
void MultiLineCommentParser::init() // override
{
	m_AllAvalibleCharacters.insert( make_pair('/', CharUtil::getCharBaseInfo('/') ) );
	m_AllAvalibleCharacters.insert( make_pair('*', CharUtil::getCharBaseInfo('*') ) );
}

// virtual 
E_PaserType  MultiLineCommentParser::appendContent(ParsedCharInfo* pInfo, list<TokenInfo*>* pTokenList) // override
{
	char ch = pInfo->baseInfo->getCh();
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
TokenInfo* MultiLineCommentParser::generateToken() // override
{
	auto retInfo = new TokenInfo(E_TOKEN_MULTI_COMMENT, E_TOKEN_MULTI_COMMENT);
	retInfo->setDetail(m_token);
	return retInfo;
}


