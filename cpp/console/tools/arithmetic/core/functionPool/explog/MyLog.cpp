#include <cmath>
#include "MyLog.h"
using namespace std;

My_log::My_log() : FunctionBase() 
{
	m_funcName = "log";
}

// virtual
My_log::~My_log() 
{
}

// virtual 
TokenBase* My_log::doCall() // override
{
    // TODO :       return   log(...);   =>  / ln(x)  /   log(e) = 1    /  log(e*e) = 2 /  log( e*e*e ) = 3
    return nullptr;
}

// virtual 
int My_log::getRequiredArgumentsCount() // override;
{
    return 1;
}

