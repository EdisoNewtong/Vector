#ifndef MY_EXP2_H
#define MY_EXP2_H

#include "functionBase.h"

class My_exp2 : public FunctionBase
{
public:
    My_exp2();
    virtual ~My_exp2();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif