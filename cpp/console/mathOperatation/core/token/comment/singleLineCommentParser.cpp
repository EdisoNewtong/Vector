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
	(void)ch;
	(void)pInfo;
	return E_P_UNDETERMIND;	
}

