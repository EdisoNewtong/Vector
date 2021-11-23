#ifndef TOKEN_LIST_ANALYZER_H
#define TOKEN_LIST_ANALYZER_H

#include <unordered_map>
#include <list>
#include "tokenInfo.h"
#include "commonEnum.h"

class TokenListAnalyzer
{
public:
	TokenListAnalyzer();
	virtual ~TokenListAnalyzer();

	TokenInfo* getPreviousToken(bool needSkipBlankComment, int* pHasSkipBlankComment);

	bool isListEmpty();
	bool checkParserIsValid(E_PaserType tp);
	bool pushToken(TokenInfo* pToken);
protected:
	std::list<TokenInfo*> m_tokenList;
	std::unordered_map<unsigned int,unsigned int> m_banPickCfgMap;

	unsigned int genFlag(E_TokenType frontToken, E_TokenType genToken);
	unsigned int genBanPickMask(bool noContinuedFlag, bool continuedFlag);
	void initBanPickCfg();

};

#endif
