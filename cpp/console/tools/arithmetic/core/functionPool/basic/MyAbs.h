#ifndef MY_ABS_H
#define MY_ABS_H

#include "functionBase.h"

class My_abs : public FunctionBase
{
public:
    My_abs();
    virtual ~My_abs();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif