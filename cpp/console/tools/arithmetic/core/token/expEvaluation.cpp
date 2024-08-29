#include "expEvaluation.h"
#include "tokenMgr.h"
#include "opUtil.h"
#include "variblePool.h"
#include "cmdOptions.h"
#include "enumUtil.h"
#include "charUtil.h"
#include "dataTypeUtil.h"
#include "myException.h"
#include "functionMgr.h"
#include "functionBase.h"

#include <iterator>
#include <iostream>
using namespace std;

//////////////////////////////////////////////////
// static Part
//////////////////////////////////////////////////
list<TokenBase*>                      ExpEvaluation::s_generatedTmpTokenPool;
list<ExpEvaluation*>                  ExpEvaluation::s_allocedEvaluatorList;
list<ExpEvaluation::suffixExpEnv*>    ExpEvaluation::s_suffixEnvList;

void ExpEvaluation::init() 
{ 
    s_generatedTmpTokenPool.clear();
    s_allocedEvaluatorList.clear();
    s_suffixEnvList.clear();
}

void ExpEvaluation::release() 
{
    clearTmpTokenList();
    clearAllocedEvaluator();
    clearEnvList();
}


// static 
ExpEvaluation* ExpEvaluation::createANewEvaluator()
{
    auto allocedEvaluator = new ExpEvaluation();
    s_allocedEvaluatorList.push_back( allocedEvaluator );
    return allocedEvaluator; 
}



void ExpEvaluation::clearTmpTokenList()
{
    for( auto it = s_generatedTmpTokenPool.begin(); it!=s_generatedTmpTokenPool.end(); ++it ) {
        INNER_SAFE_DELETE( *it );
    }
    s_generatedTmpTokenPool.clear();
}

// static 
void ExpEvaluation::clearAllocedEvaluator()
{
    for( auto it = s_allocedEvaluatorList.begin(); it!=s_allocedEvaluatorList.end(); ++it ) {
        INNER_SAFE_DELETE( *it );
    }
    s_allocedEvaluatorList.clear();
}

// static 
void ExpEvaluation::clearEnvList()
{
    for( auto it = s_suffixEnvList.begin(); it!=s_suffixEnvList.end(); ++it ) {
        INNER_SAFE_DELETE( *it );
    }
    s_suffixEnvList.clear();
}


// static 
ExpEvaluation::suffixExpEnv* ExpEvaluation::allocEnvObject()
{
    ExpEvaluation::suffixExpEnv* pEnv = new suffixExpEnv();
    s_suffixEnvList.push_back( pEnv );
    return pEnv;
}





ExpEvaluation::ExpEvaluation()
    : m_callStack()
{

}

// virtual 
ExpEvaluation::~ExpEvaluation()
{

}




    
TokenBase* ExpEvaluation::doNormalExpEvaluation(const vector<TokenBase*>& expList, int begIdx, bool checkVaibleIsValid)
{
    // ExpEvaluation::clearEnvList();
    auto veryBeginningEnv = ExpEvaluation::allocEnvObject();
    // reset call stack and push the 1st element
    // m_callStack.clear();
    m_callStack.push_back( veryBeginningEnv );
    auto currentEnv = veryBeginningEnv;

    buildSuffixExpression(expList, begIdx, currentEnv, checkVaibleIsValid);
    return evaluateSuffixExpression( currentEnv->suffixExpression, checkVaibleIsValid );
}



void ExpEvaluation::buildSuffixExpression(const vector<TokenBase*>& expList, int begIdx, ExpEvaluation::suffixExpEnv* currentEnv, bool checkVaibleIsValid)
{
    int sz = static_cast<int>( expList.size() );
    TokenBase* previousToken = nullptr;

    //
    // Step 1. Visit all the token , the preBuild the suffix expression
    //
    for ( auto idx = begIdx; idx < sz; ++idx  ) {
        auto token = expList.at(idx);
        if ( token == nullptr ) {
            previousToken = token;
            continue;
        }

        auto tokenType = token->getTokenType();
        if ( tokenType == E_TOKEN_EXPRESSION ) {
            // fixed literal / varible|funcName / KeyWord
            if ( token->isKeyword() ) {
                string strTypeKeyWords = token->getTokenContent();
                int keywordsCnt = 1; // token -> 1
                if (   previousToken != nullptr ) {
                    if ( previousToken->isKeyword() ) {
                        ++keywordsCnt; // previousToken -> ++
                        strTypeKeyWords += " ";
                        strTypeKeyWords += previousToken->getTokenContent();

                        // new feature for force type cast
                        // such as (unsigned         int)(3.14f * 5.0)
                        //              ^             ^
                        //          previousToken   token
                        TokenBase* previousMatchedBegin = nullptr;
                        // do previous search until the token is not keyword
                        for ( int j = idx-2; j >=0; --j ) {
                            auto curPreToken = expList.at(j);
                            if ( curPreToken != nullptr  ) {
                                if ( curPreToken->isKeyword() ) {
                                    strTypeKeyWords += " ";
                                    strTypeKeyWords += curPreToken->getTokenContent();
                                    ++keywordsCnt;
                                } else {
                                    previousMatchedBegin = curPreToken;
                                    break;
                                }
                            }
                        }

                        //                                     == 4
                        if ( keywordsCnt > TokenMgr::s_MAX_KEYWORDS_CNT ) {
                            MyException e(E_THROW_TOO_MANY_KEYWORDS, token->getBeginPos() );
                            e.setDetail( strTypeKeyWords );
                            throw e;
                        }

                        if (   previousMatchedBegin == nullptr 
                            || !(       previousMatchedBegin->getTokenType()    == E_TOKEN_OPERATOR
                                    &&  previousMatchedBegin->getOperatorType() == E_OPEN_PARENTHESIS
                                    &&  previousMatchedBegin->getContextRoleForOp() == E_OP_FLAG_OPEN_PARENTHESIS_FORCE_TYPE_CAST_START 
                                 ) 
                        ) 
                        {
                            MyException e(E_THROW_CANNOT_PUSH_TOKEN_KEYWORD, token->getBeginPos() );
                            throw e;
                        }
                    } else if (   previousToken->getTokenType()    == E_TOKEN_OPERATOR 
                               && previousToken->getOperatorType() == E_OPEN_PARENTHESIS ) {
                        previousToken->setContextRoleForOp( E_OP_FLAG_OPEN_PARENTHESIS_FORCE_TYPE_CAST_START );
                    } else {
                        MyException e(E_THROW_CANNOT_PUSH_TOKEN_KEYWORD, token->getBeginPos() );
                        throw e;
                    }
                } else {
                    MyException e(E_THROW_CANNOT_PUSH_TOKEN_KEYWORD, token->getBeginPos() );
                    throw e;
                }


            } else if ( token->isVarible() ) {

                if ( checkVaibleIsValid ) {
                    string varName = token->getTokenContent();

                    VaribleInfo* pVisitedVaribleInfo = VariblePool::getPool()->getVaribleByName( varName );
                    if ( pVisitedVaribleInfo == nullptr ) {
                        if ( !FunctionMgr::isInsideFunctionList(varName) ) {
                            MyException e(E_THROW_VARIBLE_NOT_DEFINED , token->getBeginPos());
                            e.setDetail( varName  );
                            throw e;
                        }
                        // else   
                        //      maybe it is a pending function-call
                    } else {
                        // it is a varible
                        token->setDataType( pVisitedVaribleInfo->dataVal.type );
                        token->setRealValue( pVisitedVaribleInfo->dataVal );
                    }
                }
            }

            if ( currentEnv!=nullptr ) {
                /*
                      (int)3.14f
                */
                // the token can be a keyword such as int/short/unsigned/long  etc.
                currentEnv->suffixExpression.push_back( token );
            }

            traceSuffixExpression(token, true);
        } else  {
            // tokenType == E_TOKEN_OPERATOR
            currentEnv = processOperatorStack(previousToken, make_pair(token, idx), currentEnv);
        }

        previousToken = token;
    }


    //
    // Step 2 : pop the rest  operator(s) inside the operatorStack one by one and push it into the suffixExpression
    //
    /*
        e.g.                                                           
               Expression  :    a + b * c

                                                                       |___|
              Suffix expression list  |     OperatorStack     |        | + |
                                      |                       |        |___|
                  |   |               |                       |        | * | 
                  |___|               |                       |        |___|
                  | c |               |                       |        | c |
                  |___|               |         |___|         |        |___|
                  | b |               |         | * |         |        | b |
                  |___|               |         |___|         |        |___|
                  | a |               |         | + |         |        | a |
                  |___|               |         |___|         |        |___|
                                      |                       |


    */
    for ( auto rit = currentEnv->operatorStack.rbegin(); rit != currentEnv->operatorStack.rend(); ++rit ) {
        auto token = rit->first;
        if (      token != nullptr 
              &&  token->getOperatorType() == E_OPEN_PARENTHESIS ) {
            MyException e(E_THROW_NO_MATCHED_CLOSE_PARENTHESIS, token->getBeginPos() );
            throw e;
        } else {
            currentEnv->suffixExpression.push_back( token );
        }
    }
    currentEnv->operatorStack.clear();

}



