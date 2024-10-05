#include <limits>
#include <iostream>
#include "enumUtil.h"
#include "myException.h"
#include "dataTypeUtil.h"

#include <QTextStream>

using namespace std;

/*
####################################################################################################

    char        :  1
    short       :  2
    int         :  3
    long        :  4
    long long   :  5

----------------------------------------------------------------------------------------------------

    float       :  6
    double      :  7

####################################################################################################
*/
TypeBaseInfo::TypeBaseInfo(E_DataType dtype)
    : m_level(0)
    , m_bytes(0)
    , m_bits(0)
    , m_type(dtype)
{
    /*
        Besides the minimal bit counts, the C++ Standard guarantees that
        1 == sizeof(char) <= sizeof(short) <= sizeof(int) <= sizeof(long) <= sizeof(long long).
    */

    if (   m_type == E_TP_CHAR   ||     m_type == E_TP_U_CHAR       || m_type == E_TP_S_CHAR )       {
        m_level = 1;
        m_bytes = static_cast<int>( sizeof(char) );
    } else if ( m_type == E_TP_U_SHORT      || m_type == E_TP_S_SHORT )      {
        m_level = 2;
        m_bytes = static_cast<int>( sizeof(short) );
    } else if ( m_type == E_TP_U_INT        || m_type == E_TP_S_INT )        {
        m_level = 3;
        m_bytes = static_cast<int>( sizeof(int) );
    } else if ( m_type == E_TP_U_LONG       || m_type == E_TP_S_LONG )       {
        m_level = 4;
        m_bytes = static_cast<int>( sizeof(long) );
    } else if ( m_type == E_TP_U_LONG_LONG  || m_type == E_TP_S_LONG_LONG )  {
        m_level = 5;
        m_bytes = static_cast<int>( sizeof(long long) );
    } else if ( m_type == E_TP_FLOAT )                                       {
        m_level = 6;
        m_bytes = static_cast<int>( sizeof(float) );
    } else if ( m_type == E_TP_DOUBLE )                                      {
        m_level = 7;
        m_bytes = static_cast<int>( sizeof(double) );
    } 

    m_bits = m_bytes * 8;
}

//
// in 'byte' unit
//
int TypeBaseInfo::getMemorySize()
{
    return m_bytes;
}

//
// in 'bits' unit
//
int TypeBaseInfo::getBits()
{
    return m_bits;
}


unsigned int TypeBaseInfo::getLevel()
{
    return m_level;
}

E_DataType TypeBaseInfo::getType()
{
    return m_type;
}

bool TypeBaseInfo::isUnsignedType()
{
    if ( m_type == E_TP_U_CHAR ) {
        return !(numeric_limits<char>::is_signed);
    } else {
        return isIntegerFamily() && (  m_type == E_TP_U_SHORT
                                    || m_type == E_TP_U_INT
                                    || m_type == E_TP_U_LONG
                                    || m_type == E_TP_U_LONG_LONG );
    }
           
}




bool TypeBaseInfo::isFloatFamily()
{
    return static_cast<int>(m_type) >= static_cast<int>( E_TP_FLOAT );
}

bool TypeBaseInfo::isIntegerFamily()
{
    return     ( static_cast<int>(m_type) >= static_cast<int>( E_TP_CHAR ) )
            && ( static_cast<int>(m_type) <= static_cast<int>( E_TP_S_LONG_LONG ) );
}

bool TypeBaseInfo::isSmallIntegerType()
{
    return      ( static_cast<int>(m_type) >= static_cast<int>( E_TP_CHAR )    )
             && ( static_cast<int>(m_type) <= static_cast<int>( E_TP_S_SHORT ) );
}









//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//


#ifdef USE_INT_INSTEAD_OF_ENUM
    // static 
    std::unordered_map<int, TypeBaseInfo*> DataTypeUtil::s_typeFamily;
#else
    // static 
    std::unordered_map<E_DataType, TypeBaseInfo*> DataTypeUtil::s_typeFamily;
#endif


