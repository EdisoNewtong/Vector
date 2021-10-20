#ifndef DECIMAL_PARSER_H
#define DECIMAL_PARSER_H

#include <unordered_map>
#include "commonEnum.h"

class DecimalParser
{
public:
	DecimalParser();
	virtual ~DecimalParser();
protected:

	  std::unordered_map<char, E_ChType> m_AllAvalibleCharacters;
};

#endif

