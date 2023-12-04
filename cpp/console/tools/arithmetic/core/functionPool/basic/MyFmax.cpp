#include <cmath>
#include "MyFmax.h"
using namespace std;

My_fmax::My_fmax() : FunctionBase()
{
   m_funcName = "fmax"; 
}

// virtual
My_fmax::~My_fmax() 
{
}

// virtual 
TokenBase* My_fmax::doCall() // override
{
    // TODO :       return   abs(...);
    return nullptr;
}

// virtual 
int My_fmax::getRequiredArgumentsCount() // override;
{
    return 2;
}