void ExpEvaluation::doNewVaribleDefEvaluation(E_DataType definedDataType, const vector<TokenBase*>& expList, int begIdx)
{
    vector<TokenBase*> prefixDefineTokeList;
    int sz = static_cast<int>( expList.size() );
    auto hasAssignment = false;
    for ( auto idx = begIdx; idx < sz; ++idx ) {
        auto token = expList.at(idx);
        if (   token != nullptr ) {
            if (    token->getTokenType() == E_TOKEN_OPERATOR
                 && token->getOperatorType() == E_ASSIGNMENT )
            {
                // break if meet the 1st   '='
                hasAssignment = true;
                break;
            } else {
                prefixDefineTokeList.push_back( token );
            }
        }
    }

    auto evaluator = ExpEvaluation::createANewEvaluator();
    TokenBase* varibleToken = nullptr;
    if ( evaluator != nullptr ) {
        //
        // Step 1 : Create a new varible
        //
        auto needCheckVaribleIsExisted = false;
        varibleToken = evaluator->doNormalExpEvaluation(prefixDefineTokeList, 0, needCheckVaribleIsExisted);
        if ( varibleToken!=nullptr  ) {
            if( !varibleToken->isVarible() ) {
                MyException e(E_THROW_SENTENCE_DEFINITION_PREFIX_IS_NOT_VARIBLE, varibleToken->getBeginPos() );
                throw e;
            } else {
                auto varname = varibleToken->getTokenContent();
                VaribleInfo* pVaribleInfo = VariblePool::getPool()->create_a_new_varible(definedDataType, varname , varibleToken->getBeginPos().line );
                if ( pVaribleInfo != nullptr  ) {
                    // Random a value and assign it to new alloced varible's
                    VariblePool::getPool()->randomVaribleValue( varname );

                    if ( varibleToken != nullptr ) { 
                        varibleToken->setDataType( definedDataType );
                        varibleToken->setRealValue( pVaribleInfo->dataVal );
                    } else {
                        MyException e(E_THROW_VARIBLE_IS_MISSING);
                        e.setDetail( varibleToken->getTokenContent() + string(" is not a varible") );
                        throw e;
                    }
                }
            }
        }

        //
        // Step 2 : do assignment if necessary
        //
        if ( hasAssignment ) {
            auto evaluatorAssignment = ExpEvaluation::createANewEvaluator();
            if ( evaluatorAssignment != nullptr ) {
                auto needCheckVaribleIsExisted2 = true;
                evaluatorAssignment->doNormalExpEvaluation(expList, begIdx, needCheckVaribleIsExisted2);
            }
        }
    }

}


/************************************************** 

   '(' : if function-call               ,  return 1
   ')' : if function-end                ,  return 2   

',' : if function-argument seperator ,  return 3

    otherwise return 0

***************************************************/
int ExpEvaluation::preCheckOperator(TokenBase* previousToken, const std::pair<TokenBase*,int>& tokenPair, ExpEvaluation::suffixExpEnv* pEnv)
{
    auto pToken = tokenPair.first;
    auto currentIdx = tokenPair.second;

    auto special_retFlag = 0;
    auto opType = pToken->getOperatorType();
    if ( opType == E_ADD  || opType == E_MINUS ) {
        E_OperatorType originalType = opType;
        auto bIsAdd = (originalType == E_ADD);
        auto needChangeType = false;
        if ( previousToken == nullptr ) {
            needChangeType = true;
        } else {
            auto previousTp = previousToken->getTokenType();
            if ( previousTp == E_TOKEN_OPERATOR ) {
                auto preOpType = previousToken->getOperatorType();
                if ( preOpType != E_CLOSE_PARENTHESIS ) {
                    needChangeType = true;
                } else {
                    // previousToken is ')'    force type cast  for new feature
                    /*
                           (int)-a             (int)+a
                           (int)-sin(3.14f)
                           (int)-3.14f
                    */
                    if ( previousToken->getContextRoleForOp() == E_OP_FLAG_OPEN_PARENTHESIS_FORCE_TYPE_CAST_END ) {
                        needChangeType = true;
                    }
                }
            }
        }

        if ( needChangeType ) {
            opType = bIsAdd ? E_POSITIVE : E_NEGATIVE;
            tracePositiveNegativeFlag(previousToken, opType);
            pToken->setOpType(opType);
        }
    }




    switch ( opType )
    {
    case E_OPEN_PARENTHESIS:
        {
            if ( previousToken == nullptr ) {
                pToken->setContextRoleForOp( E_OP_FLAG_OPEN_PARENTHESIS_PRIORITY_PREMOTE );
            } else {
                if ( previousToken->isVarible() ) {
                    // e.g.   pow( ...
                    string varName = previousToken->getTokenContent();
                    if ( !FunctionMgr::isInsideFunctionList(varName) ) {
                        MyException e(E_THROW_UNKNOWN_BUILT_IN_FUNCTION_NAME, previousToken->getBeginPos() );
                        e.setDetail( varName );
                        throw e;
                    }

                    pToken->setContextRoleForOp( E_OP_FLAG_OPEN_PARENTHESIS_FUNCTION_START );
                    special_retFlag = 1;
                } else {
                    pToken->setContextRoleForOp( E_OP_FLAG_OPEN_PARENTHESIS_PRIORITY_PREMOTE );
                }
            } 
        }
        break;
    case E_CLOSE_PARENTHESIS:
        {
            auto matchedPr = findClosestOpenParenthesis( pEnv->operatorStack );
            auto openToken = matchedPr.first;
            if ( openToken == nullptr ) {
                // the openParenthesis is not found
                MyException e(E_THROW_NO_MATCHED_OPEN_PARENTHESIS, pToken->getBeginPos() );
                throw e;
            }

            // found the matched '('
            auto roleFlag = openToken->getContextRoleForOp();
            if ( roleFlag == E_OP_FLAG_OPEN_PARENTHESIS_FUNCTION_START ) {
                pToken->setContextRoleForOp( E_OP_FLAG_CLOSE_PARENTHESIS_FUNCTION_END );
                special_retFlag = 2;
            } else if ( roleFlag == E_OP_FLAG_OPEN_PARENTHESIS_FORCE_TYPE_CAST_START ) { // previous '(' is   force  type cast  begin   for new feature  such as     (int)3.14f
                pToken->setContextRoleForOp( E_OP_FLAG_OPEN_PARENTHESIS_FORCE_TYPE_CAST_END );
                // special_retFlag = 4;
            } else {
                // E_OP_FLAG_OPEN_PARENTHESIS_PRIORITY_PREMOTE
                if ( (matchedPr.second + 1) == currentIdx ) {
                    //  none-function   closest   ()   is not allowed
                    MyException e(E_THROW_EMPTY_CONTENT_INSIDE_PARENTHESIS_PAIR_IS_NOT_ALLOWED, pToken->getBeginPos() );
                    throw e;
                }

                pToken->setContextRoleForOp( E_OP_FLAG_CLOSE_PARENTHESIS_PRIORITY_PREMOTE );
            }
        }
        break;
    case E_COMMA:
        {
            auto matchedPr = findClosestOpenParenthesis( pEnv->operatorStack );
            auto openToken = matchedPr.first;
            if ( openToken == nullptr ) {
                // such as   a = b,c,d;
                pToken->setContextRoleForOp( E_OP_COMMA_NORMAL_FOR_COMMA_EXPRESSION );
            } else {
                auto roleFlag = openToken->getContextRoleForOp();
                if ( roleFlag == E_OP_FLAG_OPEN_PARENTHESIS_FUNCTION_START ) {
                    pToken->setContextRoleForOp( E_OP_COMMA_FUNCTION_ARG_SEP );
                    special_retFlag = 3;
                } else {
                    // e.g.   a = (b,c,d);
                    pToken->setContextRoleForOp( E_OP_COMMA_NORMAL_FOR_COMMA_EXPRESSION );
                }
            }
        }
        break;
    default:
        break;
    }

    return special_retFlag;
}


void ExpEvaluation::traceSuffixExpression(TokenBase* pToken, bool push)
{
    using namespace charUtil;

    if ( CmdOptions::needTraceOperatorStackSuffixExpressionChange()  ) {
        if ( push ) {
            cerr << "SuffixExpr->Push " << SINGLE_QUOTO << pToken->getTokenContent() << SINGLE_QUOTO << endl;
        } else {
            cerr << "SuffixExpr->Pop "  << SINGLE_QUOTO << pToken->getTokenContent() << SINGLE_QUOTO << endl;
        }
    }
}

