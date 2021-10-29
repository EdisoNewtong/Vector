#ifndef FLOAT_PARSER_H
#define FLOAT_PARSER_H

#include <unordered_map>
#include "tokenParserBase.h"

#include "parsedCharInfo.h"

class FloatParser : public TokenParserBase
{
public:
	FloatParser(E_PaserType tp);
	virtual ~FloatParser();

	virtual void init();
	virtual E_PaserType  appendContent(char ch, ParsedCharInfo* pInfo);
	virtual TokenInfo* generateToken();
protected:
};

#endif
