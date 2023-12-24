#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyAtan.h"
using namespace std;

My_atan::My_atan() : FunctionBase() 
{
	m_funcName = "atan";
}

// virtual
My_atan::~My_atan() 
{
}

// virtual 
TokenBase* My_atan::doCall() // override
{
    //
    // Use this version   :   double atan(double arg)
    //          return   atan(...);  =>   atan(1) = pi/4 = ( 45 degree )
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = atan( argValue1.value.double_val);

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_atan::getRequiredArgumentsCount() // override;
{
    return 1;
}

