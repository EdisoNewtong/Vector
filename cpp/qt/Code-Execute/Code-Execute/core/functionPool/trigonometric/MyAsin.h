#ifndef MY_ASIN_H
#define MY_ASIN_H

#include "functionBase.h"

class My_asin : public FunctionBase
{
public:
    My_asin();
    virtual ~My_asin();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
