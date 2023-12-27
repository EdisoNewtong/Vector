#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyCeil.h"
using namespace std;

My_ceil::My_ceil() : FunctionBase() 
{
	m_funcName = "ceil";
}

// virtual
My_ceil::~My_ceil() 
{
}

// virtual 
TokenBase* My_ceil::doCall() // override
{
    //
    // Use this version   :   double ceil(double arg)
    //          return   ceil(...);   =>   ceil(3.5) = 4    ceil(-3.5) = -3
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = ceil( argValue1.value.double_val );

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_ceil::getRequiredArgumentsCount() // override;
{
    return 1;
}

