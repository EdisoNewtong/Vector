#ifndef BLANK_PARSER_H
#define BLANK_PARSER_H


#include "tokenParserBase.h"

#include "parserInfo.h"
#include "charInfo.h"

class BlankParser : public TokenParserBase
{
public:
	BlankParser(E_PaserType tp);
	virtual ~BlankParser();

	virtual void init();
	virtual E_PaserType  appendContent(char ch, ParserInfo* pInfo);
	virtual TokenInfo* generateToken();
};

#endif
