#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyLog.h"
using namespace std;

My_log::My_log() : FunctionBase() 
{
	m_funcName = "log";
}

// virtual
My_log::~My_log() 
{
}

// virtual 
TokenBase* My_log::doCall() // override
{
    //
    // Use this version   :   double log(double arg)
    //           return   log(...);   =>  / ln(x) => ln(e) = 1    /  ln(e*e) = 2 /  ln( e*e*e ) = 3
    //                       
    //          result  = log (x)
    //                       e
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = log( argValue1.value.double_val );

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}


// virtual 
int My_log::getRequiredArgumentsCount() // override;
{
    return 1;
}

