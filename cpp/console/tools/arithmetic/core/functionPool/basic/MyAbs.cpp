#include <cmath>
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
    // TODO :       return   abs(...);
    return nullptr;
}

// virtual 
int My_abs::getRequiredArgumentsCount() // override;
{
    return 1;
}
