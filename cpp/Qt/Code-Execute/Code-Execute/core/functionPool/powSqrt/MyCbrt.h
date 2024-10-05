#ifndef MY_CBRT_H
#define MY_CBRT_H

#include "functionBase.h"

class My_cbrt : public FunctionBase
{
public:
    My_cbrt();
    virtual ~My_cbrt();

protected:
    virtual TokenBase* doCall() override;
    virtual int getRequiredArgumentsCount() override;
};

#endif
