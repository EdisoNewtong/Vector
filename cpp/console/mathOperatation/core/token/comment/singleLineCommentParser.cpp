#include "singleLineCommentParser.h"


using namespace std;

SingleLineCommentParser::SingleLineCommentParser()
{

}



// virtual
SingleLineCommentParser::~SingleLineCommentParser()
{

}

// virtual 
void SingleLineCommentParser::init()
{
	m_AllAvalibleCharacters.insert( make_pair('/', E_ChType::E_COMMENT_CHAR_1) );
}

// virtual 
E_PaserType SingleLineCommentParser::appendContent(char ch)
{
	(void)ch;
	return E_UNDETERMIND;	
}