void ExpEvaluation::tracePositiveNegativeFlag(TokenBase* pPreviousToken, E_OperatorType op)
{
    if ( CmdOptions::needTracePositiveNegativePropertyChange() ) {
        auto isAdd = (op == E_ADD);
        if ( pPreviousToken == nullptr ) {
            cerr << "chOp : nullptr , so " << (isAdd ? " add -> positive" : " minus -> negative ") << endl;
        } else {
            auto content = pPreviousToken->getTokenContent();
            cerr << "ChOp :  \"" << content << "\" , so  " << (isAdd ? " add -> positive" : " minus -> negative ") << endl;
        }
    }
}


pair<TokenBase*,int>  ExpEvaluation::findClosestOpenParenthesis(const list< std::pair<TokenBase*,int> >& operatorStack)
{
    if ( operatorStack.empty() ) {
        return make_pair(nullptr, -1);
    }

    for( auto rit = operatorStack.rbegin(); rit != operatorStack.rend(); ++rit ) {
        if ( rit->first != nullptr &&  rit->first->getOperatorType() == E_OPEN_PARENTHESIS ) {
            return (*rit);
        }
    }
    return make_pair(nullptr, -1);
}


void ExpEvaluation::popUntilOpenParenthesis(ExpEvaluation::suffixExpEnv* pEnv)
{
    if ( pEnv == nullptr ) {
        return;
    }

    list<TokenBase*> movedTokenList;
    auto meetOpenParenthese = false;
    for( auto rit = pEnv->operatorStack.rbegin(); rit != pEnv->operatorStack.rend(); )
    {
        TokenBase* pElement = rit->first;
        auto isopenParenthesis = (pElement!=nullptr && pElement->getOperatorType() == E_OPEN_PARENTHESIS && pElement->getContextRoleForOp() == E_OP_FLAG_OPEN_PARENTHESIS_PRIORITY_PREMOTE  );

        movedTokenList.push_back( pElement );
        // traceOperatorStack( pElement, false );
        auto fit = pEnv->operatorStack.erase( (++rit).base() );
        rit = reverse_iterator< decltype(fit) >( fit );

        if ( isopenParenthesis ) {
            // meet the matched    '(' ,   erase (drop) it *** without *** push it into suffix expression
            meetOpenParenthese = true;
            break;
        } else {
            pEnv->suffixExpression.push_back( pElement );
            // traceSuffixExpression( pElement , true);
        }
    }

    traceSomeTokensFromOpMove2SuffixExpression( movedTokenList, meetOpenParenthese );
}

void ExpEvaluation::traceSomeTokensFromOpMove2SuffixExpression(const list<TokenBase*>& lst, bool specialFlag)
{
    if ( CmdOptions::needTraceOperatorStackSuffixExpressionChange()  ) {
        using namespace charUtil;
        if ( lst.empty() ) { return; }

        int cnt     = 0;
        string strOpList;
        string strSuffixList;
        auto it = lst.begin();

        int sz = static_cast<int>( lst.size() );
        while ( true )
        {
            auto tokenContent = (*it)->getTokenContent();
            string strElement = (SINGLE_QUOTO + tokenContent + SINGLE_QUOTO);
            strOpList += strElement;
            if ( specialFlag ) {
                if ( (*it)->getOperatorType() != E_OPEN_PARENTHESIS ) {
                    strSuffixList += strElement;
                }
            } else {
                strSuffixList += strElement;
            }
            ++it;
            ++cnt;

            if ( it == lst.end() ) {
                break;
            } else {
                // add op comma to opStr
                strOpList += ", ";
                // add comma to suffix list
                if ( specialFlag )  {
                    if ( cnt < (sz-1) ) {
                        strSuffixList += ", ";
                    }
                } else {
                    strSuffixList += ", ";
                }
            }
        }

        cerr << "OpStack->Pop  "    << cnt << (cnt == 1 ? " element : " : " elements : ") << strOpList << endl;
        if ( specialFlag ) { 
            --cnt; 
        }
        cerr << "SuffixExpr->Push " << cnt << (cnt == 1 ? " element : " : " elements : ") << strSuffixList << endl;
    }
}

void ExpEvaluation::traceOperatorStack(TokenBase* pToken, bool push)
{
    using namespace charUtil;

    if ( CmdOptions::needTraceOperatorStackSuffixExpressionChange()  ) {
        if ( push ) {
            cerr << "OpStack->Push " << SINGLE_QUOTO << pToken->getTokenContent() << SINGLE_QUOTO << endl;
        } else {
            cerr << "OpStack->Pop "  << SINGLE_QUOTO << pToken->getTokenContent() << SINGLE_QUOTO << endl;
        }

    }

}


