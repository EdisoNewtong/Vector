#ifndef VARIBLE_VALUE_H
#define VARIBLE_VALUE_H

#include "commonEnum.h"
#include <string>

struct VaribleData
{
    std::string name; // varible name

    E_DataType dt; // data Type
    union unValue
    {
                 char ch_val;
        unsigned char un_ch_val;
          signed char si_ch_val;

        unsigned short un_short_val;
          signed short si_short_val;

        unsigned int un_int_val;
          signed int si_int_val;

        unsigned long un_long_val;
          signed long si_long_val;

        unsigned long long un_longlong_val;
          signed long long si_longlong_val;

        float float_val;
        double double_val;

        long double longdouble_val;
    } val;
};

#endif
