#ifndef MY_EXP_H
#define MY_EXP_H

#include "functionBase.h"

class My_exp : public FunctionBase
{
public:
    My_exp();
    virtual ~My_exp();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
