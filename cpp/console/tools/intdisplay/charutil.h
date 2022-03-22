#ifndef CHAR_UTIL_H
#define CHAR_UTIL_H

namespace charutil
{
    bool isNegative(const char& ch);
    bool isNumber(const char& ch);
    bool isOctNumber(const char& ch);
    bool isHexNumber(const char& ch);
    bool isBinaryNumber(const char& ch);
    bool isxX(const char& ch);
    bool isbB(const char& ch);
}


#endif
