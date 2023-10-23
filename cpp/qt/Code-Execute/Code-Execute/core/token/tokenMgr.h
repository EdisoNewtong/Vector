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

#define OPERATOR_CNT       24
//    + - * / %     : 5
//    & | ^ ~ << >> : 6
//    ( ) =         : 3
//    += -= *= /* %=     &= |= ^=    <<=  >>=    10
// ----------------------
//  Total          =  24 = ( 5 + 6 + 3 + 10)
//


#include <QString>
// #include <QPlainTextEdit>

class QPlainTextEdit;

class TokenMgr
{
protected:
    struct OpPairCfg
    {
        OpPairCfg(bool flag1, bool flag2) : closeAvaliable(flag1), seperateAvaliable(flag2) { }

        // E_OperatorType left;
        // E_OperatorType right;

        bool           closeAvaliable;    //   permission for  the 2 operators are adjacent without any Space or Comment
        bool           seperateAvaliable; //   permission for  the 2 operators are not adjacent but there is at least some Space or Comment token in the middle
    };

public:
    static constexpr int s_TABLE_SIZE = OPERATOR_CNT * OPERATOR_CNT;

    // Singleton Instance
    static void init();
    static void release();
    static TokenMgr* getInstance();


public:
    // Core Core Core : Key Logic
    void pushToken(TokenBase* pToken);
    std::pair<bool,std::string> isLastValidTokenSemicolonOrEmpty();

    // Scan all token inside the whole list, and decide whether it is full of comment(s) or blank(s)
    bool isAllTokensTrivial();

    static void setUnInitializedVaribleAsError(bool flag);
    static void setNeedTreatBlankStatementAsWarning(bool flag);

    void setTextEdit(QPlainTextEdit* pEdit);
protected:
    TokenMgr();
    virtual ~TokenMgr();
    static bool isCommentType(TokenBase* pToken);
    static bool isBlankType(TokenBase* pToken);
    static bool isIgnoredType(TokenBase* pToken);

    bool hasPreviousExistOpenParentheses();
    std::pair<bool,TokenBase*>        checkAdjacentTokensRelationship_IsValid(TokenBase* toBePushed);
    std::pair<TokenBase*,TokenBase*>  getPreviousToken();

    bool process_SemicolonWithPriorToken(TokenBase* toBePushed, TokenBase* priorToken);
    bool process_SequenceWithPriorToken(TokenBase* toBePushed, TokenBase* priorToken);
    bool process_OperatorWithPriorToken(TokenBase* toBePushed, TokenBase* priorToken, TokenBase* priorClosestToken);

    // for tmp expression check use
    bool is1stTokenTypeKeyWord();

    E_DataType checkPrefixKeyWordsAndGetDataType(int varIdx, std::string& varname, bool hasEqual);
    void executeCode();


    /********************
     * Core Core Core   *
     ********************/
    void buildSuffixExpression(int sentenceType, int varibleIdx);

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

    TokenBase* do_Add_Assignment(TokenBase* left, TokenBase* right);
    TokenBase* do_Minus_Assignment(TokenBase* left, TokenBase* right);
    TokenBase* do_Multiply_Assignment(TokenBase* left, TokenBase* right);
    TokenBase* do_Divide_Assignment(TokenBase* left, TokenBase* right);
    TokenBase* do_Mod_Assignment(TokenBase* left, TokenBase* right);
    TokenBase* do_Bit_And_Assignment(TokenBase* left, TokenBase* right);
    TokenBase* do_Bit_Or_Assignment(TokenBase* left, TokenBase* right);
    TokenBase* do_Bit_Xor_Assignment(TokenBase* left, TokenBase* right);
    TokenBase* do_Bit_LeftShift_Assignment(TokenBase* left, TokenBase* right);
    TokenBase* do_Bit_RightShift_Assignment(TokenBase* left, TokenBase* right);



    //      Unary Op
    TokenBase* doPositive(TokenBase* op, TokenBase* right);
    TokenBase* doNegative(TokenBase* op, TokenBase* right);
    TokenBase* doBitNot(TokenBase* op, TokenBase* right);

    E_DataType operatorPrepairDataTypeConversion1(DataValue* pRightVal);
    E_DataType operatorPrepairDataTypeConversion2(DataValue* pLeftVal, DataValue* pRightVal);

    // log function
    QString printOperatorStack();
    QString printSuffixExpression(int flag);

    QString traceOperatorStack(TokenBase* pToken, bool push);
    QString traceSuffixExpression(TokenBase* pToken, bool push);

    // move from OperatorStack to  -->   SuffixExpression
    // pop from OpStack , push it into SuffixExpression list
    QString traceOpMove2SuffixExpression(TokenBase* pToken);
    QString traceSomeTokensFromOpMove2SuffixExpression(const std::list<TokenBase*>& lst, bool specialFlag);


    QString tracePositiveNegativeFlag(TokenBase* pToken, E_OperatorType op);

    QString traceNegativeOperation(TokenBase* right);

    QString tracePushedTokenWarning(TokenBase* pToken);
    QString tracebitShiftWarning(bool isLeftBitShift, TokenBase* left,  TokenBase* right);

    QString traceTmpOpResult(const std::string& expr, DataValue& retValue);
    QString traceUnInitializedVarWhenUsed(TokenBase* pToken);
    QString traceAssignOverFlow(TokenBase* leftVar, TokenBase* rightFixedInt);

    QString traceBlankStatement();
protected:
    static TokenMgr* s_gInstance;

    // static const OpPairCfg s_OpPairCfgTable[OPERATOR_CNT * OPERATOR_CNT];
    static const OpPairCfg s_OpPairCfgTable[OPERATOR_CNT][OPERATOR_CNT];

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

    QPlainTextEdit*          m_pTextEdit;

};

#endif
