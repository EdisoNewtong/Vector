#include <cmath>
#include "MyAsin.h"
using namespace std;

My_asin::My_asin() : FunctionBase() 
{
	m_funcName = "asin";
}

// virtual
My_asin::~My_asin() 
{
}

// virtual 
TokenBase* My_asin::doCall() // override
{
    // TODO :       return   asin(...);    asin( 0.5 ) = pi/6 = ( 30 degree )
    return nullptr;
}

// virtual 
int My_asin::getRequiredArgumentsCount() // override;
{
    return 1;
}

