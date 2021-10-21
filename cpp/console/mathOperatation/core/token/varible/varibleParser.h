#ifndef VARIBLE_PARSER_H
#define VARIBLE_PARSER_H

#include <unordered_map>
#include "tokenParserBase.h"

class VaribleParser : public TokenParserBase
{
public:
	VaribleParser();
	virtual ~VaribleParser();

	virtual void init();
	virtual E_PaserType  appendContent(char ch);
protected:
};

#endif
