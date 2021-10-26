#include "multiLineCommentParser.h"

using namespace std;


MultiLineCommentParser::MultiLineCommentParser(E_PaserType tp)
	: TokenParserBase(tp)
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
E_PaserType  MultiLineCommentParser::appendContent(char ch, ParserInfo* pInfo)
{
	(void)ch;
	(void)pInfo;
	return E_P_UNDETERMIND;	
}

