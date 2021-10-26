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
void SingleLineCommentParser::init()
{
	m_AllAvalibleCharacters.insert( make_pair('/', CharInfo(E_ChType::E_COMMENT_CHAR_1, E_CAT_OTHERS) ) );
}

// virtual 
E_PaserType SingleLineCommentParser::appendContent(char ch, ParserInfo* pInfo)
{
    if ( pInfo->isLastChar ) {
	    m_token += ch;
		return E_P_UNDETERMIND;
	} else {
	    // auto sz = m_token.size();
		// size_t lastPos = sz - 1;
		// size_t lastButOnePos = sz - 2;
		// char   chLastButOne = m_token[lastButOnePos];

		if ( pInfo->previousChar == '\n' ) {
			// Set End-Flag
			return E_P_UNDETERMIND;	
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
				return E_P_UNDETERMIND;	
			}
		} else {
			m_token += ch;
		}
	}

	return E_P_SINGLE_LINE_COMMENT;	
}

