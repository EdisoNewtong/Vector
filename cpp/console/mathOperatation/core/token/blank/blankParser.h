#ifndef BLANK_PARSER_H
#define BLANK_PARSER_H


#include "tokenParserBase.h"

#include "parserInfo.h"

class BlankParser : public TokenParserBase
{
public:
	BlankParser();
	virtual ~BlankParser();

	virtual void init();
	virtual E_PaserType  appendContent(char ch, ParserInfo* pInfo);
};

#endif
