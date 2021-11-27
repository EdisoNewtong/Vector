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


	bool isListEmpty();
	bool checkParserIsValid(E_PaserType tp);
	void pushToken(TokenInfo* pToken);
protected:
	std::list<TokenInfo*> m_tokenList;
	std::unordered_map<unsigned int,unsigned int> m_banPickCfgMap;

	TokenInfo* getPreviousToken(bool needSkipBlankComment, int* pHasSkipBlankComment, bool needBack2LastButOne);

	void initBanPickCfg();

	unsigned int genFlag(E_TokenType frontToken, E_TokenType genToken);
	unsigned int genBanPickMask(unsigned int noContinuedFlag, unsigned int continuedFlag);

	std::string  getTokenName(E_TokenType tp);
};

#endif
