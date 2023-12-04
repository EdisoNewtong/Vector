#include <cmath>
#include "MyRemainder.h"
using namespace std;

My_remainder::My_remainder() : FunctionBase()
{
	m_funcName = "remainder";
}

// virtual
My_remainder::~My_remainder() 
{
}

// virtual 
TokenBase* My_remainder::doCall() // override
{
    // TODO :       return   abs(...);
    return nullptr;
}

// virtual 
int My_remainder::getRequiredArgumentsCount() // override;
{
    return 2;
}

