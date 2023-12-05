#include <cmath>
#include "MyAtan.h"
using namespace std;

My_atan::My_atan() : FunctionBase() 
{
	m_funcName = "atan";
}

// virtual
My_atan::~My_atan() 
{
}

// virtual 
TokenBase* My_atan::doCall() // override
{
    // TODO :       return   atan(...);  =>   atan(1) = pi/4 = ( 45 degree )
    return nullptr;
}

// virtual 
int My_atan::getRequiredArgumentsCount() // override;
{
    return 1;
}

