#ifndef HEX_PARSER_H
#define HEX_PARSER_H

#include <unordered_map>
#include "commonEnum.h"

class HexParser
{
public:
	HexParser();
	virtual ~HexParser();
protected:
	  std::unordered_map<char, E_ChType> m_AllAvalibleCharacters;
};

#endif



