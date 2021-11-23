#ifndef OCTAL_PARSER_H
#define OCTAL_PARSER_H

#include <unordered_map>
#include "decimalParser.h"
#include "parsedCharInfo.h"

class OctalParser : public DecimalParser
{
public:
	OctalParser(E_PaserType tp);
	virtual ~OctalParser();

	virtual void init() override;
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo) override;
	virtual bool isEnd(ParsedCharInfo* pInfo) override;
protected:
	bool isOctBufValid(std::string& errMsg);
	bool isInnerOctCode(char ch);
};

#endif


