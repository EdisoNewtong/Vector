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

    m_tokenType = E_TOKEN_MULTI_COMMENT;
    m_exceptionCode = E_MULTI_LINE_COMMENT_INVALID_FORMAT;
	m_parserName = "MultiLineCommentParser";
}


// virtual 
E_PaserType  MultiLineCommentParser::appendContent(ParsedCharInfo* pInfo) // override
{
	char curCh = pInfo->currentChar;
	m_alreadyTravelsaledString += curCh; // append current char first
	m_endInfo = pInfo->position;

	// new-size After   Append += curCh;
	int sz = static_cast<int>( m_alreadyTravelsaledString.size() );
	if ( sz >= 4 ) {
		auto last2str = m_alreadyTravelsaledString.substr( sz-2 );
		if ( last2str == "/*" ) {
			//  /* 
			//  
			//     /* line-1          warning 1
			//     /* line-2          warning 2
			//     line-3
			//     line-4
			//  */
	 		++m_warningCnt;
		} else if ( last2str == "*/" ) {
			m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT;
	 		return E_P_DEFAULT;	
		}
	}

	return m_type;	
}




// virtual
bool MultiLineCommentParser::isEnd(ParsedCharInfo* pInfo) // override
{
	int sz = static_cast<int>( m_alreadyTravelsaledString.size() );
	if ( sz < 4 ) {
		return false;
	} 

	// else >=4
	string last2str = m_alreadyTravelsaledString.substr( sz - 2 );
	return last2str == "*/";
}


// virtual 
TokenInfo* MultiLineCommentParser::generateToken() // override;
{
	auto subType = m_tokenType;
	auto pRetToken = new TokenInfo( E_TOKEN_COMMENT_TYPE , subType);
	m_token = m_alreadyTravelsaledString;
	pRetToken->setDetail( m_token );
	return pRetToken;
}

