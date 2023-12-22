#ifndef TOKEN_MGR_H
#define TOKEN_MGR_H


#include "tokenBase.h"
#include "commonEnum.h"
#include "variblePool.h"
#include "dataValue.h"
#include "dataTypeUtil.h"
#include "functionBase.h"
#include <string>
// #include <stack>
#include <list>
#include <vector>
#include <unordered_map>
#include <utility>

#define OPERATOR_CNT       25
//    + - * / %     : 5
//    & | ^ ~ << >> : 6
//    ( ) =         : 3
//    += -= *= /* %=     &= |= ^=    <<=  >>=    10
//    ,             : 1
// ----------------------
//  Total          =  25 = ( 5 + 6 + 3 + 10 + 1)
//

class TokenMgr
{
protected:
    struct OpPairCfg
    {
        OpPairCfg(bool flag1, bool flag2) : closeAvaliable(flag1), seperateAvaliable(flag2) { };

        bool           closeAvaliable;    //   permission for  the 2 operators are adjacent without any Space or Comment
        bool           seperateAvaliable; //   permission for  the 2 operators are not adjacent but there is at least some Space or Comment token in the middle
    };

public:
    static constexpr int s_TABLE_SIZE = OPERATOR_CNT * OPERATOR_CNT;

    // Singleton Instance
    static void init();
    static void release();
    static TokenMgr* getInstance();

    static void setUnInitializedVaribleAsError(bool flag);
    static void setNeedTreatBlankStatementAsWarning(bool flag);
public:
    // Core Core Core : Key Logic
    void pushToken(TokenBase* pToken);
    std::pair<bool,std::string> isLastValidTokenSemicolonOrEmpty();

    // Scan all token inside the whole list, and decide whether it is full of comment(s) or blank(s)
    bool isAllTokensTrivial();

protected:
    TokenMgr();
    virtual ~TokenMgr();
    static bool isCommentType(TokenBase* pToken);
    static bool isBlankType(TokenBase* pToken);
    static bool isIgnoredType(TokenBase* pToken);

    bool hasPreviousExistOpenParenthesis();
    std::pair<bool,TokenBase*>        checkAdjacentTokensRelationship_IsValid(TokenBase* toBePushed);
    std::pair<TokenBase*,TokenBase*>  getPreviousToken();

    bool process_SemicolonWithPriorToken(TokenBase* toBePushed, TokenBase* priorToken);
    bool process_SequenceWithPriorToken(TokenBase* toBePushed, TokenBase* priorToken);
    bool process_OperatorWithPriorToken(TokenBase* toBePushed, TokenBase* priorToken, TokenBase* priorClosestToken);

    // void processPreviousFunctionRelated_IfNecessary(TokenBase* pFunctObj);
    void processParenthesisOrCommaFlag_IfNecessary(TokenBase* pTokenToBePushed, int tokenIndex, int iFlag);

    void allocNewArgumentExpr();

    // for tmp expression check use
    bool is1stTokenTypeKeyWord();

    E_DataType checkPrefixKeyWordsAndGetDataType(int varIdx, std::string& varname, bool hasEqual);
    void executeCode();

    void executeCode_New();

    /********************
     * Core Core Core   *
     ********************/
    void buildSuffixExpression(int sentenceType, int varibleIdx);

    void checkSuffixExpressionValid();
    void evaluateSuffixExpression();
    void processOperatorStack(TokenBase* previousToken, TokenBase* pToken);
    void popAllOperatorStack();

    void popUntilOpenParenthesis();


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
    void printOperatorStack();
    void printSuffixExpression(int flag);

    void traceOperatorStack(TokenBase* pToken, bool push);
    void traceSuffixExpression(TokenBase* pToken, bool push);

    // move from OperatorStack to  -->   SuffixExpression
    // pop from OpStack , push it into SuffixExpression list
    void traceOpMove2SuffixExpression(TokenBase* pToken);
    void traceSomeTokensFromOpMove2SuffixExpression(const std::list<TokenBase*>& lst, bool specialFlag);


    void tracePositiveNegativeFlag(TokenBase* pToken, E_OperatorType op);

    void traceNegativeOperation(TokenBase* right);

    void tracePushedTokenWarning(TokenBase* pToken);
    void tracebitShiftWarning(bool isLeftBitShift, TokenBase* left,  TokenBase* right);

    void traceTmpOpResult(const std::string& expr, DataValue& retValue);
    void traceUnInitializedVarWhenUsed(TokenBase* pToken);
    void traceAssignOverFlow(TokenBase* leftVar, TokenBase* rightFixedInt);

    void traceBlankStatement();

    void clearOneArgumentsPool();
protected:
    static TokenMgr* s_gInstance;

    static const OpPairCfg s_OpPairCfgTable[OPERATOR_CNT][OPERATOR_CNT];

    static const int s_MAX_KEYWORDS_CNT;
    static const std::unordered_map<std::string, E_DataType> s_keywordsDataTypeMap;
    static std::list<TokenBase*> s_generatedTmpTokenPool;

    static TokenBase* generateTmpExpression(E_DataType dt, const std::string& expression, TokenBase* begtoken, TokenBase* endtoken);
    static void clearTmpGenTokenPool();

protected:
    std::vector<TokenBase*> m_allTokenList;    // for memory pool manager
    std::vector<TokenBase*> m_validTokenList;  // a list for only 1 statement end up with ';' with all tokens that is neither comment nor blank ( arbitary <Space> or <Tab> )  ,  So the list can analyze the 2 adjacent statements' relationship.
    std::vector<TokenBase*> m_oneSentence;     // a list for only 1 statement with neither comment nor blank and whose delimiter is ';'

    // std::stack<TokenBase*>  m_opertorStack;
    std::list<TokenBase*>   m_opertorStack;
    std::list<TokenBase*>   m_suffixExpression; // Core : the main suffix expressions only contain a function object's name without  any function arugments , and it can only caotain the comma ',' which plays a role of comma expression seperator ( seperate two token for evaluattion   rather than function arguments seperator ):


    //////////////////////////////////////////////////////////////////////
    //
    // Function related data members
    //
    //////////////////////////////////////////////////////////////////////

    std::list<TokenBase*>                   m_callstackFuncList;
    /**************************************************

      e.g.   
          expr : sin(  1 + sin(30 + sin(45) ) )

          there is a layer conception inside  argument parsing

                 1.     1 +                 ==>   1 +  sin( 30 + sin(45) )
             (sin) 2.     30 +              ==>   30 + sin(45) ( pop | sin(30 + sin(45) |      to  layer-1 )
               (sin) 3.      45 <Done>      _________| ( pop | sin(45) |      to layer-2 )

    ***************************************************/
    std::list< std::list<TokenBase*>* >     m_oneArgument4ACertainFunc;
    std::list< std::pair<TokenBase*,int> >  m_openParenthesisList;

	// the following 2 list will not treat as one part of (m_opertorStack, m_suffixExpression) 
    std::list<TokenBase*>                   m_opertorStack4OneArgument;
	std::list<TokenBase*>                   m_suffixExpression4OneArgument;


    int m_execodeIdx;

};

#endif