ExpEvaluation::suffixExpEnv* ExpEvaluation::processOperatorStack(TokenBase* previousToken, const std::pair<TokenBase*,int>& tokenPair, ExpEvaluation::suffixExpEnv* currentEnv)
{
    auto retEnv = currentEnv;
    if ( currentEnv == nullptr ) {
        return currentEnv;
    }

    auto token = tokenPair.first;
    auto retFlag = preCheckOperator(previousToken, tokenPair, currentEnv);
    auto bNeedDoNormalProcess = false;
    switch( retFlag )
    {
    case 1: // '(' for function begin
        {
            auto genFuncObject = FunctionMgr::generateFunctionObjectByName( previousToken->getTokenContent() );
            genFuncObject->setBeginParenthesis( previousToken , tokenPair.first ); 
            previousToken->setAsFunction( genFuncObject );

            auto newAllocedEnv = allocEnvObject();
            m_callStack.push_back( newAllocedEnv );
            currentEnv = m_callStack.back();
            currentEnv->funcObj = genFuncObject;
            // push the very beginning '(' for function-begin
            currentEnv->operatorStack.push_back( tokenPair );
        }
        break;
    case 2: // ')' for function end
        {
            // build suffix exp  push last arugment part
            if ( !currentEnv->suffixExpression.empty() ) {
                for ( auto rit = currentEnv->operatorStack.rbegin(); rit != currentEnv->operatorStack.rend(); ++rit ) {
                    if (       rit->first != nullptr 
                          &&   rit->first->getOperatorType()     == E_OPEN_PARENTHESIS 
                          &&   rit->first->getContextRoleForOp() == E_OP_FLAG_OPEN_PARENTHESIS_FUNCTION_START ) 
                    {
                        break;
                    } else {
                        currentEnv->suffixExpression.push_back( rit->first );
                    }
                }
                // clear all list with the very beginning '(' of function-begin
                currentEnv->operatorStack.clear();

                if (  currentEnv->funcObj != nullptr ) {
                    currentEnv->funcObj->pushOneArgumentExpr( currentEnv->suffixExpression );
                }
            }

            if ( currentEnv->funcObj != nullptr ) {
                currentEnv->funcObj->setCloseParenthesisValidFlag( tokenPair.first );
            }
            
            if ( !m_callStack.empty() ) {
                m_callStack.pop_back();
            }

            if ( m_callStack.empty() ) {
                MyException e( E_THROW_CALL_STACK_SHOULD_NEVER_BE_EMPTY, token->getBeginPos() );
                throw e;
            }

            currentEnv = m_callStack.back();
        }
        break;
    case 3: // ',' for one argument sequence for a certain function 
        {
            if ( currentEnv->suffixExpression.empty() ) {
                MyException e(E_THROW_FUNCTION_ARGUMENT_CANNOT_BE_EMPTY_WHEN_MEET_COMMA , token->getBeginPos() );
                throw e;
            }

            for ( auto rit = currentEnv->operatorStack.rbegin(); rit != currentEnv->operatorStack.rend(); ++rit ) {
                if (       rit->first != nullptr 
                      &&   rit->first->getOperatorType()     == E_OPEN_PARENTHESIS 
                      &&   rit->first->getContextRoleForOp() == E_OP_FLAG_OPEN_PARENTHESIS_FUNCTION_START ) 
                {
                    break;
                } else {
                    currentEnv->suffixExpression.push_back( rit->first );
                }
            }
            
            // leave the very beginning '('
            auto veryBeginOpenTokenPr = currentEnv->operatorStack.front();
            currentEnv->operatorStack.clear();
            currentEnv->operatorStack.push_back( veryBeginOpenTokenPr );

            if ( currentEnv->funcObj != nullptr ) {
                currentEnv->funcObj->pushOneArgumentExpr( currentEnv->suffixExpression );
                // clear argument list which is pushed just now  and waiting for the upcoming new function argument
                currentEnv->suffixExpression.clear();
            }
        }
        break;
    default:
        bNeedDoNormalProcess = true;
        break;
    }

    if ( bNeedDoNormalProcess  &&  currentEnv!=nullptr  ) {
        auto opType = token->getOperatorType();

        auto bOpNeedProcess = false;
        int matchedForceTypeCastOpenParenthesisIdx = -1;
        TokenBase* pTokenForceTypeCast = nullptr;

        if ( opType == E_CLOSE_PARENTHESIS ) {
            // '('
            auto pr = findClosestOpenParenthesis( currentEnv->operatorStack );
            if ( pr.first == nullptr ) {
                MyException e(E_THROW_NO_MATCHED_OPEN_PARENTHESIS, token->getBeginPos() );
                throw e;
            } else {
                if ( token->getContextRoleForOp() == E_OP_FLAG_OPEN_PARENTHESIS_FORCE_TYPE_CAST_END ) {
                    if ( pr.first->getContextRoleForOp() != E_OP_FLAG_OPEN_PARENTHESIS_FORCE_TYPE_CAST_START ) {
                        MyException e(E_THROW_NO_MATCHED_OPEN_PARENTHESIS, token->getBeginPos() );
                        throw e;
                    }

                    //
                    // prepare keyword list
                    //
                    int keywordsCnt = 0;
                    std::list<string> tmpKeywordsList;
                    string placehold_space(" ");
                    while( !currentEnv->suffixExpression.empty() ) 
                    {
                        auto pKeyword_token = currentEnv->suffixExpression.back();
                        if ( pKeyword_token!=nullptr && pKeyword_token->isKeyword() ) {
                            tmpKeywordsList.push_front( pKeyword_token->getTokenContent() );
                            tmpKeywordsList.push_front( placehold_space );
                            ++keywordsCnt;

                            //
                            // erase all the keywords at tail until meet an unmatched one
                            //
                            currentEnv->suffixExpression.pop_back();
                        } else {
                            break;
                        }
                    }

                    if ( keywordsCnt == 0 ) {
                        MyException e(E_THROW_FORCE_TYPE_CAST_LIST_IS_EMPTY, token->getBeginPos() );
                        throw e;
                    }

                    // keywordsCnt > 0
                    // delete the front " ";
                    tmpKeywordsList.pop_front();
                    
                    string str_force_type_cast_name;
                    for( auto s_key : tmpKeywordsList ) {
                        str_force_type_cast_name += s_key;
                    }

                    if ( keywordsCnt > TokenMgr::s_MAX_KEYWORDS_CNT ) {
                        MyException e(E_THROW_TOO_MANY_KEYWORDS, token->getBeginPos() );
                        e.setDetail( str_force_type_cast_name );
                        throw e;
                    }


                    // cout << "str_force_type_cast_name = \"" << str_force_type_cast_name << "\" ." << endl;
                    auto forceTypeCast4_dt = TokenMgr::getDataTypeByString( str_force_type_cast_name );
                    if ( forceTypeCast4_dt == E_TP_UNKNOWN ) {
                        MyException e(E_THROW_SENTENCE_UNKNOWN_DATA_TYPE, token->getBeginPos() );
                        e.setDetail( str_force_type_cast_name );
                        throw e;
                    }


                    // Core Core Core
                    // generate a new token for   such as  :   (unsigned long long long)
                    pTokenForceTypeCast = generateTmpExpression_4ForceTypeCast(forceTypeCast4_dt, (charUtil::STR_OPEN_PARENTHESIS + str_force_type_cast_name + charUtil::STR_CLOSE_PARENTHESIS), pr.first, token );

                    // pop the previous (    of    (int)
                    //                  ^
                    currentEnv->operatorStack.pop_back();

                    if ( pTokenForceTypeCast == nullptr ) {
                        MyException e(E_THROW_SENTENCE_UNKNOWN_DATA_TYPE , token->getBeginPos() );
                        e.setDetail( string("\"") +  str_force_type_cast_name + string("\"") );
                        throw e;
                    } 

                    // Core Core Core
                    // (int)  -->  change the sequence into a force type cast operator type     for new feature
                    opType = E_FORCE_TYPE_CAST;
                    bOpNeedProcess = true;
                    matchedForceTypeCastOpenParenthesisIdx = pr.second;
                } else {
                    popUntilOpenParenthesis( currentEnv );
                }
            }
        } else {
            bOpNeedProcess = true;
        }

        if ( bOpNeedProcess ) {
            pair<TokenBase*,int> toProcessTokenPair = (matchedForceTypeCastOpenParenthesisIdx==-1) ? tokenPair : make_pair(pTokenForceTypeCast, matchedForceTypeCastOpenParenthesisIdx);

            if ( currentEnv->operatorStack.empty() ) {
                currentEnv->operatorStack.push_back( toProcessTokenPair );
                traceOperatorStack( token, true);
                retEnv = currentEnv;
                return retEnv;
            }

            auto toPushedOpMeta    = OpUtil::getOpInfo(opType);
            auto toPushed_p = toPushedOpMeta.getPriority();
            // auto current_rightAss = toPushedOpMeta.isRight2Left();
            auto forwardIt = currentEnv->operatorStack.begin();

            auto pushedFlag = false;
            for( auto rit = currentEnv->operatorStack.rbegin(); rit != currentEnv->operatorStack.rend(); )
            {
                auto pVisitToken = rit->first;
                auto visitOpType = pVisitToken->getOperatorType();
                auto cmpOpMeta = OpUtil::getOpInfo( visitOpType );
                auto cmp_p = cmpOpMeta.getPriority();
                auto cmp_rightAss = toPushedOpMeta.isRight2Left();

                if ( toPushed_p < cmp_p ) {
                    // toPush.priority is higher than the priority of top operator
                    currentEnv->operatorStack.insert( rit.base(), toProcessTokenPair );
                    traceOperatorStack(token, true );

                    pushedFlag = true;
                } else if ( toPushed_p == cmp_p ) {
                    if ( opType == E_OPEN_PARENTHESIS ) { // '(' is right-to-Left Associativity
                        currentEnv->operatorStack.insert( rit.base(), toProcessTokenPair );
                        traceOperatorStack( token, true );
                    } else {
                        if ( cmp_rightAss ) {
                            // Associativity is   right to left
                            forwardIt = currentEnv->operatorStack.insert( rit.base(), toProcessTokenPair );
                            traceOperatorStack( token, true );
                            // rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                        } else {
                            // Associativity is   left to right , 
                            //   1. pop only one : the current
                            //   2. copy current op into  m_suffixExpression  ( move the token from opStack --> suffixExpression  which is metioned @ step1 )
                            //   3. insert the same priority operator

                            forwardIt = currentEnv->operatorStack.erase( (++rit).base() ); // 1
                            // rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                            // traceOperatorStack( pVisitToken, false );

                            currentEnv->suffixExpression.push_back( pVisitToken ); // 1
                            // traceSuffixExpression( pVisitToken, true );
                            traceOpMove2SuffixExpression( pVisitToken );


                            currentEnv->operatorStack.insert( forwardIt, toProcessTokenPair ); // 3
                            // rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                            traceOperatorStack( token, true );
                        }
                    }

                    pushedFlag = true;
                } else {
                    // toPushed_p > cmp_p
                    // toPush.priority is lower than the priority of top operator
                    if ( visitOpType == E_OPEN_PARENTHESIS ) {
                        forwardIt = currentEnv->operatorStack.insert( rit.base(), toProcessTokenPair );
                        // rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                        traceOperatorStack( token, true );

                        pushedFlag = true;
                    } else {
                        // traceOperatorStack( pVisitToken, false );
                        forwardIt = currentEnv->operatorStack.erase( (++rit).base() );
                        rit = reverse_iterator< decltype(forwardIt) >( forwardIt );
                        currentEnv->suffixExpression.push_back( pVisitToken );
                        // traceSuffixExpression( pVisitToken, true );

                        traceOpMove2SuffixExpression(pVisitToken);
                    }
                }

                if ( pushedFlag ) {
                    break;
                }
            }


            if ( !pushedFlag ) {
                // if the operatorStack become empty until all the operator has been poped up
                //    you should never forget push the target operator at last
                //
                //          operator ','  to be pushed   |  After pop all operator whose priority is higher than ','
                //                                       |  than push the target   ','
                //  |   |                                |    
                //  |___|                                |    
                //  | * |                                |    
                //  |___|                                |    
                //  | + |                                |    
                //  |___|                                |                                
                //  | = |                                |                                | , |
                //  |___|                                |                                |___|
                //
                //     e.g.         a =    3 +   4 * 5       , 6
                //          =>    ( a = (  3 + ( 4 * 5 ) ) ) , 6
                //
                //############################################################
                //
                // the Operator order :
                // 1.    4 * 5   = 20
                // 2.    3 + 20  = 23
                // 3.    a = 23  =>   varible 'a' stored a value of 23
                // 4.    a, 6    =>   yield the final result as 6 :  23,6    => 6
                //
                //############################################################
                //
                //----------------------------------------------------------------------------------------------------
                //     the final suffixExpr :          | a | 3 | 4 | 5 | * | + | = | 6 | , |
                //----------------------------------------------------------------------------------------------------
                currentEnv->operatorStack.push_back( toProcessTokenPair );
                traceOperatorStack( token, true );
            }
        }
    }

    retEnv = currentEnv;
    return retEnv;
}

