#include <cmath>
#include "MyLog10.h"
using namespace std;

My_log10::My_log10() : FunctionBase() 
{
	m_funcName = "log10";
}

// virtual
My_log10::~My_log10() 
{
}

// virtual 
TokenBase* My_log10::doCall() // override
{
    // TODO :       return   log10(...);   =>   log(10) = 1 /  log(10*10) = 2 /  log(10*10*10) = 3
    return nullptr;
}

// virtual 
int My_log10::getRequiredArgumentsCount() // override;
{
    return 1;
}

