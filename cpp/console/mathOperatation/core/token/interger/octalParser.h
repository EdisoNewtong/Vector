#ifndef OCTAL_PARSER_H
#define OCTAL_PARSER_H

#include <unordered_map>
#include "commonEnum.h"

class OctalParser
{
public:
	OctalParser();
	virtual ~OctalParser();
protected:

	  std::unordered_map<char, E_ChType> m_AllAvalibleCharacters;
};

#endif


