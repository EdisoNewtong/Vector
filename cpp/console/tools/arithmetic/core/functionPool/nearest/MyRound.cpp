#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyRound.h"
using namespace std;

My_round::My_round() : FunctionBase() 
{
	m_funcName = "round";
}

// virtual
My_round::~My_round() 
{
}

// virtual 
TokenBase* My_round::doCall() // override
{

	/****************************************************************************************************
	
	    round(+2.3) = 2  round(+2.5) = 3  round(+2.7) = 3
	    round(-2.3) = -2  round(-2.5) = -3  round(-2.7) = -3
	    round(-0.0) = -0
	    round(-Inf) = -inf
	
	****************************************************************************************************/
    // 
    // Use this version   :   double round(double arg)
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = round( argValue1.value.double_val );

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_round::getRequiredArgumentsCount() // override;
{
    return 1;
}
