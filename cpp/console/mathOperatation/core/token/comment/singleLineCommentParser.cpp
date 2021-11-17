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
E_PaserType SingleLineCommentParser::appendContent(ParsedCharInfo* pInfo) // override
{
	auto curCh = pInfo->currentChar;
	auto lastCh = m_alreadyTravelsaledString.back();
	
	if ( curCh == '\n' ) {
		
		m_alreadyTravelsaledString += curCh;
		// m_switchFlag = 2;
		return E_P_DEFAULT;	
	} else {
		// curCh != '\n'
		if ( lastCh == '\r' ) {
			// m_switchFlag = 1; // previous '\r' is end , passby default
			return E_P_DEFAULT;	
		} else {
			m_alreadyTravelsaledString += curCh;
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


// virtual 
bool SingleLineCommentParser::isEnd() // override;
{
	return true;	
}

