#ifndef MY_ATAN2_H
#define MY_ATAN2_H

#include "functionBase.h"

class My_atan2 : public FunctionBase
{
public:
    My_atan2();
    virtual ~My_atan2();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
