#ifndef ENUM_UTIL_H
#define ENUM_UTIL_H

#include "commonEnum.h"
#include "parserBase.h"
#include <string>

class EnumUtil
{
public:
    // overload function with different enum type
    static std::string enumName(E_CharType tp);
    static std::string enumName(E_OperatorType tp);
    static std::string enumName(E_TokenType tp);
    static std::string enumName(E_DataType tp);
    static std::string enumName(E_ParserAction tp);
    static std::string enumName(E_ExceptionType tp);
    static std::string enumName(ParserBase::E_PARSER_TYPE tp);

    static E_OperatorType getOpType(const char& firstCh, bool isUnary = false);

private:
    EnumUtil() = delete;
    EnumUtil(const EnumUtil& clone) = delete;
    virtual ~EnumUtil() = delete;
    EnumUtil& operator = (const EnumUtil& right) = delete;
};

#endif

