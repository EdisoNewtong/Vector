#ifndef MY_LOG_H
#define MY_LOG_H

#include "functionBase.h"

class My_log : public FunctionBase
{
public:
    My_log();
    virtual ~My_log();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif