#include <cmath>
#include "MyCeil.h"
using namespace std;

My_ceil::My_ceil() : FunctionBase() 
{
	m_funcName = "ceil";
}

// virtual
My_ceil::~My_ceil() 
{
}

// virtual 
TokenBase* My_ceil::doCall() // override
{
    // TODO :       return   ceil(...);   =>   ceil(3.5) = 4    ceil(-3.5) = -3
    return nullptr;
}

// virtual 
int My_ceil::getRequiredArgumentsCount() // override;
{
    return 1;
}
