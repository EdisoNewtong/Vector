#ifndef BLANK_PARSER_H
#define BLANK_PARSER_H


#include "tokenParserBase.h"
#include "parsedCharInfo.h"

class BlankParser : public TokenParserBase
{
public:
	BlankParser(E_PaserType tp);
	virtual ~BlankParser();

	virtual void init() override;
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo) override;

	virtual bool isEnd(ParsedCharInfo* pInfo) override;
};

#endif