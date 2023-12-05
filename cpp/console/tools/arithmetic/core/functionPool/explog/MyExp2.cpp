#include <cmath>
#include "MyExp2.h"
using namespace std;

My_exp2::My_exp2() : FunctionBase() 
{
	m_funcName = "exp2";
}

// virtual
My_exp2::~My_exp2() 
{
}

// virtual 
TokenBase* My_exp2::doCall() // override
{
    // TODO :       return   exp2(...);   =>   exp2( 5 ) = 2*2*2*2*2 = 32;
    return nullptr;
}

// virtual 
int My_exp2::getRequiredArgumentsCount() // override;
{
    return 1;
}

