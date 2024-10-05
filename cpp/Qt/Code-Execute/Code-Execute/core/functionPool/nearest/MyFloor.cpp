#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyFloor.h"
using namespace std;

My_floor::My_floor() : FunctionBase() 
{
	m_funcName = "floor";
}

// virtual
My_floor::~My_floor() 
{
}

// virtual 
TokenBase* My_floor::doCall() // override
{
    //
    // Use this version   :   double ceil(double arg)
    //          return   floor(...);   =>   floor(3.5) = 3    floor(-3.5) = -4
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = floor( argValue1.value.double_val );

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_floor::getRequiredArgumentsCount() // override;
{
    return 1;
}

