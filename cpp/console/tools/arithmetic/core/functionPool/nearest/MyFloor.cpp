#include <cmath>
#include "MyFloor.h"
using namespace std;

My_floor::My_floor() : FunctionBase() 
{
	m_funcName = "floor";
}

// virtual
My_floor::~My_floor() 
{
}

// virtual 
TokenBase* My_floor::doCall() // override
{
    // TODO :       return   floor(...);   =>   floor(3.5) = 3    floor(-3.5) = -4
    return nullptr;
}

// virtual 
int My_floor::getRequiredArgumentsCount() // override;
{
    return 1;
}
