#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyHypot.h"
using namespace std;

My_hypot::My_hypot() : FunctionBase() 
{
	m_funcName = "hypot";
}

// virtual
My_hypot::~My_hypot() 
{
}

// virtual 
TokenBase* My_hypot::doCall() // override
{
    //
    // Use this version   :   double hypot(double a, double b)
    //          return   hypot(...);    ,  return  sqrt( x^2 + y^2 )
    //
    //                                        (1/2)
    //          result  = ( a * a + b * b ) ^
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    ++it;
    auto arg2 = *it;
    DataValue argValue2 = arg2->getRealValue();
    argValue2.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = hypot( argValue1.value.double_val,  argValue2.value.double_val);

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_hypot::getRequiredArgumentsCount() // override;
{
    return 2;
}

