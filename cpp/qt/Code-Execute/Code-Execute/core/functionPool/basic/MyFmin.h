#ifndef MY_FMIN_H
#define MY_FMIN_H

#include "functionBase.h"

class My_fmin : public FunctionBase
{
public:
    My_fmin();
    virtual ~My_fmin();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
