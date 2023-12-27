#ifndef MY_SQRT_H
#define MY_SQRT_H

#include "functionBase.h"

class My_sqrt : public FunctionBase
{
public:
    My_sqrt();
    virtual ~My_sqrt();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
