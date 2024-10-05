#ifndef MY_ROUND_H
#define MY_ROUND_H

#include "functionBase.h"

class My_round : public FunctionBase
{
public:
    My_round();
    virtual ~My_round();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
