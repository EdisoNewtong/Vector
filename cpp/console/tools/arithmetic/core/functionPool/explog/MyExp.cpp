#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyExp.h"
using namespace std;

My_exp::My_exp() : FunctionBase() 
{
	m_funcName = "exp";
}

// virtual
My_exp::~My_exp() 
{
}

// virtual 
TokenBase* My_exp::doCall() // override
{
    //
    // Use this version   :   double exp(double arg)
    //           return   exp(...);  =>  exp(1) = e = 2.718  ,    exp(2) = e*e = 7.387
    //
    //                       (arg)
    //          result  = e ^
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = exp( argValue1.value.double_val);

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_exp::getRequiredArgumentsCount() // override;
{
    return 1;
}

