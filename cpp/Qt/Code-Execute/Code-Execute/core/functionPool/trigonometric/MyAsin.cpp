#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyAsin.h"
using namespace std;

My_asin::My_asin() : FunctionBase() 
{
	m_funcName = "asin";
}

// virtual
My_asin::~My_asin() 
{
}

// virtual 
TokenBase* My_asin::doCall() // override
{
    //
    // Use this version   :   double asin(double arg)
    //          return   asin(...);    asin( 0.5 ) = pi/6 = ( 30 degree )
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = asin( argValue1.value.double_val);

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_asin::getRequiredArgumentsCount() // override;
{
    return 1;
}

