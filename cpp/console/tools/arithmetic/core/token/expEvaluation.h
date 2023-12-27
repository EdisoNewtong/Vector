#ifndef EXP_EVALUATION_H
#define EXP_EVALUATION_H

#include <vector>
#include <list>
#include "tokenBase.h"

class ExpEvaluation
{
public:
    static void init();
    static void release();


public:
    TokenBase* doNormalExpEvaluation(const std::vector<TokenBase*>& expList, int begIdx, bool checkVaibleIsValid);
    void doNewVaribleDefEvaluation(E_DataType definedDataType, const std::vector<TokenBase*>& expList, int begIdx);

    TokenBase* evaluateSuffixExpression(std::list<TokenBase*>& suffixExpression, bool needCheckVarible);

    static TokenBase* generateTmpExpression(E_DataType dt, const std::string& expression, TokenBase* begtoken, TokenBase* endtoken);
    static ExpEvaluation* createANewEvaluator();

protected:
    struct suffixExpEnv {
        suffixExpEnv() 
            : funcObj( nullptr )
            , operatorStack()
            , suffixExpression()
        {

        }

        virtual ~suffixExpEnv() {
            funcObj = nullptr;
            operatorStack.clear();
            suffixExpression.clear();
        }

        FunctionBase*                          funcObj;
        std::list< std::pair<TokenBase*,int> > operatorStack;
        std::list<TokenBase*>                  suffixExpression;
    };

    static void clearTmpTokenList();
    static void clearAllocedEvaluator();
    static void clearEnvList();

    static suffixExpEnv* allocEnvObject();

protected:
    ExpEvaluation();
    virtual ~ExpEvaluation();


    int preCheckOperator(TokenBase* previousToken, const std::pair<TokenBase*,int>& tokenPair,  ExpEvaluation::suffixExpEnv* pEnv);

    void buildSuffixExpression(const std::vector<TokenBase*>& expList, int begIdx, ExpEvaluation::suffixExpEnv* pEnv, bool checkVaibleIsValid);

    std::pair<TokenBase*,int>  findClosestOpenParenthesis(const std::list< std::pair<TokenBase*,int> >& operatorStack);
    void popUntilOpenParenthesis(ExpEvaluation::suffixExpEnv* pEnv);
    ExpEvaluation::suffixExpEnv* processOperatorStack(TokenBase* previousToken, const std::pair<TokenBase*,int>& tokenPair, ExpEvaluation::suffixExpEnv* currentEnv);



    void traceOperatorStack(TokenBase* pToken, bool push);//D
    void traceSuffixExpression(TokenBase* pToken, bool push);//D
    void tracePositiveNegativeFlag(TokenBase* pPreviousToken, E_OperatorType op);//D
    void traceSomeTokensFromOpMove2SuffixExpression(const std::list<TokenBase*>& lst, bool specialFlag);//D
    void traceOpMove2SuffixExpression(TokenBase* pToken);//D
    void traceUnInitializedVarWhenUsed(TokenBase* pToken);//D
    void traceTmpOpResult(const std::string& expr, DataValue& retValue);//D


    void traceNegativeOperation(TokenBase* right);//D
    void traceBitShiftWarning(bool isLeftBitShift, TokenBase* left,  TokenBase* right);
    void traceAssignOverFlow(TokenBase* leftVar, TokenBase* rightFixedInt);//D



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
    TokenBase* doCommaExpression(TokenBase* left, TokenBase* right);

    // a = b;
    TokenBase* doAssignment(TokenBase* left, TokenBase* right);
    // a ?= b;
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

    // for only one operand ,  dataType conversion ( such as : + , - , ~ )
    E_DataType operatorPrepairDataTypeConversion1(DataValue* pRightVal);
    // for two operands , dataType conversion
    E_DataType operatorPrepairDataTypeConversion2(DataValue* pLeftVal, DataValue* pRightVal);


protected:
    static std::list<TokenBase*>                      s_generatedTmpTokenPool;
    static std::list<ExpEvaluation*>                  s_allocedEvaluatorList;
    static std::list<ExpEvaluation::suffixExpEnv*>    s_suffixEnvList;

    std::list<ExpEvaluation::suffixExpEnv*>           m_callStack;
};


#endif
