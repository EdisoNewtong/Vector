#ifndef MY_TRUNC_H
#define MY_TRUNC_H

#include "functionBase.h"

class My_trunc : public FunctionBase
{
public:
    My_trunc();
    virtual ~My_trunc();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
