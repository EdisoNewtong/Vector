#ifndef PARSER_H
#define PARSER_H

#include <unordered_map>
#include <string>
#include "commonEnum.h"
#include "tokenParserBase.h"
#include "tokenInfo.h"
#include "parsedCharInfo.h"
#include "tokenListAnalyzer.h"


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

	  static std::string getstrParserType(E_PaserType tp);
	  void checkEndLogic();
	  
protected:
	  const char* m_buf;
      size_t m_size;

      TokenParserBase* m_defaultParser;    // default
	  TokenParserBase* m_currentParser;    // current
	  E_PaserType      m_currentPaserType;

	  TokenListAnalyzer m_TokenListAnalyzer;

	  ParsedCharInfo       m_pInfo;
};


#endif