// static
void DataTypeUtil::init()
{
    s_typeFamily.clear();
    s_typeFamily.insert( make_pair(enumCvt(E_TP_CHAR), new TypeBaseInfo(E_TP_CHAR) ) );
    s_typeFamily.insert( make_pair(enumCvt(E_TP_U_CHAR), new TypeBaseInfo(E_TP_U_CHAR) ) );
    s_typeFamily.insert( make_pair(enumCvt(E_TP_S_CHAR), new TypeBaseInfo(E_TP_S_CHAR) ) );

    s_typeFamily.insert( make_pair(enumCvt(E_TP_U_SHORT), new TypeBaseInfo(E_TP_U_SHORT) ) );
    s_typeFamily.insert( make_pair(enumCvt(E_TP_S_SHORT), new TypeBaseInfo(E_TP_S_SHORT) ) );

    s_typeFamily.insert( make_pair(enumCvt(E_TP_U_INT), new TypeBaseInfo(E_TP_U_INT) ) );
    s_typeFamily.insert( make_pair(enumCvt(E_TP_S_INT), new TypeBaseInfo(E_TP_S_INT) ) );

    s_typeFamily.insert( make_pair(enumCvt(E_TP_U_LONG), new TypeBaseInfo(E_TP_U_LONG) ) );
    s_typeFamily.insert( make_pair(enumCvt(E_TP_S_LONG), new TypeBaseInfo(E_TP_S_LONG) ) );

    s_typeFamily.insert( make_pair(enumCvt(E_TP_U_LONG_LONG), new TypeBaseInfo(E_TP_U_LONG_LONG) ) );
    s_typeFamily.insert( make_pair(enumCvt(E_TP_S_LONG_LONG), new TypeBaseInfo(E_TP_S_LONG_LONG) ) );

    s_typeFamily.insert( make_pair(enumCvt(E_TP_FLOAT), new TypeBaseInfo(E_TP_FLOAT) ) );
    s_typeFamily.insert( make_pair(enumCvt(E_TP_DOUBLE), new TypeBaseInfo(E_TP_DOUBLE) ) );
    // s_typeFamily.insert( make_pair(enumCvt(E_TP_LONG_DOUBLE), new TypeBaseInfo(E_TP_LONG_DOUBLE) ) );
}


// static 
void DataTypeUtil::finalize()
{
    for( auto it = s_typeFamily.begin(); it != s_typeFamily.end(); ++it )
    {
        if ( it->second != nullptr ) {
            delete it->second;
            it->second = nullptr;
        }
    }
    s_typeFamily.clear();
}


// static 
TypeBaseInfo* DataTypeUtil::getTypeInfo(E_DataType dtype)
{

#ifdef USE_INT_INSTEAD_OF_ENUM
    auto it = s_typeFamily.find( enumCvt(dtype) );
#else
    auto it = s_typeFamily.find( dtype );
#endif

    if ( it != s_typeFamily.end() ) {
        return it->second;
    } else {
        return nullptr;
    }
}


/*
Integral promotion :

prvalues of small integral types (such as char) may be converted to prvalues of larger integral types (such as int). In particular, arithmetic operators do not accept types smaller than int as arguments, and integral promotions are automatically applied after lvalue-to-rvalue conversion, if applicable. This conversion always preserves the value.

The following implicit conversions are classified as integral promotions:

1. signed char or signed short can be converted to int;
    //
    // signed char / signed short   ==>   signed int
    //
2. unsigned char, or unsigned short can be converted to int if it can hold its entire value range, and unsigned int otherwise;
    //
    // unsigned char / unsigned short   ==>    signed int   ( if <int> can hold its entire value range )
    //                                  ==>  unsigned int   ( otherwise )
    //

3. char can be converted to int or unsigned int depending on the underlying type: signed char or unsigned char (see above);

*/


// static
std::pair<bool,E_DataType>  DataTypeUtil::needDoIntegerPromotion(E_DataType oldtp)
{
    auto retPair = make_pair(false, oldtp);

    auto pBaseInfo = getTypeInfo(oldtp);
    if ( pBaseInfo == nullptr ) {
        MyException e( E_THROW_NULL_PTR );

        string detail( __FILE__ );
        detail += (string(" @line : ") + to_string(__LINE__));

        e.setDetail(detail);
        throw e;
    }

    // else
    // pBaseInfo != nullptr
    if ( pBaseInfo->isSmallIntegerType()  ) {
        retPair.first = true;

        auto newtp = oldtp;
        if ( oldtp == E_TP_CHAR ) {
            if ( numeric_limits<char>::is_signed ) {
                oldtp = E_TP_S_CHAR; 
            } else {
                oldtp = E_TP_U_CHAR;
            }
        } 

        if ( oldtp == E_TP_S_CHAR || oldtp == E_TP_S_SHORT ) {
            newtp =  E_TP_S_INT; // signed int
        } else {
            // unsigned char /  unsigned short

            // maybe 
            //    sizeof(short) == 2 bytes
            //    sizeof(int)   == 2 bytes (at least) by C++ Standard 
            if ( sizeof(unsigned short) < sizeof(unsigned int) ) {
                newtp = E_TP_S_INT;
            } else {
                newtp = E_TP_U_INT;
            }
        }

        retPair.second = newtp;
    } 

    return retPair;
}


