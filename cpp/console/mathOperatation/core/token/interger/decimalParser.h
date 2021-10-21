#ifndef DECIMAL_PARSER_H
#define DECIMAL_PARSER_H

#include <unordered_map>
#include "tokenParserBase.h"

class DecimalParser : public TokenParserBase
{
public:
	DecimalParser();
	virtual ~DecimalParser();

	virtual void init();
	virtual E_PaserType  appendContent(char ch);
protected:
};

#endif

