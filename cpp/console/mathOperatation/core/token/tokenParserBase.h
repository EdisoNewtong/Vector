#ifndef TOKEN_PARSER_BASE_H
#define TOKEN_PARSER_BASE_H


#include <unordered_map>
#include <string>
#include "commonEnum.h"

#include "parsedCharInfo.h"
#include "charInfo.h"
#include "tokenInfo.h"


//
// Base Class
//
class TokenParserBase
{
public:
	TokenParserBase(E_PaserType tp);
	virtual ~TokenParserBase();


	virtual void init();
	virtual E_PaserType appendContent(char ch, ParsedCharInfo* pInfo);
	virtual TokenInfo* generateToken();

	std::pair< std::unordered_map<char, CharInfo>::iterator, bool> isValidChar(char ch);
	std::pair< std::unordered_map<char, CharInfo>::iterator, bool> commonCheck(char ch, ParsedCharInfo* pInfo);

	void transferToken(TokenParserBase* pBase);
	E_PaserType getType();
	std::string getToken();
	void markBeginTag(ParsedCharInfo* pInfo);
	// void markEndTag(CharInfo* pInfo);
protected:
	std::unordered_map<char, CharInfo> m_CharSet;

	std::unordered_map<char, CharInfo> m_AllAvalibleCharacters;

	std::string m_alreadyTravelsaledString;
	std::string m_token;

	E_PaserType m_type;
	bool m_isValidEnd;

	ParsedCharInfo m_beginInfo;
	ParsedCharInfo m_endInfo;
};

#endif
