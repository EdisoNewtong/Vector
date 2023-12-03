
#include "functionBase.h"
#include "tokenBase.h"

#include "commonEnum.h"
#include "myException.h"

FunctionBase::FunctionBase() 
    : m_funcName()
    , m_argumentList()
{

}

FunctionBase::~FunctionBase()
{

}


TokenBase* FunctionBase::executeFunction()
{
    preCheckArgCount();
    auto retValue = doCall();

	// After call , Delete argument list
	m_argumentList.clear();

	return retValue;
}


void FunctionBase::preCheckArgCount()
{
    int argCnt = static_cast<int>( m_argumentList.size() );
    int requiredCnt = getRequiredArgumentsCount();
    if ( argCnt >  requiredCnt )  {
		MyException e(E_THROW_CALL_FUNCTION_TOO_MANY_ARGUMENTS, m_argumentList.back().back()->getBeginPos() );
        throw e;
    } else if ( argCnt <  requiredCnt ) {
		MyException e(E_THROW_CALL_FUNCTION_TOO_LITTLE_ARGUMENTS, m_argumentList.back().back()->getBeginPos() );
		throw e;
    }

}


void FunctionBase::pushOneArgumentExpr( const std::list<TokenBase*>& oneArgumentExpr )
{
	using namespace std;

    int requiredCnt = getRequiredArgumentsCount();
	int existedSz = static_cast<int>( m_argumentList.size() );
	if ( (existedSz + 1) > requiredCnt ) {
		MyException e(E_THROW_PARSE_FUNCTION_PUSH_TOO_MANY_ARGUMENTS, oneArgumentExpr.back()->getBeginPos() );
		e.setDetail( string("Required argument count = ") + to_string(requiredCnt) + string(", a new argument #") + to_string(existedSz + 1) + string(" to be pushed. ") );
        throw e;
	}

	m_argumentList.push_back( oneArgumentExpr );
}

