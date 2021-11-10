#ifndef HEX_PARSER_H
#define HEX_PARSER_H

#include <unordered_map>
#include "tokenParserBase.h"

#include "parsedCharInfo.h"

class HexParser : public TokenParserBase
{
public:
	HexParser(E_PaserType tp);
	virtual ~HexParser();

	virtual void init() override;
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo, std::list<TokenInfo*>* pTokenList) override;
	virtual TokenInfo* generateToken() override;
protected:
};

#endif



