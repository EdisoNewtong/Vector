#ifndef OPERATOR_PARSER_H
#define OPERATOR_PARSER_H

#include <unordered_map>
#include <string>
#include "tokenParserBase.h"
#include "commonEnum.h"

class OperatorParser : public TokenParserBase
{
public: 
	OperatorParser(E_PaserType tp);
	virtual ~OperatorParser();

	virtual void init();
	virtual E_PaserType appendContent(char ch, ParserInfo* pInfo);
};

#endif
