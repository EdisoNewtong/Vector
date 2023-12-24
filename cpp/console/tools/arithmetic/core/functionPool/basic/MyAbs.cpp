#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyAbs.h"


using namespace std;

My_abs::My_abs() : FunctionBase() 
{
	m_funcName = "abs";
}

// virtual
My_abs::~My_abs() 
{
}

// virtual 
TokenBase* My_abs::doCall() // override
{
    //
    // Use this version   :   double abs( double arg);
    //
    auto arg = m_argResultList.front();
    DataValue argValue = arg->getRealValue();
    argValue.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = abs( argValue.value.double_val );
    argValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( argValue );

    return retToken;
}

// virtual 
int My_abs::getRequiredArgumentsCount() // override;
{
    return 1;
}


