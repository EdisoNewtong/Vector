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

		E_NATURALLY_TERMINAL_GEN_TOKEN_AND_PUSH,      // e.g. comment is end with     */    or   // axxx\n
		E_TRANSFER_NON_DEFAULT_PARSER_WAIT_NEXT_CHAR, // start with /*  :     operator parser ->  comment parser 
		E_TRANSFER_DEFAULT_PARSER_AND_DO_REPARSE,     // \t\t123  \t\ta : parser char : "1"  or parser char : "a"
	};

	TokenParserBase(E_PaserType tp);
	virtual ~TokenParserBase();

	virtual void init();
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo, std::list<TokenInfo*>* pTokenList);
	virtual TokenInfo* generateToken();
	virtual void reset();
	virtual bool isEnd();

	CharBaseInfo* getInsideCharSetBaseInfo(char ch);
	CharBaseInfo* commonCheck(char ch, ParsedCharInfo* pInfo);

	void transferToken(TokenParserBase* pBase);
	E_PaserType getType();
	std::string getToken();
	void markBeginTag(ParsedCharInfo* pInfo);
	E_TERMINAL_STATUS getSwitchFlag();
protected:
	std::unordered_map<char, CharBaseInfo*> m_AllAvalibleCharacters;

	std::string m_alreadyTravelsaledString;
	std::string m_token;

	E_PaserType m_type;
	bool m_isValidEnd;

	E_TERMINAL_STATUS  m_switchFlag;
	
	ParsedCharInfo m_beginInfo;
	ParsedCharInfo m_endInfo;
};

#endif
