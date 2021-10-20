#ifndef FLOAT_PARSER_H
#define FLOAT_PARSER_H

#include <unordered_map>
#include "commonEnum.h"

class FloatParser
{
public:
	FloatParser();
	virtual ~FloatParser();
protected:
	  std::unordered_map<char, E_ChType> m_AllAvalibleCharacters;
};

#endif
