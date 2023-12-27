#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyPow.h"
using namespace std;

My_pow::My_pow() : FunctionBase() 
{
	m_funcName = "pow";
}

// virtual
My_pow::~My_pow() 
{
}

// virtual 
TokenBase* My_pow::doCall() // override
{
    //
    // Use this version   :   double pow(double base, double exp)
    //          return   pow(...);  => pow(4,3) =>  4*4*4 = 56
    //
    //                           (arg)
    //          result  = base ^
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
    double dRet = pow( argValue1.value.double_val,  argValue2.value.double_val);

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_pow::getRequiredArgumentsCount() // override;
{
    return 2;
}

