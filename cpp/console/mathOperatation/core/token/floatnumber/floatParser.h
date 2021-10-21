#ifndef FLOAT_PARSER_H
#define FLOAT_PARSER_H

#include <unordered_map>
#include "tokenParserBase.h"

class FloatParser : public TokenParserBase
{
public:
	FloatParser();
	virtual ~FloatParser();

	virtual void init();
	virtual E_PaserType  appendContent(char ch);
protected:
};

#endif
