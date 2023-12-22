#include <cmath>
#include "expEvaluation.h"
#include "functionMgr.h"
#include "MyAbs.h"
using namespace std;

My_abs::My_abs() : FunctionBase() 
{
	m_funcName = "abs";
}

// virtual
My_abs::~My_abs() 
{
}

// virtual 
TokenBase* My_abs::doCall() // override
{
    // Use this version   :   double abs( double arg);
    // static TokenBase* generateTmpExpression(E_DataType dt, const std::string& expression, TokenBase* begtoken, TokenBase* endtoken);
    // void DataValue::doConvertion(E_DataType destinationTp)

	std::list<TokenBase*> argResult;
	if ( FunctionMgr::isUseStdCallConvension() ) {
		for ( auto rit = m_argumentList.rbegin(); rit != m_argumentList.rend(); ++rit ) {
			auto expEvaObject = ExpEvaluation::createANewEvaluator();
			auto retOne = expEvaObject->evaluateSuffixExpression( *rit );
			argResult.push_front( retOne );
		}
	} else {
		for( auto fit = m_argumentList.begin(); fit != m_argumentList.end(); ++fit ) {
			auto expEvaObject = ExpEvaluation::createANewEvaluator();
			auto retOne = expEvaObject->evaluateSuffixExpression( *fit );
			argResult.push_back( retOne );
		}
	}

	// TODO abs( 1.0)
	return nullptr;
}

// virtual 
int My_abs::getRequiredArgumentsCount() // override;
{
    return 1;
}
