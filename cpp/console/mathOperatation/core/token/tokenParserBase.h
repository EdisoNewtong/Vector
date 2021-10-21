#ifndef TOKEN_PARSER_BASE_H
#define TOKEN_PARSER_BASE_H


#include <unordered_map>
#include <string>
#include "commonEnum.h"

//
// Base Class
//
class TokenParserBase
{
public:
	TokenParserBase();
	virtual ~TokenParserBase();


	virtual void init();
	virtual E_PaserType appendContent(char ch);

	std::pair< std::unordered_map<char, E_ChType>::iterator, bool> isValidChar(char ch);
	void clearTravelsaled();
protected:
	std::unordered_map<char, E_ChType> m_CharSet;

	std::unordered_map<char, E_ChType> m_AllAvalibleCharacters;

	std::string m_alreadyTravelsaledString;
};

#endif
