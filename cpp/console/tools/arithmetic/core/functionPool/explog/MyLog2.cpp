#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyLog2.h"
using namespace std;

My_log2::My_log2() : FunctionBase() 
{
	m_funcName = "log2";
}

// virtual
My_log2::~My_log2() 
{
}

// virtual 
TokenBase* My_log2::doCall() // override
{
    //
    // Use this version   :   double log2(double arg)
    //           return   log2(...);  =>   log(2) = 1 / log(2*2) = 2;   log(2*2*2) = 3;
    //                       
    //          result  = log (x)
    //                       2
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = log2( argValue1.value.double_val );

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_log2::getRequiredArgumentsCount() // override;
{
    return 1;
}

