#include <cmath>
#include "MyFmod.h"
using namespace std;

My_fmod::My_fmod() : FunctionBase()
{
   m_funcName = "fmod";
}

// virtual
My_fmod::~My_fmod() 
{
}

// virtual 
TokenBase* My_fmod::doCall() // override
{
    // TODO :       return   fmod( ... );
    return nullptr;
}

// virtual 
int My_fmod::getRequiredArgumentsCount() // override;
{
    return 2;
}

