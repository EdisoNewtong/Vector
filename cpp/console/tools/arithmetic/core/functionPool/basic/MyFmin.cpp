#include <cmath>
#include "MyFmin.h"
using namespace std;

My_fmin::My_fmin() : FunctionBase()
{
	m_funcName = "fmin";
}

// virtual
My_fmin::~My_fmin() 
{
}

// virtual 
TokenBase* My_fmin::doCall() // override
{
    // TODO :       return   abs(...);
    return nullptr;
}

// virtual 
int My_fmin::getRequiredArgumentsCount() // override;
{
    return 2;
}

