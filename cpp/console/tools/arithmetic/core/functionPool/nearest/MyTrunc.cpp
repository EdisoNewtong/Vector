#include <cmath>
#include "MyTrunc.h"
using namespace std;

My_trunc::My_trunc() : FunctionBase() 
{
	m_funcName = "trunc";
}

// virtual
My_trunc::~My_trunc() 
{
}

// virtual 
TokenBase* My_trunc::doCall() // override
{
    // TODO :       return   trunc(...);   =>   ceil(3.5) = 3    ceil(-3.5) = -3
    return nullptr;
}

// virtual 
int My_trunc::getRequiredArgumentsCount() // override;
{
    return 1;
}