// move from OperatorStack to  -->   SuffixExpression
// pop from OpStack , push it into SuffixExpression list
void ExpEvaluation::traceOpMove2SuffixExpression(TokenBase* pToken)
{
    using namespace charUtil;

    if ( CmdOptions::needTraceOperatorStackSuffixExpressionChange()  ) {
        auto tokenContent = pToken->getTokenContent();
        cerr << "OpStack->Pop "        <<  SINGLE_QUOTO << tokenContent << SINGLE_QUOTO << " , "
             << "SuffixExpr->Push "    <<  SINGLE_QUOTO << tokenContent << SINGLE_QUOTO 
             << endl;
    }
}


TokenBase* ExpEvaluation::evaluateSuffixExpression(list<TokenBase*>& suffixExpression, bool needCheckVarible)
{
    list<TokenBase*> backupSuffixExpression(suffixExpression);

    int operatorProcessCnt = 0; // count for do binaryOp/UnaryOp Cnt
    int previousExpCnt = 0;
    for( auto it = suffixExpression.begin(); it != suffixExpression.end(); ) 
    {
        auto currentElement = *it;
        auto currentTokenType = currentElement->getTokenType();
        if ( currentTokenType != E_TOKEN_OPERATOR ) {
            if ( currentElement->isFunction() ) {
                auto funcObj = currentElement->getFuncObject();
                if ( funcObj == nullptr ) {
                    MyException e(E_THROW_FUNCTION_OBJECT_IS_NULLPTR , currentElement->getBeginPos() );
                    throw e;
                }
                
                auto retToken = funcObj->executeFunction();
                it = suffixExpression.erase( it );
                it = suffixExpression.insert(it, retToken );
                ++previousExpCnt;
            } else {
                // varible or fixed literal number 
                if ( needCheckVarible && currentElement->isVarible() ) {
                    string pVisitVarName = currentElement->getTokenContent();

                    VaribleInfo* pVisitedVaribleInfo = VariblePool::getPool()->getVaribleByName( pVisitVarName );
                    if ( pVisitedVaribleInfo == nullptr ) {
                        MyException e(E_THROW_VARIBLE_NOT_DEFINED , currentElement->getBeginPos());
                        e.setDetail( pVisitVarName );
                        throw e;
                    }

                    currentElement->setDataType( pVisitedVaribleInfo->dataVal.type );
                    currentElement->setRealValue( pVisitedVaribleInfo->dataVal );
                }

                ++it;
                ++previousExpCnt;
            }
        } else {
            // is an operator
            auto opType = currentElement->getOperatorType();
            if ( opType == E_OPERATOR_UNKNOWN || opType == E_OPEN_PARENTHESIS || opType == E_CLOSE_PARENTHESIS ) {
                MyException e(E_THROW_INVALID_OPERATOR , currentElement->getBeginPos() );
                string detailstr = "inside suffix expression : ";
                detailstr += EnumUtil::enumName( opType );
                detailstr += (string(", ") + currentElement->getBeginPos().getPos() );
                e.setDetail( detailstr );
                throw e;
            }

            auto opMeta = OpUtil::getOpInfo( opType );
            auto isbinaryOp = !opMeta.isUnaryOp();
            if ( isbinaryOp ) {
                if ( previousExpCnt < 2 ) {
                    MyException e(E_THROW_SUFFIXEXPR_BINARY_OP_MISS_TWO_OPERANDS, currentElement->getBeginPos() );
                    // e.setDetail( currentElement->getBeginPos().getPos() );
                    throw e;
                }

                auto nextIt = next(it);

                TokenBase* leftOperand  = nullptr;
                TokenBase* rightOperand = nullptr;

                it = prev(it,1);
                rightOperand = *it;

                it = prev(it,1);
                leftOperand = *it;

                if (   CmdOptions::needTraceUninitializedVaribleWhenEvaluatingExpression() ) {
                    // opType != E_ASSIGNMENT
                    // check Left Operand
                    auto needCheckLeft = (opType != E_ASSIGNMENT);
                    if ( needCheckLeft ) {
                        if ( needCheckVarible  &&   leftOperand!=nullptr   &&   leftOperand->isVarible() ) {
                            string varName = leftOperand->getTokenContent();
                            VaribleInfo* pVisitedVaribleInfo = VariblePool::getPool()->getVaribleByName( varName );
                            if ( pVisitedVaribleInfo != nullptr  &&   !pVisitedVaribleInfo->isInitialed ) {
                                if ( CmdOptions::needTreatUninitializedVaribleAsError()  ) {
                                    MyException e(E_THROW_VARIBLE_NOT_INITIALIZED_BEFORE_USED, leftOperand->getBeginPos() );
                                    throw e;
                                } else {
                                    traceUnInitializedVarWhenUsed(leftOperand);
                                }
                            }
                        }
                    }

                    //
                    // Not matter whether the operator is '=' or not , always check Right operand 
                    // 
                    if ( needCheckVarible &&   rightOperand!=nullptr    &&   rightOperand->isVarible() ) {
                        string varName = rightOperand->getTokenContent();
                        VaribleInfo* pVisitedVaribleInfo = VariblePool::getPool()->getVaribleByName( varName );
                        if ( pVisitedVaribleInfo != nullptr  &&   !pVisitedVaribleInfo->isInitialed ) {
                            if ( CmdOptions::needTreatUninitializedVaribleAsError()  ) {
                                MyException e(E_THROW_VARIBLE_NOT_INITIALIZED_BEFORE_USED, rightOperand->getBeginPos() );
                                throw e;
                            } else {
                                traceUnInitializedVarWhenUsed(rightOperand);
                            }
                        }
                    }
                }

                auto genTmpExp = doBinaryOp(leftOperand, currentElement, rightOperand);
                ++operatorProcessCnt;

                //      erase         range :       [ it,  nextIt )            include it , exclude nextIt
                it = suffixExpression.erase(it, nextIt);
                // insert a new tmp 
                it = suffixExpression.insert(it, genTmpExp);


                /*
                // previousExpCnt -= 3;
                // previousExpCnt += 1;
                previousExpCnt -= 2;     
                */


                // previousExpCnt -= 2;
                // previousExpCnt += 1;
                previousExpCnt -= 1;     
            } else {
                if ( previousExpCnt < 1 ) {
                    MyException e(E_THROW_SUFFIXEXPR_UNARY_OP_MISS_ONE_OPERAND, currentElement->getBeginPos());
                    // e.setDetail( currentElement->getBeginPos().getPos() );
                    throw e;
                }

                auto nextIt = next(it);
                TokenBase* rightOperand = nullptr;

                it = prev(it,1);
                rightOperand = *it;

                if ( CmdOptions::needTraceUninitializedVaribleWhenEvaluatingExpression() ) {
                    // check the only right Operand for Unary operator expression
                    if ( needCheckVarible && rightOperand!=nullptr &&   rightOperand->isVarible() ) {
                        string varName = rightOperand->getTokenContent();
                        VaribleInfo* pVisitedVaribleInfo = VariblePool::getPool()->getVaribleByName( varName );
                        if ( pVisitedVaribleInfo != nullptr  &&   !pVisitedVaribleInfo->isInitialed ) {
                            if ( CmdOptions::needTreatUninitializedVaribleAsError()  ) {
                                MyException e(E_THROW_VARIBLE_NOT_INITIALIZED_BEFORE_USED, rightOperand->getBeginPos() );
                                throw e;
                            } else {
                                traceUnInitializedVarWhenUsed(rightOperand);
                            }
                        }
                    }
                }

                auto genTmpExp = doUnaryOp(currentElement, rightOperand);
                ++operatorProcessCnt;

                it = suffixExpression.erase(it, nextIt);
                it = suffixExpression.insert(it, genTmpExp);

                /*
                // previousExpCnt -= 2;
                // previousExpCnt += 1;
                --previousExpCnt;
                */

                // previousExpCnt -= 1;
                // previousExpCnt += 1;
                previousExpCnt += 0;   // keep original count

            }

        }
    }

    auto afterEvaluateSz = static_cast<int>( suffixExpression.size() );
    if ( afterEvaluateSz != 1 ) {
        (void)backupSuffixExpression;

        MyException e(E_THROW_SUFFIXEXPR_FINAL_EVALUATE_MUST_LEAVE_ONLY_ONE_ELEMENT);
        throw e;
    }


    // special condition if tmp expression has no operator
    if (  operatorProcessCnt == 0 ) {
        for( auto it = suffixExpression.begin(); it != suffixExpression.end(); ++it ) 
        {
            auto currentElement = *it;
            auto currentTokenType = currentElement->getTokenType();

            if ( CmdOptions::needTraceUninitializedVaribleWhenEvaluatingExpression() ) {
                if ( needCheckVarible && currentElement!=nullptr   &&   currentElement->isVarible() ) {
                    string varName = currentElement->getTokenContent();
                    VaribleInfo* pVisitedVaribleInfo = VariblePool::getPool()->getVaribleByName( varName );
                    if ( pVisitedVaribleInfo != nullptr  &&   !pVisitedVaribleInfo->isInitialed ) {
                        if ( CmdOptions::needTreatUninitializedVaribleAsError()  ) {
                            MyException e(E_THROW_VARIBLE_NOT_INITIALIZED_BEFORE_USED, currentElement->getBeginPos() );
                            throw e;
                        } else {
                            traceUnInitializedVarWhenUsed(currentElement);
                        }
                    }
                }
            }


            if ( currentTokenType == E_TOKEN_EXPRESSION  ) {
                auto content = currentElement->getTokenContent();
                auto dataVal = currentElement->getRealValue();
                traceTmpOpResult(content , dataVal);
            } else {
                MyException e(E_THROW_CODE_CANNOT_REACH_HERE, currentElement->getBeginPos()  );
                e.setDetail(" When operatorStack is empty or binary/unary operator's count == 0 ");
                throw e;
            }
        }
    }

    return suffixExpression.front();
}

