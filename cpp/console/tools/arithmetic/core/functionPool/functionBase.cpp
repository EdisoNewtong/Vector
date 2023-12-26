#include "functionBase.h"
#include "functionMgr.h"
#include "expEvaluation.h"
#include "tokenBase.h"

#include "commonEnum.h"
#include "myException.h"

#include <iterator>
using namespace std;

FunctionBase::FunctionBase() 
    : m_funcName("???")
    , m_argumentList()
    , m_argResultList()
    , m_argStrSuffixExprList()
    , m_pFuncToken( nullptr )
    , m_pOpenParenthesis( nullptr )
    , m_pCloseParenthesis( nullptr )
{

}

FunctionBase::~FunctionBase()
{
    m_argumentList.clear();

    m_argResultList.clear();
    m_argStrSuffixExprList.clear();

    m_pFuncToken = nullptr;
    m_pOpenParenthesis = nullptr;
    m_pCloseParenthesis = nullptr;

}


TokenBase* FunctionBase::executeFunction()
{
    if ( m_pCloseParenthesis == nullptr  ) {
        MyException e(E_THROW_FUNCTION_IS_NOT_VALID_BY_MISSING_CLOSE_PARENTHESIS, m_pOpenParenthesis->getBeginPos() );
        e.setDetail( string("function.name = ") + m_funcName );
        throw e;
    }

    auto needThrow = true;
    preCheckArgCount( needThrow );

    // prepare argument expression evaluation and a certain argument list's  string suffix
    m_argResultList.clear();
    m_argStrSuffixExprList.clear();
    auto needCheckVarible = true;
    if ( FunctionMgr::isUseStdCallConvension() ) {
        for ( auto rit = m_argumentList.rbegin(); rit != m_argumentList.rend(); ++rit ) {
            m_argStrSuffixExprList.push_front( FunctionBase::getSuffixExpString( *rit ) );

            auto expEvaObject = ExpEvaluation::createANewEvaluator();
            auto oneArgValue = expEvaObject->evaluateSuffixExpression( *rit, needCheckVarible );
            m_argResultList.push_front( oneArgValue );
        }
    } else {
        for( auto fit = m_argumentList.begin(); fit != m_argumentList.end(); ++fit ) {
            m_argStrSuffixExprList.push_back( FunctionBase::getSuffixExpString( *fit ) );

            auto expEvaObject = ExpEvaluation::createANewEvaluator();
            auto oneArgValue = expEvaObject->evaluateSuffixExpression( *fit, needCheckVarible );
            m_argResultList.push_back( oneArgValue );
        }
    }


    auto retValue = doCall();

    // After call , Delete argument list
    m_argumentList.clear();

    return retValue;
}


bool FunctionBase::preCheckArgCount(bool needThrow)
{
    int argCnt = static_cast<int>( m_argumentList.size() );
    int requiredCnt = getRequiredArgumentsCount();

    auto isMatched =  (argCnt != requiredCnt);
    if ( needThrow ) {
        if ( argCnt >  requiredCnt )  {
            MyException e(E_THROW_CALL_FUNCTION_TOO_MANY_ARGUMENTS, m_argumentList.back().back()->getBeginPos() );
            e.setDetail( string("function.name = ") + m_funcName );
            throw e;
        } else if ( argCnt <  requiredCnt ) {
            if ( argCnt != 0 ) {
                MyException e(E_THROW_CALL_FUNCTION_TOO_LITTLE_ARGUMENTS, m_argumentList.back().back()->getBeginPos() );
                e.setDetail( string("function.name = ") + m_funcName );
                throw e;
            } else {
                MyException e(E_THROW_CALL_FUNCTION_TOO_LITTLE_ARGUMENTS, m_pCloseParenthesis->getBeginPos() );
                e.setDetail( string("function.name = ") + m_funcName + string(" . it has 0 argument   "));
                throw e;
            }
        }
    } 

    return isMatched;
}


void FunctionBase::pushOneArgumentExpr( const std::list<TokenBase*>& oneArgumentExpr )
{
    using namespace std;

    int requiredCnt = getRequiredArgumentsCount();
    int existedSz = static_cast<int>( m_argumentList.size() );
    if ( (existedSz + 1) > requiredCnt ) {
        MyException e(E_THROW_PARSE_FUNCTION_PUSH_TOO_MANY_ARGUMENTS, oneArgumentExpr.back()->getBeginPos() );
        e.setDetail( m_funcName + string(" , it required argument count = ") + to_string(requiredCnt) + string(", a new argument #") + to_string(existedSz + 1) + string(" to be pushed. ") );
        throw e;
    }

    m_argumentList.push_back( oneArgumentExpr );
}



void FunctionBase::setCloseParenthesisValidFlag(TokenBase* pEndParenthesis)
{
    m_pCloseParenthesis = pEndParenthesis;
    preCheckArgCount( true );
}


bool FunctionBase::isFunctionEndupValid()
{
    return m_pCloseParenthesis != nullptr;
}


void  FunctionBase::setBeginParenthesis(TokenBase* funcName, TokenBase* openParenthesis)
{
    m_pFuncToken = funcName;
    m_pOpenParenthesis = openParenthesis;
}


 
// static 
string  FunctionBase::getSuffixExpString( const list<TokenBase*>& oneArgumentExpr)
{
    string retStrExp;
    for( auto it = oneArgumentExpr.begin(); it != oneArgumentExpr.end(); ++it ) {
        if ( *it != nullptr ) {
            if ( (*it)->isFunction() ) {
                retStrExp += ( (*it)->getTokenContent() + string("( )") );
            } else {
                retStrExp += (*it)->getTokenContent();
            }
            retStrExp += string(" ");
        }
    }

    return retStrExp;
}


string  FunctionBase::buildFuncitonDetailString(const list<string>& strExpList)
{
    string retStrExp = m_funcName + string("( ");

    for ( auto it = strExpList.begin(); it != strExpList.end(); ++it ) {
        retStrExp += (*it);
        auto nextIt = next(it); // require header file <iterator>
        if ( nextIt != strExpList.end() ) {
            retStrExp += string(" , ");
        }
    }

    retStrExp += string(" )");
    return retStrExp;
}


