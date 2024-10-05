#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyTrunc.h"
using namespace std;

My_trunc::My_trunc() : FunctionBase() 
{
	m_funcName = "trunc";
}

// virtual
My_trunc::~My_trunc() 
{
}

// virtual 
TokenBase* My_trunc::doCall() // override
{
    //
    // Use this version   :   double trunc(double arg)
    //          return   trunc(...);   =>   trunc(3.5) = 3    trunc(-3.5) = -3
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = trunc( argValue1.value.double_val );

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_trunc::getRequiredArgumentsCount() // override;
{
    return 1;
}

