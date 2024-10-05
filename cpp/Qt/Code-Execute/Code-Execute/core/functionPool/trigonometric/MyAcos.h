#ifndef MY_ACOS_H
#define MY_ACOS_H

#include "functionBase.h"

class My_acos : public FunctionBase
{
public:
    My_acos();
    virtual ~My_acos();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