TokenBase* ExpEvaluation::doBinaryOp(TokenBase* left, TokenBase* op, TokenBase* right)
{
    TokenBase* genTmpExp = nullptr;

    switch( op->getOperatorType() )
    {
    case E_ADD:
        genTmpExp = doAdd(left,right);
        break;
    case E_MINUS:
        genTmpExp = doMinus(left,right);
        break;
    case E_MULTIPLY:
        genTmpExp = doMultiply(left,right);
        break;
    case E_DIVIDE:
        genTmpExp = doDivide(left,right);
        break;
    case E_MOD:
        genTmpExp = doMod(left,right);
        break;
    case E_BIT_AND:
        genTmpExp = doBitAnd(left,right);
        break;
    case E_BIT_OR:
        genTmpExp = doBitOr(left,right);
        break;
    case E_BIT_XOR:
        genTmpExp = doBitXor(left,right);
        break;
    case E_BIT_LEFT_SHIFT:
        genTmpExp = doBitLeftShift(left,right);
        break;
    case E_BIT_RIGHT_SHIFT:
        genTmpExp = doBitRightShift(left,right);
        break;
    case E_ASSIGNMENT:
        genTmpExp = doAssignment(left,right);
        break;
    case E_ADD_ASSIGNMENT:                 // +=
        genTmpExp = do_Add_Assignment(left, right);
        break;
    case E_MINUS_ASSIGNMENT:               // -=
        genTmpExp = do_Minus_Assignment(left, right);
        break;
    case E_MULTIPLY_ASSIGNMENT:            // *=
        genTmpExp = do_Multiply_Assignment(left, right);
        break;
    case E_DIVIDE_ASSIGNMENT:              // /=
        genTmpExp = do_Divide_Assignment(left, right);
        break;
    case E_MOD_ASSIGNMENT:                 // %=
        genTmpExp = do_Mod_Assignment(left, right);
        break;
    case E_BIT_AND_ASSIGNMENT:             // &=
        genTmpExp = do_Bit_And_Assignment(left, right);
        break;
    case E_BIT_OR_ASSIGNMENT:              // |=
        genTmpExp = do_Bit_Or_Assignment(left, right);
        break;
    case E_BIT_XOR_ASSIGNMENT:             // ^=
        genTmpExp = do_Bit_Xor_Assignment(left, right);
        break;
    case E_BIT_LEFT_SHIFT_ASSIGNMENT:      // <<=
        genTmpExp = do_Bit_LeftShift_Assignment(left, right);
        break;
    case E_BIT_RIGHT_SHIFT_ASSIGNMENT:     // >>=
        genTmpExp = do_Bit_RightShift_Assignment(left, right);
        break;
    case E_COMMA:     // , 
        genTmpExp = doCommaExpression(left, right);
        break;
    default:
        break;
    }

    return genTmpExp;
}

TokenBase* ExpEvaluation::doUnaryOp(TokenBase* op, TokenBase* right)
{
    TokenBase* genTmpExp = nullptr;
    switch( op->getOperatorType() )
    {
    case E_POSITIVE:
        genTmpExp = doPositive(op,right);
        break;
    case E_NEGATIVE:
        genTmpExp = doNegative(op,right);
        break;
    case E_BIT_NOT:
        genTmpExp = doBitNot(op,right);
        break;
    case E_FORCE_TYPE_CAST: // for   force type cast    for new feature
        genTmpExp = doForceTypeCastConversion(op, right);
        break;
    default:
        break;
    }

    return genTmpExp;
}



//####################################################################################################
//
//      Binary Op
TokenBase* ExpEvaluation::doAdd(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_ADD + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    E_DataType retDt = leftVal.type;
    DataValue sumRet = leftVal + rightVal;
    traceTmpOpResult(finalExpr, sumRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( sumRet );

    return ret;
}

TokenBase* ExpEvaluation::doMinus(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_MINUS + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    E_DataType retDt = leftVal.type;
    DataValue substractRet = leftVal - rightVal;
    traceTmpOpResult(finalExpr, substractRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setBeginPos( left->getBeginPos() );
    ret->setEndPos(   right->getBeginPos() );
    ret->setRealValue( substractRet );

    return ret;
}

TokenBase* ExpEvaluation::doMultiply(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_MULTIPLY  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    E_DataType retDt = leftVal.type;
    DataValue multiRet = leftVal * rightVal;
    traceTmpOpResult(finalExpr, multiRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( multiRet  );

    return ret;
}

TokenBase* ExpEvaluation::doDivide(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_DIVIDE  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);


    E_DataType retDt = leftVal.type;

    // calc 
    DataValue divideRet;
    try {
        divideRet  = (leftVal / rightVal);
    } catch ( MyException& e ) {
        // auto detailstr = e.getDetail();
        // if ( detailstr.empty() ) {
        //     e.setDetail( rightContent + " is zero Value ( in Divide ) @"  +  right->getBeginPos().getPos() );
        // } 
        e.setDetail( rightContent + " is zero Value ( in Divide ) @"  +  right->getBeginPos().getPos(0) );
        throw;
    }
    traceTmpOpResult(finalExpr, divideRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( divideRet );

    return ret;
}

TokenBase* ExpEvaluation::doMod(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_MOD  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_MODULO_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_MODULO_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }


    E_DataType retDt = leftVal.type;

    // calc 
    DataValue modRet;
    try {
        modRet  = (leftVal % rightVal);
    } catch ( MyException& e ) {
        if ( e.getDetail().empty() ) {
            e.setDetail( rightContent + " is zero Value ( in Modulo ) @" + right->getBeginPos().getPos(0) );
        }
        throw;
    }

    traceTmpOpResult(finalExpr, modRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( modRet );

    return ret;
}

TokenBase* ExpEvaluation::doBitAnd(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_AND  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITAND_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITAND_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }

    E_DataType retDt = leftVal.type;

    // calc 
    DataValue bitAndRet = (leftVal & rightVal);
    traceTmpOpResult(finalExpr, bitAndRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( bitAndRet );

    return ret;

}

TokenBase* ExpEvaluation::doBitOr(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_OR  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITOR_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITOR_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }


    E_DataType retDt = leftVal.type;

    // calc 
    DataValue bitOrRet = (leftVal | rightVal);
    traceTmpOpResult(finalExpr, bitOrRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( bitOrRet );

    return ret;
}

TokenBase* ExpEvaluation::doBitXor(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_XOR  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();
    operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITXOR_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITXOR_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }

    

    E_DataType retDt = leftVal.type;

    // calc 
    DataValue bitXOrRet = (leftVal ^ rightVal);
    traceTmpOpResult(finalExpr, bitXOrRet);

    TokenBase* ret = generateTmpExpression(retDt, finalExpr, left, right);
    ret->setRealValue( bitXOrRet );

    return ret;
}

TokenBase* ExpEvaluation::doBitLeftShift(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_LEFT_SHIFT  + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();

    E_DataType leftPromotionDt = operatorPrepairDataTypeConversion2(&leftVal, &rightVal);


    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BIT_LEFTSHIFT_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BIT_LEFTSHIFT_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }

    traceBitShiftWarning(true, left, right);


    // E_DataType retDt = leftVal->type;

    // calc 
    DataValue bitLeftShiftRet = (leftVal << rightVal);
    bitLeftShiftRet.downerCast( leftPromotionDt );

    traceTmpOpResult(finalExpr, bitLeftShiftRet);

    TokenBase* ret = generateTmpExpression(leftPromotionDt, finalExpr, left, right);
    ret->setRealValue( bitLeftShiftRet );

    return ret;
}

TokenBase* ExpEvaluation::doBitRightShift(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_RIGHT_SHIFT   + rightContent;

    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();

    E_DataType leftPromotionDt = operatorPrepairDataTypeConversion2(&leftVal, &rightVal);

    if ( leftVal.type == E_TP_FLOAT  ||  leftVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BIT_RIGHTSHIFT_CANNOT_APPLY_ON_FLOAT, left->getBeginPos() );
        e.setDetail( leftContent );
        throw e;
    } else if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BIT_RIGHTSHIFT_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    }

    traceBitShiftWarning(false, left, right);
    

    // calc 
    DataValue bitRightShiftRet = (leftVal >> rightVal);
    bitRightShiftRet.downerCast( leftPromotionDt );
    traceTmpOpResult(finalExpr, bitRightShiftRet);

    TokenBase* ret = generateTmpExpression(leftPromotionDt, finalExpr, left, right);
    ret->setRealValue( bitRightShiftRet );

    return ret;
}



