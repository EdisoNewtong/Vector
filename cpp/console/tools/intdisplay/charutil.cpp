#include "charutil.h"


namespace charutil
{

    bool isNegative(const char& ch)
    {
        return ch == '-';
    }


    bool isNumber(const char& ch)
    {
        return ch >= '0' && ch <= '9';
    }

    bool isOctNumber(const char& ch)
    {
        return ch >= '0' && ch <= '7';
    }

    bool isHexNumber(const char& ch)
    {
        return    (ch >='0'  && ch <='9')
               || (ch >='a'  && ch <='f')
               || (ch >='A'  && ch <='F');
    }

    bool isBinaryNumber(const char& ch)
    {
        return ch =='0' ||  ch =='1';
    }

    bool isxX(const char& ch)
    {
        return ch == 'x' || ch == 'X';
    }

    bool isbB(const char& ch)
    {
        return ch == 'b' || ch == 'B';
    }

}

