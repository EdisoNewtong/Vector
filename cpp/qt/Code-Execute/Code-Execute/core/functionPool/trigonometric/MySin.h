#ifndef MY_SIN_H
#define MY_SIN_H

#include "functionBase.h"

class My_sin : public FunctionBase
{
public:
    My_sin();
    virtual ~My_sin();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
