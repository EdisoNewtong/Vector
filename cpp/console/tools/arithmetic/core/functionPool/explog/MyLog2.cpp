#include <cmath>
#include "MyLog2.h"
using namespace std;

My_log2::My_log2() : FunctionBase() 
{
	m_funcName = "log2";
}

// virtual
My_log2::~My_log2() 
{
}

// virtual 
TokenBase* My_log2::doCall() // override
{
    // TODO :       return   log2(...);  =>   log(2) = 1 / log(2*2) = 2;   log(2*2*2) = 3;
    return nullptr;
}

// virtual 
int My_log2::getRequiredArgumentsCount() // override;
{
    return 1;
}

