#ifndef MY_FMAX_H
#define MY_FMAX_H

#include "functionBase.h"

class My_fmax : public FunctionBase
{
public:
    My_fmax();
    virtual ~My_fmax();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif