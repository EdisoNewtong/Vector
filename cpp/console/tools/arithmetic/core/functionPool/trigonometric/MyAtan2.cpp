#include <cmath>
#include "MyAtan2.h"
using namespace std;

My_atan2::My_atan2() : FunctionBase() 
{
	m_funcName = "atan2";
}

// virtual
My_atan2::~My_atan2() 
{
}

// virtual 
TokenBase* My_atan2::doCall() // override
{
    // TODO :       return   atan2(...);  => atan2(x,y)   =>   arctan( y/x )
    return nullptr;
}

// virtual 
int My_atan2::getRequiredArgumentsCount() // override;
{
    return 2;
}

