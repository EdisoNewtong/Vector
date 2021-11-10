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
	TokenParserBase(E_PaserType tp);
	virtual ~TokenParserBase();


	virtual void init();
	virtual E_PaserType appendContent(ParsedCharInfo* pInfo, std::list<TokenInfo*>* pTokenList);
	virtual TokenInfo* generateToken();

	CharBaseInfo* isInsideCharSet(char ch);
	CharBaseInfo* commonCheck(char ch, ParsedCharInfo* pInfo);

	void transferToken(TokenParserBase* pBase);
	E_PaserType getType();
	std::string getToken();
	void markBeginTag(ParsedCharInfo* pInfo);
protected:
	std::unordered_map<char, CharBaseInfo*> m_AllAvalibleCharacters;

	std::string m_alreadyTravelsaledString;
	std::string m_token;

	E_PaserType m_type;
	bool m_isValidEnd;

	ParsedCharInfo m_beginInfo;
	ParsedCharInfo m_endInfo;
};

#endif
