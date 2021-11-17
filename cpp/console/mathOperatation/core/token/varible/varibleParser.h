#ifndef VARIBLE_PARSER_H
#define VARIBLE_PARSER_H

#include <unordered_map>
#include "tokenParserBase.h"

#include "parsedCharInfo.h"

class VaribleParser : public TokenParserBase
{
public:
	VaribleParser(E_PaserType tp);
	virtual ~VaribleParser();

	virtual void init() override;
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo) override;
	virtual TokenInfo* generateToken() override;

	virtual bool isEnd() override;
protected:
};

#endif
