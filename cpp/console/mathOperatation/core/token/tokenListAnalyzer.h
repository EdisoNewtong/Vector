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

	void expValidCheck();
	int  evaluateExp();
	void clearEvaluateList();
protected:
	std::list<TokenInfo*> m_tokenList;
	std::unordered_map<unsigned int,unsigned int> m_banPickCfgMap;

	std::list<TokenInfo*> m_evaluateSuffixExpression;
	std::list< std::pair<TokenInfo*, OperatorBaseInfo*> > m_operatorStack;

	std::list<TokenInfo*> m_tmpMiddleValue;

	int m_lastSemicolonPosition;

	TokenInfo* getPreviousToken(bool needSkipBlankComment, int* pHasSkipBlankComment, bool needBack2LastButOne);
	void judgeTokenIsPositiveOrNegativeAndReset(TokenInfo* pToken);
	TokenInfo* hasOpenParentheseBefore();
	void tryPopPreviousOperatorLowerPriority(TokenInfo* pToken);
	void popUtilOpenParenthese();

	void generateTmpMiddleBinaryToken(TokenInfo* left, TokenInfo* op, TokenInfo* right);
	  void doAdd(TokenInfo* left,  TokenInfo* right);
	  void doMinus(TokenInfo* left,  TokenInfo* right);
	  void doMultiply(TokenInfo* left,  TokenInfo* right);
	  void doDivide(TokenInfo* left,  TokenInfo* right);
	  void doMod(TokenInfo* left,  TokenInfo* right);
	  void doBitAnd(TokenInfo* left,  TokenInfo* right);
	  void doBitOr(TokenInfo* left,  TokenInfo* right);
	  void doBitXor(TokenInfo* left, TokenInfo* right);
	  void doBitLeftShift(TokenInfo* left,  TokenInfo* right);
	  void doBitRightShift(TokenInfo* left,  TokenInfo* right);
	  void doAssignment(TokenInfo* left,  TokenInfo* right);

	void generateTmpMiddleUnaryToken(TokenInfo* op, TokenInfo* right);
	  void doPositive(TokenInfo* token);
	  void doNegative(TokenInfo* token);
	  void doBitNot(TokenInfo* token);

	OperatorBaseInfo* token2Op(TokenInfo* pToken);

	void initBanPickCfg();

	unsigned int genFlag(E_TokenType frontToken, E_TokenType genToken);
	unsigned int genBanPickMask(unsigned int noContinuedFlag, unsigned int continuedFlag);

	bool isOperatorType(E_TokenType tp);


};

#endif
