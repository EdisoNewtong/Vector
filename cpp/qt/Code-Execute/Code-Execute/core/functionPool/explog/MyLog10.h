#ifndef MY_LOG10_H
#define MY_LOG10_H

#include "functionBase.h"

class My_log10 : public FunctionBase
{
public:
    My_log10();
    virtual ~My_log10();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
