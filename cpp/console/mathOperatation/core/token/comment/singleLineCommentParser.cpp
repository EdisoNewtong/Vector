#include "singleLineCommentParser.h"


using namespace std;

SingleLineCommentParser::SingleLineCommentParser(E_PaserType tp)
	: TokenParserBase(tp)
{

}



// virtual
SingleLineCommentParser::~SingleLineCommentParser()
{

}

// virtual 
void SingleLineCommentParser::init() // override
{
	m_AllAvalibleCharacters.insert( make_pair('/', CharUtil::getCharBaseInfo('/') ) );
}

// virtual 
E_PaserType SingleLineCommentParser::appendContent(ParsedCharInfo* pInfo, list<TokenInfo*>* pTokenList) // override
{
	char ch = pInfo->baseInfo->getCh();
    if ( pInfo->isLastChar ) {
	    m_token += ch;
		return E_P_DEFAULT;
	} else {
		if ( pInfo->previousChar == '\n' ) {
			// Set End-Flag
			return E_P_DEFAULT;	
		} else if ( pInfo->previousChar == '\r' ) {
			if ( ch == '\n' ) {
				// last string is  "\r\n"
				m_token += ch;
			} else {
				// Set End-Flag , such as   
				/*
				      aaaa\r
					  b
				*/
				return E_P_DEFAULT;	
			}
		} else {
			m_token += ch;
		}
	}

	return m_type;	
}


// virtual 
TokenInfo* SingleLineCommentParser::generateToken() // override
{
	auto retInfo = new TokenInfo(E_TOKEN_SINGLE_COMMENT, E_TOKEN_SINGLE_COMMENT);
	retInfo->setDetail(m_token);
	return retInfo;
}

