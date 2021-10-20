#ifndef VARIBLE_PARSER_H
#define VARIBLE_PARSER_H

#include <unordered_map>
#include "commonEnum.h"

class VaribleParser
{
public:
	VaribleParser();
	virtual ~VaribleParser();

protected:
	  std::unordered_map<char, E_ChType> m_AllAvalibleCharacters;
};

#endif
