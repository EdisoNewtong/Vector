#ifndef OCTAL_PARSER_H
#define OCTAL_PARSER_H

#include <unordered_map>
#include "tokenParserBase.h"
#include "parserInfo.h"

class OctalParser : public TokenParserBase
{
public:
	OctalParser(E_PaserType tp);
	virtual ~OctalParser();

	virtual void init();
	virtual E_PaserType  appendContent(char ch, ParserInfo* pInfo);
	virtual TokenInfo* generateToken();
protected:
};

#endif


