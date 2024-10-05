#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MySin.h"
using namespace std;

My_sin::My_sin() : FunctionBase() 
{
	m_funcName = "sin";
}

// virtual
My_sin::~My_sin() 
{
}

// virtual 
TokenBase* My_sin::doCall() // override
{
    //
    // Use this version   :   double sin(double arg)
    //          return   sin(...);    sin( pi / 6 ) = sin ( 30 degree ) = 0.5
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = sin( argValue1.value.double_val);

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_sin::getRequiredArgumentsCount() // override;
{
    return 1;
}

