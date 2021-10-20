#ifndef PARSER_H
#define PARSER_H

#include <ios>
#include <unordered_map>

#include "commonEnum.h"

class Parser
{
public:
      Parser(const char* buf, std::streamsize sz);
      virtual ~Parser();

	  int doParse();
protected:
	  const char* m_buf;
      std::streamsize m_size;

	  std::unordered_map<char, E_ChType> m_AllAvalibleCharacters;
};


#endif
