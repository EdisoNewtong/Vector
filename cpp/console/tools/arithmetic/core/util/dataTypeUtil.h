#ifndef TYPE_UTIL_H
#define TYPE_UTIL_H


#include <cstdlib>
#include <utility>
#include <unordered_map>
#include "commonEnum.h"


// Varible Data Type    or   fixed literal number
class TypeBaseInfo
{
public:
    TypeBaseInfo(E_DataType dtype);

    int getMemorySize(); // in 'byte' unit
    int getBits();       // in 'bits' unit
    unsigned int getLevel();
    E_DataType getType();

    // util function : float / double 
    bool isFloatFamily();

    // char /short
    bool isSmallIntegerType();  // char /short
    bool isIntegerFamily();     // char ~ long long
    bool isUnsignedType();
protected:
    unsigned int m_level;
    int   m_bytes;
    int   m_bits;
    E_DataType m_type;

};



class DataTypeUtil
{
public:
    static void init();
    static void finalize();
    static TypeBaseInfo* getTypeInfo(E_DataType dtype);
    static std::pair<bool,E_DataType>  needDoIntegerPromotion(E_DataType dtype);
    static std::pair< std::pair<bool,E_DataType>, std::pair<bool,E_DataType> > needDoConvertion(E_DataType leftTp, E_DataType rightTp);

    static bool testCase();
protected:

#ifdef USE_INT_INSTEAD_OF_ENUM
    static std::unordered_map<int, TypeBaseInfo*> s_typeFamily;
#else
    static std::unordered_map<E_DataType, TypeBaseInfo*> s_typeFamily;
#endif
};


#endif 
