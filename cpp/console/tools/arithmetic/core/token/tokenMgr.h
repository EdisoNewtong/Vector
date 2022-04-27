#ifndef TOKEN_MGR_H
#define TOKEN_MGR_H


#include "tokenBase.h"
#include "commonEnum.h"
#include "variblePool.h"
#include "dataValue.h"
#include "dataTypeUtil.h"
#include <string>
// #include <stack>
#include <list>
#include <vector>
#include <unordered_map>
#include <utility>

#define OPERATOR_CNT       14
/*
   + - * / %     : 5
   & | ^ ~ << >> : 6
   ( ) =         : 3
----------------------
 Total          =  14 = ( 5 + 6 + 3 )

*/ 

class TokenMgr
{
protected:
    struct OpPairCfg
    {
        E_OperatorType left;
        E_OperatorType right;
        bool           closeAvaliable;
        bool           seperateAvaliable;
    };

public:
    static constexpr int s_TABLE_SIZE = OPERATOR_CNT * OPERATOR_CNT;

    // Singleton Instance
    static void init();
    static void release();
    static TokenMgr* getInstance();


public:
    void pushToken(TokenBase* pToken);
    std::pair<bool,std::string> isLastValidTokenSemicolonOrEmpty();
protected:
    TokenMgr();
    virtual ~TokenMgr();
    static bool isCommentType(E_TokenType tp);
    static bool isBlankType(E_TokenType tp);
    static bool isIgnoredType(E_TokenType tp);

    bool hasPreviousExistOpenParentheses();
    std::pair<bool,TokenBase*>        checkTokenValidFromPreviousRelationship(TokenBase* toBePushed);
    std::pair<TokenBase*,TokenBase*>  getPreviousToken();

    E_DataType checkPrefixKeyWordsAndGetDataType(int varIdx, std::string& varname);
    void executeCode();
    void buildSuffixExpression(int sentenceType, VaribleInfo* pVarible, int varibleIdx);
    void checkSuffixExpressionValid();
    void evaluateSuffixExpression();
    void processOperatorStack(TokenBase* previousToken, TokenBase* pToken);
    void popAllOperatorStack();

    void popUntilOpenParentheses();


    TokenBase* doBinaryOp(TokenBase* left, TokenBase* op, TokenBase* right);
    TokenBase* doUnaryOp(TokenBase* op, TokenBase* right);

    // Each operator  implementation 
    //
    //      Binary Op
    TokenBase* doAdd(TokenBase* left, TokenBase* right);
    TokenBase* doMinus(TokenBase* left, TokenBase* right);
    TokenBase* doMultiply(TokenBase* left, TokenBase* right);
    TokenBase* doDivide(TokenBase* left, TokenBase* right);
    TokenBase* doMod(TokenBase* left, TokenBase* right);
    TokenBase* doBitAnd(TokenBase* left, TokenBase* right);
    TokenBase* doBitOr(TokenBase* left, TokenBase* right);
    TokenBase* doBitXor(TokenBase* left, TokenBase* right);
    TokenBase* doBitLeftShift(TokenBase* left, TokenBase* right);
    TokenBase* doBitRightShift(TokenBase* left, TokenBase* right);
    TokenBase* doAssignment(TokenBase* left, TokenBase* right);

    //      Unary Op
    TokenBase* doPositive(TokenBase* op, TokenBase* right);
    TokenBase* doNegative(TokenBase* op, TokenBase* right);
    TokenBase* doBitNot(TokenBase* op, TokenBase* right);

    E_DataType operatorPrepairDataTypeConversion1(DataValue* pRightVal);
    E_DataType operatorPrepairDataTypeConversion2(DataValue* pLeftVal, DataValue* pRightVal);

    // log function
    void printOperatorStack();
    void printSuffixExpression(int flag);

    void traceOperatorStack(TokenBase* pToken, bool push);
    void traceSuffixExpression(TokenBase* pToken, bool push);
    void tracePositiveNegativeFlag(TokenBase* pToken, E_OperatorType op);

    void tracePushedTokenWarning(TokenBase* pToken);
    void tracebitShiftWarning(TypeBaseInfo& leftTpInfo,const std::string& lExpr,  TypeBaseInfo& rightTpInfo, const std::string& rExpr,  DataValue& rightVal);

protected:
    static TokenMgr* s_gInstance;

    static const OpPairCfg s_OpPairCfgTable[s_TABLE_SIZE];
    static const int s_MAX_KEYWORDS_CNT;
    static const std::unordered_map<std::string, E_DataType> s_keywordsDataTypeMap;
    static std::list<TokenBase*> s_generatedTmpTokenPool;

    static TokenBase* generateTmpExpression(E_DataType dt, const std::string& expression, TokenBase* begtoken, TokenBase* endtoken);
    static void clearTmpGenTokenPool();



protected:
    std::vector<TokenBase*> m_allTokenList;
    std::vector<TokenBase*> m_validTokenList;
    std::vector<TokenBase*> m_oneSentence;

    // std::stack<TokenBase*>  m_opertorStack;
    std::list<TokenBase*>   m_opertorStack;
    std::list<TokenBase*>   m_suffixExpression;

    int m_execodeIdx;

};

#endif
