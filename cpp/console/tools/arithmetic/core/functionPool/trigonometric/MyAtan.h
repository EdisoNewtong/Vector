#ifndef MY_ATAN_H
#define MY_ATAN_H

#include "functionBase.h"

class My_atan : public FunctionBase
{
public:
    My_atan();
    virtual ~My_atan();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