TokenBase* ExpEvaluation::doCommaExpression(TokenBase* left, TokenBase* right)
{
    // using namespace charUtil;
    // string leftContent  = left->getExpressionContent();
    // string rightContent = right->getExpressionContent();
    // string finalExpr = leftContent + SC_OP_COMMA  + rightContent;

    (void)left;
    return right;
}


TokenBase* ExpEvaluation::doAssignment(TokenBase* left, TokenBase* right)
{
    using namespace charUtil;

    VaribleInfo* toBeAssignmentVar = nullptr;
    auto content = left->getTokenContent();
    if (    !( left->isVarible() ) 
        ||  ( ( toBeAssignmentVar = VariblePool::getPool()->getVaribleByName(content)) == nullptr )  ) {
        MyException e(E_THROW_VARIBLE_NOT_DEFINED, left->getBeginPos() );
        e.setDetail( content );
        throw e;
    }

    string leftContent  = left->getExpressionContent();
    string rightContent = right->getExpressionContent();
    string finalExpr = leftContent + SC_OP_BIT_ASSIGNMENT + rightContent;


    DataValue leftVal  = left->getRealValue();
    DataValue rightVal = right->getRealValue();

    traceAssignOverFlow(left, right);

    //
    // Core Core Core :
    // Can't use  assignment statement like the following :
    //       Because the left operand and the right operand has different data type
    //
    //    leftVal = rightVal;
    //
    leftVal.doAssignment(rightVal);
    left->setRealValue( leftVal );

    // update varible' value
    toBeAssignmentVar->dataVal.doAssignment( leftVal );
    toBeAssignmentVar->isInitialed = true; // set as initialed

    traceTmpOpResult(finalExpr, leftVal);
    left->setGeneratedExp( finalExpr );
    return left;

}


/***************************************************
 *    ?=
****************************************************/
TokenBase*      ExpEvaluation::do_Add_Assignment(TokenBase* left, TokenBase* right)
{  return doAssignment( left,  doAdd(left, right) ); }

TokenBase*     ExpEvaluation::do_Minus_Assignment(TokenBase* left, TokenBase* right) 
{  return doAssignment( left, doMinus(left, right) ); }

TokenBase*     ExpEvaluation::do_Multiply_Assignment(TokenBase* left, TokenBase* right) 
{  return doAssignment( left, doMultiply(left, right) ); }

TokenBase*     ExpEvaluation::do_Divide_Assignment(TokenBase* left, TokenBase* right) 
{  return doAssignment( left, doDivide(left, right) ); }

TokenBase*      ExpEvaluation::do_Mod_Assignment(TokenBase* left, TokenBase* right) 
{  return doAssignment( left,  doMod(left, right) ); }

TokenBase*      ExpEvaluation::do_Bit_And_Assignment(TokenBase* left, TokenBase* right) 
{  return doAssignment( left,  doBitAnd(left, right) ); }

TokenBase*      ExpEvaluation::do_Bit_Or_Assignment(TokenBase* left, TokenBase* right) 
{  return doAssignment( left,  doBitOr(left, right) ); }

TokenBase*      ExpEvaluation::do_Bit_Xor_Assignment(TokenBase* left, TokenBase* right)
{  return doAssignment( left,  doBitXor(left, right) ); }

TokenBase*      ExpEvaluation::do_Bit_LeftShift_Assignment(TokenBase* left, TokenBase* right)
{  return doAssignment( left,  doBitLeftShift(left, right) ); }

TokenBase*      ExpEvaluation::do_Bit_RightShift_Assignment(TokenBase* left, TokenBase* right)
{  return doAssignment( left,  doBitRightShift(left, right) ); }



//####################################################################################################
//
//      Unary Op
TokenBase* ExpEvaluation::doPositive(TokenBase* op, TokenBase* right)
{
    using namespace charUtil;

    string rightContent = right->getExpressionContent();
    string finalExpr = SC_OP_POSITIVE_BEGIN + rightContent;

    DataValue rightVal = right->getRealValue();
    E_DataType rightPromotionDt = operatorPrepairDataTypeConversion1(&rightVal);

    // calc 
    DataValue positiveRet = +rightVal;
    traceTmpOpResult(finalExpr, positiveRet);

    TokenBase* ret = generateTmpExpression(rightPromotionDt, finalExpr, op, right);
    if ( right->isFixedLiteral()  ) {
        auto base = right->getFixedLiteralBase();
        ret->setBaseAsFixedLiteral( base );
    }
    ret->setRealValue( positiveRet );

    return ret;
}

TokenBase* ExpEvaluation::doNegative(TokenBase* op, TokenBase* right)
{
    using namespace charUtil;

    string rightContent = right->getExpressionContent();
    string finalExpr = SC_OP_NEGATIVE_BEGIN + rightContent;

    DataValue rightVal = right->getRealValue(); 
    traceNegativeOperation(right);

    E_DataType rightPromotionDt = operatorPrepairDataTypeConversion1(&rightVal);

    // calc 
    DataValue negativeRet = -(rightVal);
    traceTmpOpResult(finalExpr, negativeRet);

    TokenBase* ret = generateTmpExpression(rightPromotionDt, finalExpr, op, right);
    if ( right->isFixedLiteral()  ) {
        auto base = right->getFixedLiteralBase();
        ret->setBaseAsFixedLiteral( base );
    }
    ret->setRealValue( negativeRet  );

    return ret;

}

TokenBase* ExpEvaluation::doBitNot(TokenBase* op, TokenBase* right)
{
    using namespace charUtil;

    string rightContent = right->getExpressionContent();
    string finalExpr = SC_OP_BIT_NOT_BEGIN + rightContent;

    DataValue rightVal = right->getRealValue();
    E_DataType rightPromotionDt = operatorPrepairDataTypeConversion1(&rightVal);

    if ( rightVal.type == E_TP_FLOAT  ||  rightVal.type == E_TP_DOUBLE ) {
        MyException e(E_THROW_BITNOT_CANNOT_APPLY_ON_FLOAT, right->getBeginPos() );
        e.setDetail( rightContent );
        throw e;
    } 


    // calc 
    DataValue bitNotRet   =    ~rightVal;
    traceTmpOpResult(finalExpr, bitNotRet);

    TokenBase* ret = generateTmpExpression(rightPromotionDt, finalExpr, op, right);
    ret->setRealValue( bitNotRet );

    return ret;
}





TokenBase* ExpEvaluation::doForceTypeCastConversion(TokenBase* op, TokenBase* right)
{
    E_DataType castType = op->getOpForceCastDataType();
    using namespace charUtil;

    string rightContent = right->getExpressionContent();
    string finalExpr =   STR_OPEN_PARENTHESIS + EnumUtil::enumName(castType) + STR_CLOSE_PARENTHESIS 
                       + STR_OPEN_PARENTHESIS + rightContent + STR_CLOSE_PARENTHESIS;

    DataValue rightVal = right->getRealValue();

    // calc 
    DataValue forceCastResultData = rightVal.doForceDataTypeCast( castType );
    traceTmpOpResult(finalExpr, forceCastResultData);

    TokenBase* ret = generateTmpExpression(castType, finalExpr, op, right);
    ret->setRealValue( forceCastResultData );

    return ret;
}







E_DataType ExpEvaluation::operatorPrepairDataTypeConversion1(DataValue* pRightVal)
{
    auto retType = pRightVal->type;
    auto rightpr = DataTypeUtil::needDoIntegerPromotion( pRightVal->type );
    if ( rightpr.first ) {
        pRightVal->doIntergerPromotion( rightpr.second );
        // set new data Type after finishing interger Promotion 
        retType = rightpr.second;
    } 

    return retType;

}






