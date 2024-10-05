#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyCbrt.h"
using namespace std;

My_cbrt::My_cbrt() : FunctionBase() 
{
	m_funcName = "cbrt";
}

// virtual
My_cbrt::~My_cbrt() 
{
}

// virtual 
TokenBase* My_cbrt::doCall() // override
{
    //
    // Use this version   :   double cbrt(double arg)
    //          return   return   cbrt(...);    ,   cbrt(27) = 3     because   3*3*3 = 27
    //
    //                           (1/3)
    //          result  = base ^
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = cbrt( argValue1.value.double_val);

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_cbrt::getRequiredArgumentsCount() // override;
{
    return 1;
}

