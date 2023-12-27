#ifndef MY_POW_H
#define MY_POW_H

#include "functionBase.h"

class My_pow : public FunctionBase
{
public:
    My_pow();
    virtual ~My_pow();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
