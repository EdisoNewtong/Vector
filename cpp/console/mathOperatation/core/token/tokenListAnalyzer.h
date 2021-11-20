#ifndef TOKEN_LIST_ANALYZER_H
#define TOKEN_LIST_ANALYZER_H

#include <list>
#include "tokenInfo.h"
#include "commonEnum.h"

class TokenListAnalyzer
{
public:
	TokenListAnalyzer();
	virtual ~TokenListAnalyzer();

	TokenInfo* getPreviousValidToken();
	bool checkParserIsValid(E_PaserType tp);
	bool pushToken(TokenInfo* pToken);
protected:
	std::list<TokenInfo*> m_tokenList;
};

#endif
