#ifndef MY_REMAINDER_H
#define MY_REMAINDER_H

#include "functionBase.h"

class My_remainder : public FunctionBase
{
public:
    My_remainder();
    virtual ~My_remainder();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