E_DataType ExpEvaluation::operatorPrepairDataTypeConversion2(DataValue* pLeftVal, DataValue* pRightVal)
{
    auto setFlag = false;
    auto retType = pLeftVal->type;
    auto leftpr = DataTypeUtil::needDoIntegerPromotion( pLeftVal->type );
    if ( leftpr.first ) {
        pLeftVal->doIntergerPromotion( leftpr.second );
        // set new data Type after finishing interger Promotion 
        setFlag = true;
        retType = leftpr.second;
    }

    auto rightpr = DataTypeUtil::needDoIntegerPromotion( pRightVal->type );
    if ( rightpr.first ) {
        pRightVal->doIntergerPromotion( rightpr.second );
    }

    auto cvtPrpr = DataTypeUtil::needDoConvertion( pLeftVal->type, pRightVal->type );
    auto retDt = cvtPrpr.first.second;
    if ( cvtPrpr.first.first ) {
        pLeftVal->doConvertion( retDt );
    }

    if ( cvtPrpr.second.first ) {
        pRightVal->doConvertion( retDt );
    }

    if ( setFlag ) {
        retType = retDt;
    }

    return retType;

}

void ExpEvaluation::traceUnInitializedVarWhenUsed(TokenBase* pToken)
{

    using namespace charUtil;

    if ( CmdOptions::needPrintParseRuntimeWarning() ) {
        cerr << SC_WARNING_TITLE;
        cerr << " varible named " << SINGLE_QUOTO << pToken->getTokenContent() << SINGLE_QUOTO << " is not initialized before used " << endl;
    }

}

void ExpEvaluation::traceTmpOpResult(const std::string& expr, DataValue& retValue)
{
    if ( CmdOptions::needTraceTmpExpressionProcess() ) {
        TypeBaseInfo retTpInfo(retValue.type);
        cerr << "[INFO] : " << expr << " => " << EnumUtil::enumName( retTpInfo.getType() ) << " , value = " << retValue.getPrintValue(0) << endl;
    }
}


void ExpEvaluation::traceBitShiftWarning(bool isLeftBitShift, TokenBase* left,  TokenBase* right)
{
    using namespace charUtil;

    // Core Core Core : config this flag
    static const bool SC_B_SHOULD_ALWAYS_CHECK_RIGHT_VALUE = true;

    if (  CmdOptions::needPrintParseRuntimeWarning()  ) {
        auto hasprintFlag = false;
        auto hasLeftWarningFlag = false;
        auto hasRightWarningFlag = false;

        TypeBaseInfo leftTpInfo(  left->getRealValue().type );
        TypeBaseInfo rightTpInfo( right->getRealValue().type );
        DataValue rightVal = right->getRealValue();

        auto leftusFlag = leftTpInfo.isUnsignedType();
        if ( CmdOptions::needTreatSignedIntergerBitShiftAsWarning() &&  left->isVarible() &&  !leftusFlag ) {
            cerr << SC_WARNING_TITLE;
            cerr << " calculating : " << (isLeftBitShift ? "<<" : ">>") << endl;
            cerr << "leftOperand is not unsigned type : "  << EnumUtil::enumName(leftTpInfo.getType()) << " : " << left->getTokenContent() << SPACE_1 << "@" << left->getBeginPos().getPos(0) << endl;
            hasprintFlag = true;
            hasLeftWarningFlag = true;
        }

        auto checkRightFlag =  SC_B_SHOULD_ALWAYS_CHECK_RIGHT_VALUE || right->isVarible();

        if ( !checkRightFlag )  {
            return;
        }

        int leftallbits = leftTpInfo.getBits();
        auto rightusFlag = rightTpInfo.isUnsignedType();
        if ( !rightusFlag ) {
            // with  +/-
            if ( rightVal.isNegative() ) {
                if ( !hasprintFlag ) {
                    cerr << SC_WARNING_TITLE;
                    cerr << " calculating : " << (isLeftBitShift ? "<<" : ">>") << endl;
                    hasprintFlag = true;
                }

                cerr << "rightOperand's value <= 0 , type = "  << EnumUtil::enumName(rightTpInfo.getType()) << " : " << right->getTokenContent() << " = " << rightVal.getPrintValue(0) << endl;
                hasRightWarningFlag = true;
            } else {
                // >=0
                if ( rightVal.isGreaterEqualBitsWidth(leftallbits) ) {
                    if ( !hasprintFlag ) {
                        cerr << SC_WARNING_TITLE;
                        cerr << " calculating : " << (isLeftBitShift ? "<<" : ">>") << endl;
                        hasprintFlag = true;
                    }

                    cerr << "rightOperand's value >= "  << leftallbits << " (Bits-Width) , " << right->getTokenContent() << " = " << rightVal.getPrintValue(0) << endl;
                    hasRightWarningFlag = true;
                }
            }
        } else {
            // >=0
            if ( rightVal.isGreaterEqualBitsWidth(leftallbits) ) {
                if ( !hasprintFlag ) {
                    cerr << SC_WARNING_TITLE;
                    cerr << " calculating : " << (isLeftBitShift ? "<<" : ">>") << endl;
                    hasprintFlag = true;
                }

                cerr << "rightOperand's value > "  << leftallbits << " (Bits-Width) , " << right->getTokenContent() << " = " << rightVal.getPrintValue(0) << endl;
                hasRightWarningFlag = true;
            }
        }


        if ( hasLeftWarningFlag || hasRightWarningFlag ) {
            cerr << endl;
        }
    }
}

void ExpEvaluation::traceNegativeOperation(TokenBase* right)
{
    using namespace charUtil;

    if ( CmdOptions::needPrintParseRuntimeWarning()  ) {
        if ( right->isVarible()  ) {
            DataValue rightVal = right->getRealValue();

            TypeBaseInfo tpInfo( rightVal.type );
            if ( tpInfo.isIntegerFamily() ) {

                if ( tpInfo.isUnsignedType() ) {
                    cerr << SC_WARNING_TITLE;
                    cerr << " operator - (negative) on a unsigned varible \""<< right->getTokenContent() << "\"  doesn't take effect on a number whose value is always >= 0 " << endl;
                } else if ( rightVal.isMinimumNegativeNumber() ) {
                    cerr << SC_WARNING_TITLE;
                    cerr << " operator - (negative) on signed varible \""<< right->getTokenContent() << "\"  doesn't take effect on a number ( minimum negative number ) to making a result as a +number > 0" << endl;
                }
            }
        }
    }

}


void ExpEvaluation::traceAssignOverFlow(TokenBase* leftVar, TokenBase* rightFixedInt)
{
    using namespace charUtil;

    if ( CmdOptions::needCheckFixedLiteralIntRangeWhenAssign()   &&   rightFixedInt->isFixedLiteral() ) {
        TypeBaseInfo  leftVarTpInfo(  leftVar->getDataType() );
        TypeBaseInfo rightFixTpInfo( rightFixedInt->getDataType() );
        if (          leftVarTpInfo.isIntegerFamily()
              &&     rightFixTpInfo.isIntegerFamily()  ) 
        {
            if ( rightFixTpInfo.getLevel() > leftVarTpInfo.getLevel() ) {
                DataValue rVal = rightFixedInt->getRealValue();
                string strMinVal("???_MinVal");
                string strMaxVal("???_MaxVal");
                if ( rVal.isIntOutOfRange( leftVar->getDataType(), strMinVal, strMaxVal ) ) {
                    cerr << SC_WARNING_TITLE;
                    cerr << " Sentence : " << SINGLE_QUOTO << EnumUtil::enumName(leftVar->getDataType()) << SPACE_1 << leftVar->getTokenContent() << " = "
                         << rightFixedInt->getTokenContent() << SINGLE_QUOTO << " is out of range ( " << strMinVal << " ~ " << strMaxVal << " ) " 
                         << "@line : " << rightFixedInt->getBeginPos().line
                         << endl;
                }
            }
        }
        
    }
}




// static 
TokenBase* ExpEvaluation::generateTmpExpression(E_DataType dt, const std::string& expression, TokenBase* begtoken, TokenBase* endtoken)
{
    TokenBase* pTmpExpression = new TokenBase(dt);
    pTmpExpression->setAsTmpExpression();
    pTmpExpression->setTokenContent( expression );
    pTmpExpression->setGeneratedExp( expression );
    pTmpExpression->setBeginPos( begtoken->getBeginPos() );
    pTmpExpression->setEndPos( endtoken->getEndPos() );

    s_generatedTmpTokenPool.push_back( pTmpExpression );
    return pTmpExpression;
}


// static 
TokenBase* ExpEvaluation::generateTmpExpression_4ForceTypeCast(E_DataType dt, const std::string& expression, TokenBase* begtoken, TokenBase* endtoken)
{
    TokenBase* pForceCastOperator = new TokenBase( E_TOKEN_OPERATOR );
    pForceCastOperator->setOpType( E_FORCE_TYPE_CAST );
    pForceCastOperator->setOpForceCastDataType( dt );
    // pTmpExpression->setAsTmpExpression();
    pForceCastOperator->setTokenContent( expression );
    pForceCastOperator->setGeneratedExp( expression );
    pForceCastOperator->setBeginPos( begtoken->getBeginPos() );
    pForceCastOperator->setEndPos( endtoken->getEndPos() );

    s_generatedTmpTokenPool.push_back( pForceCastOperator );
    return pForceCastOperator;
}




