#include <cmath>
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
    // TODO :       return   round(...);   
    return nullptr;
}

// virtual 
int My_round::getRequiredArgumentsCount() // override;
{
    return 1;
}
