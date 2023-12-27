#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyCos.h"
using namespace std;

My_cos::My_cos() : FunctionBase() 
{
	m_funcName = "cos";
}

// virtual
My_cos::~My_cos() 
{
}

// virtual 
TokenBase* My_cos::doCall() // override
{
    //
    // Use this version   :   double cos(double arg)
    //          return   cos(...);  =>  cos( pi/3 ) = cos( 60 degree ) = 0.5
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = cos( argValue1.value.double_val);

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_cos::getRequiredArgumentsCount() // override;
{
    return 1;
}