/*

Conversions :

    If the operand passed to an arithmetic operator is integral or unscoped enumeration type, 
then before any other action (but after lvalue-to-rvalue conversion, if applicable), the operand undergoes integral promotion. 
If an operand has array or function type, array-to-pointer and function-to-pointer conversions are applied.

For the binary operators (except shifts), 
    if the promoted operands have different types, additional set of implicit conversions is applied, 
known as usual arithmetic conversions with the goal to produce the common type (also accessible via the std::common_type type trait). 
If, prior to any integral promotion, one operand is of enumeration type and the other operand is of a floating-point type or a different enumeration type, this behavior is deprecated. (since C++20)


If either operand has scoped enumeration type, no conversion is performed: the other operand and the return type must have the same type

    1. Otherwise, if either operand is long double, the other operand is converted to long double
    2. Otherwise, if either operand is double, the other operand is converted to double
    3. Otherwise, if either operand is float, the other operand is converted to float

Otherwise: the operand has integer type (because bool, char, char8_t, char16_t, char32_t, wchar_t, and unscoped enumeration were promoted at this point) 
and integral conversions are applied to produce the common type, as follows:


If both operands are signed or both are unsigned, the operand with lesser conversion rank is converted to the operand with the greater integer conversion rank

    Otherwise, if the unsigned operand's conversion rank is greater or equal to the conversion rank of the signed operand, the signed operand is converted to the unsigned operand's type.
    Otherwise, if the signed operand's type can represent all values of the unsigned operand, the unsigned operand is converted to the signed operand's type
    Otherwise, both operands are converted to the unsigned counterpart of the signed operand's type.

The conversion rank above increases in order bool, signed char, short, int, long, long long. The rank of any unsigned type is equal to the rank of the corresponding signed type. The rank of char is equal to the rank of signed char and unsigned char. The ranks of char8_t, char16_t, char32_t, and wchar_t are equal to the ranks of their underlying types.
*/
// static 
pair< pair<bool,E_DataType>, pair<bool,E_DataType> > DataTypeUtil::needDoConvertion(E_DataType leftTp, E_DataType rightTp)
{

    pair< pair<bool,E_DataType>, pair<bool,E_DataType> > pairret;
    pairret.first.first = false;
    pairret.first.second = leftTp;

    pairret.second.first = false;
    pairret.second.second = rightTp;


    auto pLeftInfo  = DataTypeUtil::getTypeInfo(leftTp);
    auto pRightInfo = DataTypeUtil::getTypeInfo(rightTp);
    if ( pLeftInfo!=nullptr && pRightInfo!=nullptr ) {
        if ( pLeftInfo->isFloatFamily() || pRightInfo->isFloatFamily() ) {
            if ( pLeftInfo->isFloatFamily() &&  pRightInfo->isFloatFamily() ) {
                auto leftLv  = pLeftInfo->getLevel();
                auto rightLv = pRightInfo->getLevel();
                if ( leftLv < rightLv ) {
                    // left part should be convert -> right
                    pairret.first.first = true;
                    pairret.first.second = pRightInfo->getType();
                } else if ( leftLv > rightLv ) {
                    // right part should be convert -> left
                    pairret.second.first = true;
                    pairret.second.second = pLeftInfo->getType();
                }
                // else equal , do nothing
            } else {
                if ( pLeftInfo->isFloatFamily() ) { // left is float family , right is not
                    pairret.second.first = true;
                    pairret.second.second = pLeftInfo->getType();
                } else { // right is float family , left is not
                    pairret.first.first = true;
                    pairret.first.second = pRightInfo->getType();
                }
            }
        } else {
            //
            //  left && right are    both   interger type
            //
            auto leftLv  = pLeftInfo->getLevel();
            auto rightLv = pRightInfo->getLevel();

            if ( pLeftInfo->isUnsignedType() && pRightInfo->isUnsignedType() ) {
                // both are unsigned
                if ( leftLv < rightLv ) {
                    pairret.first.first = true;
                    pairret.first.second = pRightInfo->getType();
                } else if ( leftLv > rightLv ) {
                    pairret.second.first = true;
                    pairret.second.second = pLeftInfo->getType();
                } // else equal , do nothing
            } else if ( !(pLeftInfo->isUnsignedType()) && !(pRightInfo->isUnsignedType()) ) {
                // both are signed
                if ( leftLv < rightLv ) {
                    pairret.first.first = true;
                    pairret.first.second = pRightInfo->getType();
                } else if ( leftLv > rightLv ) {
                    pairret.second.first = true;
                    pairret.second.second = pLeftInfo->getType();
                } // else equal , do nothing
            } else {
                //
                // one is unsigned and the other is signed
                //

                if ( pLeftInfo->isUnsignedType() ) {
                    // left is unsigned  &&  right is signed
                    if ( leftLv >= rightLv ) {
                        pairret.second.first = true;
                        pairret.second.second = pLeftInfo->getType();
                    } else {
                        if ( pRightInfo->getMemorySize() > pLeftInfo->getMemorySize() ) {
                            pairret.first.first = true;
                            pairret.first.second = pRightInfo->getType();
                        } else {
                            // both convert to unsigned type
                            pairret.second.first = true;
                            pairret.second.second = pLeftInfo->getType();
                        }
                    }
                } else {
                    // left is signed  &&  right is unsigned
                    if ( rightLv >= leftLv  ) {
                        pairret.first.first = true;
                        pairret.first.second = pRightInfo->getType();
                    } else {
                        if ( pLeftInfo->getMemorySize() > pRightInfo->getMemorySize() ) {
                            pairret.second.first = true;
                            pairret.second.second = pLeftInfo->getType();
                        } else {
                            // both convert to unsigned type
                            pairret.first.first = true;
                            pairret.first.second = pRightInfo->getType();
                        }
                    }
                }
            }

        }
    } else {
        // left or right is nullptr
    }

    return pairret;
}



