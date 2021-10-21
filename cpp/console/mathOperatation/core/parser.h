#ifndef PARSER_H
#define PARSER_H

#include <unordered_map>
#include "commonEnum.h"
#include "tokenParserBase.h"
#include "parserInfo.h"


class Parser
{
public:
      Parser();
      virtual ~Parser();

	  void setContent(const char* buf, size_t sz);
	  int doParse();
protected:
	  void processLineInfo(char ch, size_t idx);
protected:
	  const char* m_buf;
      size_t m_size;

	  // std::unordered_map<E_PaserType, TokenParserBase*> m_parserMap;
	  std::unordered_map<int, TokenParserBase*> m_parserMap;

      TokenParserBase* m_defaultParser;    // default
	  TokenParserBase* m_currentParser;    // current

	  E_PaserType      m_currentPaserType;

	  ParserInfo       m_pInfo;



};


#endif
