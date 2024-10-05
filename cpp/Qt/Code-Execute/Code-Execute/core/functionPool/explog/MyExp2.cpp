#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyExp2.h"
using namespace std;

My_exp2::My_exp2() : FunctionBase() 
{
	m_funcName = "exp2";
}

// virtual
My_exp2::~My_exp2() 
{
}

// virtual 
TokenBase* My_exp2::doCall() // override
{    
    //
    // Use this version   :   double exp2(double arg)
    //          return   exp2(...);   =>   exp2( 5 ) = 2*2*2*2*2 = 32;
    //
    //                       (arg)
    //          result  = 2 ^
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = exp2( argValue1.value.double_val);

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}


// virtual 
int My_exp2::getRequiredArgumentsCount() // override;
{
    return 1;
}

