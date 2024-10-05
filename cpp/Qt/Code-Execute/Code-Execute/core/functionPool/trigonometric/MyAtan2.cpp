#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyAtan2.h"
using namespace std;

My_atan2::My_atan2() : FunctionBase() 
{
	m_funcName = "atan2";
}

// virtual
My_atan2::~My_atan2() 
{
}

// virtual 
TokenBase* My_atan2::doCall() // override
{
    //
    // Use this version   :   double sin(double arg)
    //          return   atan2(...);  => atan2(x,y)   =>   arctan( y/x )
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
    double dRet = atan2( argValue1.value.double_val,  argValue2.value.double_val);

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_atan2::getRequiredArgumentsCount() // override;
{
    return 2;
}

