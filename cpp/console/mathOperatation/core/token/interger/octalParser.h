#ifndef OCTAL_PARSER_H
#define OCTAL_PARSER_H

#include <unordered_map>
#include "tokenParserBase.h"

class OctalParser : public TokenParserBase
{
public:
	OctalParser();
	virtual ~OctalParser();

	virtual void init();
	virtual E_PaserType  appendContent(char ch);
protected:
};

#endif


