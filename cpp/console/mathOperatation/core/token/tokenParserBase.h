#ifndef TOKEN_PARSER_BASE_H
#define TOKEN_PARSER_BASE_H


#include <unordered_map>
#include <string>
#include "commonEnum.h"

#include "parserInfo.h"
#include "charInfo.h"


//
// Base Class
//
class TokenParserBase
{
public:
	TokenParserBase(E_PaserType tp);
	virtual ~TokenParserBase();


	virtual void init();
	virtual E_PaserType appendContent(char ch, ParserInfo* pInfo);

	std::pair< std::unordered_map<char, CharInfo>::iterator, bool> isValidChar(char ch);
	std::pair< std::unordered_map<char, CharInfo>::iterator, bool> commonCheck(char ch, ParserInfo* pInfo);

	void transferToken(TokenParserBase* pBase);
	E_PaserType getType();
protected:
	std::unordered_map<char, CharInfo> m_CharSet;

	std::unordered_map<char, CharInfo> m_AllAvalibleCharacters;

	std::string m_alreadyTravelsaledString;
	std::string m_token;

	E_PaserType m_type;
};

#endif
