#include <cmath>
#include "MyHypot.h"
using namespace std;

My_hypot::My_hypot() : FunctionBase() 
{
	m_funcName = "hypot";
}

// virtual
My_hypot::~My_hypot() 
{
}

// virtual 
TokenBase* My_hypot::doCall() // override
{
    // TODO :       return   hypot(...);    ,  return  sqrt( x^2 + y^2 )
    return nullptr;
}

// virtual 
int My_hypot::getRequiredArgumentsCount() // override;
{
    return 2;
}

