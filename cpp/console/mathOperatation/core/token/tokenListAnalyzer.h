#ifndef TOKEN_LIST_ANALYZER_H
#define TOKEN_LIST_ANALYZER_H

#include <unordered_map>
#include <list>
#include "charUtil.h"
#include "parsedCharInfo.h"
#include "tokenInfo.h"

class TokenListAnalyzer
{
public:
	TokenListAnalyzer();
	virtual ~TokenListAnalyzer();

	bool isListEmpty();
	// bool checkParserIsValid(E_PaserType tp);
	int pushToken(TokenInfo* pToken);

	int  evaluateExp();
	void clearEvaluateList();
protected:
	std::list<TokenInfo*> m_tokenList;
	std::unordered_map<unsigned int,unsigned int> m_banPickCfgMap;

	std::list<TokenInfo*> m_evaluateSuffixExpression;
	std::list< std::pair<TokenInfo*, OperatorBaseInfo*> > m_operatorStack;


	TokenInfo* getPreviousToken(bool needSkipBlankComment, int* pHasSkipBlankComment, bool needBack2LastButOne);
	void judgeTokenIsPositiveOrNegativeAndReset(TokenInfo* pToken);
	bool hasMathedOpenParentheseBefore();
	OperatorBaseInfo* token2Op(TokenInfo* pToken);
	void tryPopPreviousOperatorLowerPriority(TokenInfo* pToken);
	void popUtilOpenParenthese();

	void initBanPickCfg();

	unsigned int genFlag(E_TokenType frontToken, E_TokenType genToken);
	unsigned int genBanPickMask(unsigned int noContinuedFlag, unsigned int continuedFlag);

	std::string  getTokenName(E_TokenType tp);
	bool isOperatorType(E_TokenType tp);
};

#endif
