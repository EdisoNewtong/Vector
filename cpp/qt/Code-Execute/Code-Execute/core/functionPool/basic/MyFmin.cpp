#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyFmin.h"
using namespace std;

My_fmin::My_fmin() : FunctionBase()
{
	m_funcName = "fmin";
}

// virtual
My_fmin::~My_fmin() 
{
}

// virtual 
TokenBase* My_fmin::doCall() // override
{
    //
    // Use this version   :   double fmin(double x, double y);
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
    double dRet = fmin( argValue1.value.double_val, argValue2.value.double_val );

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_fmin::getRequiredArgumentsCount() // override;
{
    return 2;
}
