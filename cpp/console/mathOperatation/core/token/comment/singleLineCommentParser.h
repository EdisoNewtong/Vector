#ifndef  SINGLE_LINE_COMMENT_PARSER_H
#define  SINGLE_LINE_COMMENT_PARSER_H

#include "tokenParserBase.h"

#include "parsedCharInfo.h"

class SingleLineCommentParser : public TokenParserBase
{
public:
	SingleLineCommentParser(E_PaserType tp);
	virtual ~SingleLineCommentParser();

	virtual void init() override;
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo) override;

	virtual bool isEnd(ParsedCharInfo* pInfo) override;
};


#endif
