#ifndef HEX_PARSER_H
#define HEX_PARSER_H

#include <unordered_map>
#include "tokenParserBase.h"

#include "parserInfo.h"

class HexParser : public TokenParserBase
{
public:
	HexParser();
	virtual ~HexParser();

	virtual void init();
	virtual E_PaserType appendContent(char ch, ParserInfo* pInfo);
protected:
};

#endif



