#ifndef MY_HYPOT_H
#define MY_HYPOT_H

#include "functionBase.h"

class My_hypot : public FunctionBase
{
public:
    My_hypot();
    virtual ~My_hypot();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
