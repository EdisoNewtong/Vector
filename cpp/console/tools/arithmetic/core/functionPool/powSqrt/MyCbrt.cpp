#include <cmath>
#include "MyCbrt.h"
using namespace std;

My_cbrt::My_cbrt() : FunctionBase() 
{
	m_funcName = "cbrt";
}

// virtual
My_cbrt::~My_cbrt() 
{
}

// virtual 
TokenBase* My_cbrt::doCall() // override
{
    // TODO :       return   cbrt(...);    ,   cbrt(56) = 4     because   4*4*4 = 56
    return nullptr;
}

// virtual 
int My_cbrt::getRequiredArgumentsCount() // override;
{
    return 1;
}

