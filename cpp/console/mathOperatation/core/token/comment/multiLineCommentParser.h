#ifndef  MULTI_LINE_COMMENT_PARSER_H
#define  MULTI_LINE_COMMENT_PARSER_H

#include "tokenParserBase.h"

#include "parsedCharInfo.h"

class MultiLineCommentParser : public TokenParserBase
{
public:
	MultiLineCommentParser(E_PaserType tp);
	virtual ~MultiLineCommentParser();

	virtual void init() override;
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo) override;
	// virtual TokenInfo* generateToken() override;

	virtual bool isEnd(ParsedCharInfo* pInfo) override;
protected:
	int m_warningCnt;
};


#endif

