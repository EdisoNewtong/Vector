#ifndef MY_TAN_H
#define MY_TAN_H

#include "functionBase.h"

class My_tan : public FunctionBase
{
public:
    My_tan();
    virtual ~My_tan();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
