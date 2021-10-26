#ifndef  MULTI_LINE_COMMENT_PARSER_H
#define  MULTI_LINE_COMMENT_PARSER_H

#include "tokenParserBase.h"

#include "parserInfo.h"

class MultiLineCommentParser : public TokenParserBase
{
public:
	MultiLineCommentParser(E_PaserType tp);
	virtual ~MultiLineCommentParser();

	virtual void init();
	virtual E_PaserType  appendContent(char ch, ParserInfo* pInfo);
protected:
	int m_warningCnt;
};


#endif

