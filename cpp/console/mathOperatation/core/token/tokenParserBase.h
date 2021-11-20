#ifndef TOKEN_PARSER_BASE_H
#define TOKEN_PARSER_BASE_H


#include <unordered_map>
#include <string>
#include <list>
#include "commonEnum.h"

#include "parsedCharInfo.h"
#include "tokenInfo.h"
#include "charUtil.h"


//
// Base Class
//
class TokenParserBase
{
public:
	enum E_TERMINAL_STATUS
	{
		// e.g.
		/* */
		// or    // abc \n
		E_TERMINAL_NONE,

		// 
		//   e.g. comment is end with  */         
		//                 or   // axxx\n
		//
		E_TOKEN_TERMINATE_TO_DEFAULT,

		// 
		//   e.g.   abc+ 
		//             ^
		//   "abc" is force interrupted by character '+'
		//   so should re-parse '+'
		E_TOKEN_TERMINATE_TO_DEFAULT_RE_PARSE,                   

		//
		//   /*
		//    ^
		//   convert from opParser to    <Multi-Line Comment Parser>
		E_TOKEN_CONVERT_TO_OTHER,
	};

	TokenParserBase(E_PaserType tp);
	virtual ~TokenParserBase();

	virtual void init();
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo);
	virtual TokenInfo* generateToken();
	virtual void reset();
	virtual bool isEnd(ParsedCharInfo* pInfo);

	CharBaseInfo* getInsideCharSetBaseInfo(char ch);
	CharBaseInfo* commonCheckWithException(char ch, ParsedCharInfo* pInfo);

	void transferToken(TokenParserBase* pBase);
	E_PaserType getType();
	std::string getToken();
	void markBeginTag(const PosInfo& pInfo);
	E_TERMINAL_STATUS getSwitchFlag();
protected:
	std::unordered_map<char, CharBaseInfo*> m_AllAvalibleCharacters;

	std::string m_alreadyTravelsaledString;
	std::string m_token;

	E_PaserType m_type;
	E_TokenType m_tokenType;

	E_TERMINAL_STATUS  m_switchFlag;
	
	PosInfo m_beginInfo;
	PosInfo m_endInfo;
};

#endif
