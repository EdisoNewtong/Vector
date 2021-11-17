#ifndef OCTAL_PARSER_H
#define OCTAL_PARSER_H

#include <unordered_map>
#include "tokenParserBase.h"
#include "parsedCharInfo.h"

class OctalParser : public TokenParserBase
{
public:
	OctalParser(E_PaserType tp);
	virtual ~OctalParser();

	virtual void init() override;
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo) override;
	virtual TokenInfo* generateToken() override;
protected:
};

#endif


