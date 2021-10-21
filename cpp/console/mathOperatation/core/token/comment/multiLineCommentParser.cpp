#include "multiLineCommentParser.h"

using namespace std;


MultiLineCommentParser::MultiLineCommentParser()
{
}

// virtual 
MultiLineCommentParser::~MultiLineCommentParser()
{
}

// virtual 
void MultiLineCommentParser::init()
{
	m_AllAvalibleCharacters.insert( make_pair('/', E_ChType::E_COMMENT_CHAR_1) );
	m_AllAvalibleCharacters.insert( make_pair('*', E_ChType::E_COMMENT_CHAR_2) );
}

// virtual 
E_PaserType  MultiLineCommentParser::appendContent(char ch, ParserInfo* pInfo)
{
	(void)ch;
	(void)pInfo;
	return E_UNDETERMIND;	
}

