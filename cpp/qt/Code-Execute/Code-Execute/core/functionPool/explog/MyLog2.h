#ifndef MY_LOG2_H
#define MY_LOG2_H

#include "functionBase.h"

class My_log2 : public FunctionBase
{
public:
    My_log2();
    virtual ~My_log2();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
