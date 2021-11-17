#ifndef DECIMAL_PARSER_H
#define DECIMAL_PARSER_H

#include <unordered_map>
#include "tokenParserBase.h"

#include "parsedCharInfo.h"

class DecimalParser : public TokenParserBase
{
public:
	DecimalParser(E_PaserType tp);
	virtual ~DecimalParser();

	virtual void init() override;
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo) override;
	virtual TokenInfo* generateToken() override;
protected:
};

#endif

