#ifndef  SINGLE_LINE_COMMENT_PARSER_H
#define  SINGLE_LINE_COMMENT_PARSER_H

#include "tokenParserBase.h"

#include "parserInfo.h"

class SingleLineCommentParser : public TokenParserBase
{
public:
	SingleLineCommentParser();
	virtual ~SingleLineCommentParser();

	virtual void init();
	virtual E_PaserType  appendContent(char ch, ParserInfo* pInfo);
};


#endif
