#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MySqrt.h"
using namespace std;

My_sqrt::My_sqrt() : FunctionBase() 
{
	m_funcName = "sqrt";
}

// virtual
My_sqrt::~My_sqrt() 
{
}

// virtual 
TokenBase* My_sqrt::doCall() // override
{
    //
    // Use this version   :   double sqrt(double arg)
    //          return   sqrt(...);  => sqrt(121) = 11
    //
    //                          (1/2)
    //          result  = arg ^
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = sqrt( argValue1.value.double_val);

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;

}

// virtual 
int My_sqrt::getRequiredArgumentsCount() // override;
{
    return 1;
}

