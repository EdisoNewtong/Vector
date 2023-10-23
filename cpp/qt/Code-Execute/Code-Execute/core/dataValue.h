#ifndef DATA_VALUE_H
#define DATA_VALUE_H

#include "commonEnum.h"
#include <string>


namespace intTypeRange
{
    // char
    extern const          char ch_min;
    extern const          char ch_max;
    extern const unsigned char uch_min;
    extern const unsigned char uch_max;
    extern const   signed char sch_min; 
    extern const   signed char sch_max;
    // short
    extern const   unsigned short ushort_min;
    extern const   unsigned short ushort_max;
    extern const     signed short sshort_min;
    extern const     signed short sshort_max;
    // int
    extern const   unsigned int uint_min;
    extern const   unsigned int uint_max;
    extern const     signed int sint_min;
    extern const     signed int sint_max;
    // long
    extern const   unsigned long ulong_min;
    extern const   unsigned long ulong_max;
    extern const     signed long slong_min;
    extern const     signed long slong_max;
    // long long
    extern const   unsigned long long ulonglong_min; 
    extern const   unsigned long long ulonglong_max; 
    extern const     signed long long slonglong_min;
    extern const     signed long long slonglong_max;
}

struct DataValue
{
    E_DataType type;
    union {
        // char 
                 char       char_val;
          signed char       schar_val;
        unsigned char       uchar_val;
        // short 
          signed short      sshort_val;
        unsigned short      ushort_val;
        // int 
          signed int        sint_val;
        unsigned int        uint_val;
        // long 
          signed long       slong_val;
        unsigned long       ulong_val;
        // long long
          signed long long  slonglong_val;
        unsigned long long  ulonglong_val;
        // float / double 
        float               float_val;
        double              double_val;
    } value;

    void doIntergerPromotion(E_DataType promotionTp);
    void doConvertion(E_DataType destinationTp);
    bool isIntZero() const;
    bool isFloatZero() const;
    bool isNegative();
    bool isGreaterEqualBitsWidth(int bits);
    bool isMinimumNegativeNumber();
    bool isIntOutOfRange(E_DataType dt, std::string& strMinVal, std::string& strMaxVal);
    std::string getPrintValue(unsigned int flag, bool hasPreviousWithEqual = false);

    void randomInternalValue();

    void downerCast(E_DataType castTp);

    DataValue operator + (); // positive
    DataValue operator - (); // negative
    DataValue operator ~ (); // bit not

    DataValue operator + ( const DataValue& right);
    DataValue operator - ( const DataValue& right);
    DataValue operator * ( const DataValue& right);
    DataValue operator / ( const DataValue& right);
    DataValue operator % ( const DataValue& right);

    DataValue operator & ( const DataValue& right);
    DataValue operator | ( const DataValue& right);
    DataValue operator ^ ( const DataValue& right);

    DataValue operator << ( const DataValue& right);
    DataValue operator >> ( const DataValue& right);

    void doAssignment( const DataValue& right);


    //
    static std::string toBinary(const std::string& hex);

};






#endif
