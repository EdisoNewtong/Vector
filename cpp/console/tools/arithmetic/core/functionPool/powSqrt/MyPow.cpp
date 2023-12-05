#include <cmath>
#include "MyPow.h"
using namespace std;

My_pow::My_pow() : FunctionBase() 
{
	m_funcName = "pow";
}

// virtual
My_pow::~My_pow() 
{
}

// virtual 
TokenBase* My_pow::doCall() // override
{
    // TODO :       return   pow(...);  => pow(4,3) =>  4*4*4 = 56
    return nullptr;
}

// virtual 
int My_pow::getRequiredArgumentsCount() // override;
{
    return 2;
}

