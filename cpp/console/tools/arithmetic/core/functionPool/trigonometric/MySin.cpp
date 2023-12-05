#include <cmath>
#include "MySin.h"
using namespace std;

My_sin::My_sin() : FunctionBase() 
{
	m_funcName = "sin";
}

// virtual
My_sin::~My_sin() 
{
}

// virtual 
TokenBase* My_sin::doCall() // override
{
    // TODO :       return   sin(...);    sin( pi / 6 ) = sin ( 30 degree ) = 0.5
    return nullptr;
}

// virtual 
int My_sin::getRequiredArgumentsCount() // override;
{
    return 1;
}