// static 
bool DataTypeUtil::testCase(QString& output)
{
    QTextStream ts(&output);

    auto bret = true;
    for( auto i = static_cast<int>(E_TP_CHAR); i <= static_cast<int>(E_TP_DOUBLE); ++i )
    {
        for( auto j = static_cast<int>(E_TP_CHAR); j <= static_cast<int>(E_TP_DOUBLE); ++j )
        {

            auto typeLeft = static_cast<E_DataType>(i);
            auto typeRight = static_cast<E_DataType>(j);
            auto copyLeft = typeLeft;
            auto copyRight = typeRight;

            // auto bLeft = false;
            // auto bRight = false;
            auto proLeftPair = DataTypeUtil::needDoIntegerPromotion(typeLeft);
            if ( proLeftPair.first ) {
                typeLeft = proLeftPair.second;
            }

            auto proRightPair = DataTypeUtil::needDoIntegerPromotion(typeRight);
            if ( proRightPair.first  ) {
                typeRight = proRightPair.second;
            }

            auto prpr = DataTypeUtil::needDoConvertion(typeLeft, typeRight);
            if (         prpr.first.second == prpr.second.second 
                    &&   (   (!prpr.first.first   &&  !prpr.second.first)
                         ||  ( prpr.first.first   &&  !prpr.second.first)
                         ||  ( !prpr.first.first  &&   prpr.second.first) )
            ) {
                ts << "<" << EnumUtil::enumName(copyLeft).c_str() << "," << EnumUtil::enumName(copyRight).c_str() << ">   => "
                     << EnumUtil::enumName(prpr.first.second).c_str()
                     << "  ( " << EnumUtil::enumName(copyLeft).c_str()  << " -> " << (copyLeft !=prpr.first.second  ? EnumUtil::enumName(prpr.first.second).c_str()  : "keep") << " ) "
                     << "  ( " << EnumUtil::enumName(copyRight).c_str() << " -> " << (copyRight!=prpr.second.second ? EnumUtil::enumName(prpr.second.second).c_str() : "keep") << " ) "
                     << endl;

            } else {
                bret = false;
            }
        }

        if ( !bret ) {
            break;
        }
    }

    return bret;
}





//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
