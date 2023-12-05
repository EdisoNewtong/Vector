#include <cmath>
#include "MyCos.h"
using namespace std;

My_cos::My_cos() : FunctionBase() 
{
	m_funcName = "cos";
}

// virtual
My_cos::~My_cos() 
{
}

// virtual 
TokenBase* My_cos::doCall() // override
{
    // TODO :       return   cos(...);  =>  cos( pi/3 ) = cos( 60 degree ) = 0.5
    return nullptr;
}

// virtual 
int My_cos::getRequiredArgumentsCount() // override;
{
    return 1;
}

