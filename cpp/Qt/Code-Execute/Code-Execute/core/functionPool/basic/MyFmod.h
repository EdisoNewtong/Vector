#ifndef MY_FMOD_H
#define MY_FMOD_H

#include "functionBase.h"

class My_fmod : public FunctionBase
{
public:
    My_fmod();
    virtual ~My_fmod();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
