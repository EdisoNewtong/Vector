#include <cmath>
#include "MySqrt.h"
using namespace std;

My_sqrt::My_sqrt() : FunctionBase() 
{
	m_funcName = "sqrt";
}

// virtual
My_sqrt::~My_sqrt() 
{
}

// virtual 
TokenBase* My_sqrt::doCall() // override
{
    // TODO :       return   sqrt(...);     sqrt(16) = 4
    return nullptr;
}

// virtual 
int My_sqrt::getRequiredArgumentsCount() // override;
{
    return 1;
}

