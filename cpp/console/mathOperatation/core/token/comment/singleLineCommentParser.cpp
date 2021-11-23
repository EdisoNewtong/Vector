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

	m_tokenType = E_TOKEN_SINGLE_COMMENT;
    m_exceptionCode = E_SINGLE_LINE_COMMENT_INVALID_FORMAT;
	m_parserName = "SingleLineCommentParser";
}

// virtual 
E_PaserType SingleLineCommentParser::appendContent(ParsedCharInfo* pInfo) // override
{
	auto curCh = pInfo->currentChar;
	auto lastCh = m_alreadyTravelsaledString.back();
	
	if ( curCh == '\n' ) {
		//    //  abc\n   , '\n' is the end of line character
		m_alreadyTravelsaledString += curCh;
		m_endInfo = pInfo->position;

		m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT;

		return E_P_DEFAULT;	
	} else {
		// curCh != '\n'
		if ( lastCh == '\r' ) {
			/*    
			    x = 3;\r   
				x = 4;
			    ^

				'x' should be parsed by < Default-Parser >

				curCh != '\n'

			*/

			m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE;
			return E_P_DEFAULT;	
		} else {
			// lastCh != '\r'  , append content
			m_alreadyTravelsaledString += curCh;
			m_endInfo = pInfo->position;

			if ( pInfo->isLastChar ) {
				m_switchFlag = E_TOKEN_TERMINATE_TO_DEFAULT;
				return E_P_DEFAULT;	
			}
		}
	}

	return m_type;	
}





// virtual 
bool SingleLineCommentParser::isEnd(ParsedCharInfo* pInfo) // override;
{
	int sz = static_cast<int>( m_alreadyTravelsaledString.size() );
	if ( sz < 2 ) {
		return false;
	}

	// sz >=2
	string prefix = m_alreadyTravelsaledString.substr(0,2);
	// char lastCh = m_alreadyTravelsaledString.back();
	if ( prefix == "//" ) {
		char curCh = pInfo->currentChar;
		if ( pInfo->isLastChar ) {
			//      Last Line     :    x = 10 + 5; //               OK
			//      Last Line     :    x = 10 + 5; //a              OK
			//      Last Line     :    x = 10 + 5; //\r             OK
			//      Last Line     :    x = 10 + 5; //\n             OK
			//      Last Line     :    x = 10 + 5; //\r\n           OK
			return true;
		} else {
			return      (curCh == '\n')
				     || (curCh == '\r' && pInfo->nextChar != '\n');
		}
	} else {
		return false;
	}
}


// virtual 
TokenInfo*  SingleLineCommentParser::generateToken() // override;
{
	auto subType = m_tokenType;
	auto pRetToken = new TokenInfo( E_TOKEN_COMMENT_TYPE , subType );
	m_token = m_alreadyTravelsaledString;
	pRetToken->setDetail( m_token );
	return pRetToken;
}
