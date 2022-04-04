#ifndef HEX_PARSER_H
#define HEX_PARSER_H

#include <unordered_map>
#include "decimalParser.h"

#include "parsedCharInfo.h"

class HexParser : public DecimalParser
{
public:
	HexParser(E_PaserType tp);
	virtual ~HexParser();

	virtual void init() override;
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo) override;
	virtual bool isEnd(ParsedCharInfo* pInfo) override;
	virtual bool isTokenValid() override;

    virtual bool checkIsInsideRange() override;
protected:
	bool isHexCode(char ch);
};

#endif



