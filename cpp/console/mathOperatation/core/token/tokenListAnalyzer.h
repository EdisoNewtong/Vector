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


	int m_lastSemicolonPosition;

	TokenInfo* getPreviousToken(bool needSkipBlankComment, int* pHasSkipBlankComment, bool needBack2LastButOne);
	void judgeTokenIsPositiveOrNegativeAndReset(TokenInfo* pToken);
	TokenInfo* hasOpenParentheseBefore();
	void tryPopPreviousOperatorLowerPriority(TokenInfo* pToken);
	void popUtilOpenParenthese();

    TokenInfo* generateTmpMiddleBinaryToken(TokenInfo* left, TokenInfo* op, TokenInfo* right);
    void decideFinalExpDataTypeFor2Operands(TokenInfo* left, TokenInfo* op, TokenInfo* right);
	  TokenInfo* doAdd(TokenInfo* left,  TokenInfo* right);
	  TokenInfo* doMinus(TokenInfo* left,  TokenInfo* right);
	  TokenInfo* doMultiply(TokenInfo* left,  TokenInfo* right);
	  TokenInfo* doDivide(TokenInfo* left,  TokenInfo* right);
	  TokenInfo* doMod(TokenInfo* left,  TokenInfo* right);
	  TokenInfo* doBitAnd(TokenInfo* left,  TokenInfo* right);
	  TokenInfo* doBitOr(TokenInfo* left,  TokenInfo* right);
	  TokenInfo* doBitXor(TokenInfo* left, TokenInfo* right);
	  TokenInfo* doBitLeftShift(TokenInfo* left,  TokenInfo* right);
	  TokenInfo* doBitRightShift(TokenInfo* left,  TokenInfo* right);
	  TokenInfo* doAssignment(TokenInfo* left,  TokenInfo* right);

	TokenInfo* generateTmpMiddleUnaryToken(TokenInfo* op, TokenInfo* right);
    void decideFinalExpDataTypeFor1Operands(TokenInfo* right);
	  TokenInfo* doPositive(TokenInfo* token);
	  TokenInfo* doNegative(TokenInfo* token);
	  TokenInfo* doBitNot(TokenInfo* token);

	OperatorBaseInfo* token2Op(TokenInfo* pToken);

	void initBanPickCfg();

	unsigned int genFlag(E_TokenType frontToken, E_TokenType genToken);
	unsigned int genBanPickMask(unsigned int noContinuedFlag, unsigned int continuedFlag);

	bool isOperatorType(E_TokenType tp);


};

#endif
