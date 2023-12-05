#include <cmath>
#include "MyExp.h"
using namespace std;

My_exp::My_exp() : FunctionBase() 
{
	m_funcName = "exp";
}

// virtual
My_exp::~My_exp() 
{
}

// virtual 
TokenBase* My_exp::doCall() // override
{
    // TODO :       return   exp(...);  =>  exp(1) = e = 2.718  ,    exp(2) = e*e = 7.387
    return nullptr;
}

// virtual 
int My_exp::getRequiredArgumentsCount() // override;
{
    return 1;
}

