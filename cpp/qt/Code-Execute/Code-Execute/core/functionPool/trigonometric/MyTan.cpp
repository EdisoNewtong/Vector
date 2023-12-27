#include <cmath>
#include "tokenBase.h"
#include "dataValue.h"
#include "expEvaluation.h"
#include "MyTan.h"
using namespace std;

My_tan::My_tan() : FunctionBase() 
{
	m_funcName = "tan";
}

// virtual
My_tan::~My_tan() 
{
}

// virtual 
TokenBase* My_tan::doCall() // override
{
    //
    // Use this version   :   double tan(double arg)
    //          return   tan(...);     tan( pi/4 ) = tan ( 45 degree ) = 1
    //
    auto it = m_argResultList.begin();

    auto arg1 = *it;
    DataValue argValue1 = arg1->getRealValue();
    argValue1.forceCast( E_TP_DOUBLE );

    // call system api function
    double dRet = tan( argValue1.value.double_val);

    DataValue dRetDataValue;
    dRetDataValue.type = E_TP_DOUBLE;
    dRetDataValue.value.double_val = dRet;

    auto funcExp = buildFuncitonDetailString( m_argStrSuffixExprList );
    auto retToken = ExpEvaluation::generateTmpExpression(E_TP_DOUBLE , funcExp, m_pFuncToken, m_pCloseParenthesis);
    retToken->setRealValue( dRetDataValue );
    return retToken;
}

// virtual 
int My_tan::getRequiredArgumentsCount() // override;
{
    return 1;
}

