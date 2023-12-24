#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyLog10.h"
using namespace std;

My_log10::My_log10() : FunctionBase() 
{
	m_funcName = "log10";
}

// virtual
My_log10::~My_log10() 
{
}

// virtual 
TokenBase* My_log10::doCall() // override
{
    //
    // Use this version   :   double log(double arg)
    //           return   log10(...);   =>   log(10) = 1 /  log(10*10) = 2 /  log(10*10*10) = 3
    //                       
    //          result  = log  (x)
    //                       10 
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = log10( argValue1.value.double_val );

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;

}

// virtual 
int My_log10::getRequiredArgumentsCount() // override;
{
    return 1;
}

