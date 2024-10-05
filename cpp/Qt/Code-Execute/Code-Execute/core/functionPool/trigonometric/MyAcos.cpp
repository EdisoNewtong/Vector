#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyAcos.h"
using namespace std;

My_acos::My_acos() : FunctionBase() 
{
	m_funcName = "acos";
}

// virtual
My_acos::~My_acos() 
{
}

// virtual 
TokenBase* My_acos::doCall() // override
{
    //
    // Use this version   :   double acos(double arg)
    //          return   acos(...);    acos( 0.5 ) = (pi / 3) = ( 60 degree )
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = acos( argValue1.value.double_val);

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_acos::getRequiredArgumentsCount() // override;
{
    return 1;
}

