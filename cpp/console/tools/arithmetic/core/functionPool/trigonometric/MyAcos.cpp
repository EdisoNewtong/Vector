#include <cmath>
#include "MyAcos.h"
using namespace std;

My_acos::My_acos() : FunctionBase() 
{
	m_funcName = "acos";
}

// virtual
My_acos::~My_acos() 
{
}

// virtual 
TokenBase* My_acos::doCall() // override
{
    // TODO :       return   acos(...);    acos( 0.5 ) = (pi / 3) = ( 60 degree )
    return nullptr;
}

// virtual 
int My_acos::getRequiredArgumentsCount() // override;
{
    return 1;
}

