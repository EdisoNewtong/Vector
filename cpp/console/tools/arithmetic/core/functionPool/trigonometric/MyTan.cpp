#include <cmath>
#include "MyTan.h"
using namespace std;

My_tan::My_tan() : FunctionBase() 
{
	m_funcName = "tan";
}

// virtual
My_tan::~My_tan() 
{
}

// virtual 
TokenBase* My_tan::doCall() // override
{
    // TODO :       return   tan(...);     tan( pi/4 ) = tan ( 45 degree ) = 1
    return nullptr;
}

// virtual 
int My_tan::getRequiredArgumentsCount() // override;
{
    return 1;
}

