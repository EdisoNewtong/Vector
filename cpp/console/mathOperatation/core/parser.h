#ifndef PARSER_H
#define PARSER_H

#include <list>
#include <unordered_map>
#include "commonEnum.h"
#include "tokenParserBase.h"
#include "tokenInfo.h"
#include "parsedCharInfo.h"


class Parser
{
public:
      Parser();
      virtual ~Parser();

	  void setContent(const char* buf, size_t sz);
	  int doParse();
protected:
	  void processLineInfo(char ch, size_t idx);
	  bool checkPreviousTokenIsValid();
protected:
	  const char* m_buf;
      size_t m_size;

	  // std::unordered_map<E_PaserType, TokenParserBase*> m_parserMap;
	  std::unordered_map<int, TokenParserBase*> m_parserMap;
	  std::list<TokenInfo*>                     m_tokenList;

      TokenParserBase* m_defaultParser;    // default
	  TokenParserBase* m_currentParser;    // current

	  E_PaserType      m_currentPaserType;

	  ParsedCharInfo       m_pInfo;
};


#endif
