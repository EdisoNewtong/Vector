#ifndef MY_FLOOR_H
#define MY_FLOOR_H

#include "functionBase.h"

class My_floor : public FunctionBase
{
public:
    My_floor();
    virtual ~My_floor();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif