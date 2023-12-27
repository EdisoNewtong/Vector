#ifndef MY_CEIL_H
#define MY_CEIL_H

#include "functionBase.h"

class My_ceil : public FunctionBase
{
public:
    My_ceil();
    virtual ~My_ceil();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif