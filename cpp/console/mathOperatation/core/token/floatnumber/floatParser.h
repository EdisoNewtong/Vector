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

	virtual void init() override;
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo, std::list<TokenInfo*>* pTokenList) override;
	virtual TokenInfo* generateToken() override;
protected:
};

#endif
