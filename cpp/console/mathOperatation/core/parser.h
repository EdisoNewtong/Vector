#ifndef PARSER_H
#define PARSER_H

#include <list>
#include <unordered_map>
#include <string>
#include "commonEnum.h"
#include "tokenParserBase.h"
#include "tokenInfo.h"
#include "parsedCharInfo.h"


class Parser
{
public:
      Parser();
      virtual ~Parser();

	  bool analyzeOptionArgs(const std::list<std::string>& argList, std::string& errorMsg);
	  std::string getUserManual();

	  void setContent(const char* buf, size_t sz);
	  int doParse();

protected:
	  void prepareOptionKeyWord();

	  void processLineInfo(char ch, size_t idx);
	  bool checkPreviousTokenIsValid();

	  static std::string getstrParserType(E_PaserType tp);
	  
protected:
	  const char* m_buf;
      size_t m_size;
	  int    m_debugOption;
	  int    m_flag;

	  // std::unordered_map<int, TokenParserBase*> m_parserMap;
	  std::list<TokenInfo*>                     m_tokenList;

      TokenParserBase* m_defaultParser;    // default
	  TokenParserBase* m_currentParser;    // current

	  E_PaserType      m_currentPaserType;

	  ParsedCharInfo       m_pInfo;

private:
	  std::string m_optKeyWordprefix_1;
	  std::string m_optKeyWord_1;
	  std::string m_optKeyWordprefix_2;
	  std::string m_optKeyWord_2;
};


#endif
