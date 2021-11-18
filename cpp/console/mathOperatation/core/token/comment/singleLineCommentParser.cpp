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
}

// virtual 
E_PaserType SingleLineCommentParser::appendContent(ParsedCharInfo* pInfo) // override
{
	auto curCh = pInfo->currentChar;
	auto lastCh = m_alreadyTravelsaledString.back();
	
	if ( curCh == '\n' ) {
		//    //  abc\n   , '\n' is the end of line character
		m_alreadyTravelsaledString += curCh;
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
	char lastCh = m_alreadyTravelsaledString.back();
	if ( prefix == "//" ) {
		if ( pInfo->isLastChar ) {
			//      Last Line     :    x = 10 + 5; //               OK
			//      Last Line     :    x = 10 + 5; //a              OK
			//      Last Line     :    x = 10 + 5; //\r             OK
			//      Last Line     :    x = 10 + 5; //\n             OK
			//      Last Line     :    x = 10 + 5; //\r\n           OK
			return true;
		} else {
			return (lastCh == '\r' || lastCh == '\n');
		}
	} else {
		return false;
	}
}

