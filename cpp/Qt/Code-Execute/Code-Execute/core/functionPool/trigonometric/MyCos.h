#ifndef MY_COS_H
#define MY_COS_H

#include "functionBase.h"

class My_cos : public FunctionBase
{
public:
    My_cos();
    virtual ~My_cos();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
