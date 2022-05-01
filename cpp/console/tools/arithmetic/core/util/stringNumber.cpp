#include <cctype>

#include "stringNumber.h"
#include "myException.h"

/*

----------------------------------------------------------------------------------------------------

   2 bytes    : unsigned     0177777  , 65535 , 0xFFFF
                  signed      077777  , 32767 , 0x7FFF

----------------------------------------------------------------------------------------------------

   4 bytes    : unsigned     037777777777  , 4294967295 , 0xFFFFFFFF 
                  signed     017777777777  , 2147483647 , 0x7FFFFFFF
   
----------------------------------------------------------------------------------------------------

   8 bytes    : unsigned     01777777777777777777777   , 18446744073709551615 , 0xFFFFFFFFFFFFFFFF
                  signed     0777777777777777777777    ,  9223372036854775807 , 0x7FFFFFFFFFFFFFFF 

----------------------------------------------------------------------------------------------------

*/

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// static Member Data init
//
////////////////////////////////////////////////////////////////////////////////////////////////////
// unsigned int  ,                                             for C++ Standard ,          <int> type at least 2 bytes (16 bits)
const StringNumber StringNumber::s_unsignedIntMax8(  (sizeof(int) == 4 ? string("037777777777") : string("0177777")), 8);
const StringNumber StringNumber::s_unsignedIntMax10( (sizeof(int) == 4 ? string("4294967295")   : string("65535")),  10);
const StringNumber StringNumber::s_unsignedIntMax16( (sizeof(int) == 4 ? string("0xFFFFFFFF")   : string("0xFFFF")), 16);

// signed int
const StringNumber StringNumber::s_signedIntMax8(  (sizeof(int) == 4 ? string("017777777777") : string("077777")),  8);
const StringNumber StringNumber::s_signedIntMax10( (sizeof(int) == 4 ? string("2147483647")   : string("32767")),  10);
const StringNumber StringNumber::s_signedIntMax16( (sizeof(int) == 4 ? string("0x7FFFFFFF")   : string("0x7FFF")), 16);

////////////////////////////////////////////////////////////////////////////////////////////////////
// unsigned long
const StringNumber StringNumber::s_unsignedLongMax8(  (sizeof(long) == 4 ? string("037777777777") : string("01777777777777777777777")), 8);
const StringNumber StringNumber::s_unsignedLongMax10( (sizeof(long) == 4 ? string("4294967295")   : string("18446744073709551615")),   10);
const StringNumber StringNumber::s_unsignedLongMax16( (sizeof(long) == 4 ? string("0xFFFFFFFF")   : string("0xFFFFFFFFFFFFFFFF")),     16);

// signed long
const StringNumber StringNumber::s_signedLongMax8(  (sizeof(long) == 4 ? string("017777777777") : string("0777777777777777777777")), 8);
const StringNumber StringNumber::s_signedLongMax10( (sizeof(long) == 4 ? string("2147483647")   : string("9223372036854775807")),   10);
const StringNumber StringNumber::s_signedLongMax16( (sizeof(long) == 4 ? string("0x7FFFFFFF")   : string("0x7FFFFFFFFFFFFFFF")),    16);

////////////////////////////////////////////////////////////////////////////////////////////////////
// unsigned long long
const StringNumber StringNumber::s_unsignedLongLongMax8(  string("01777777777777777777777"),   8);
const StringNumber StringNumber::s_unsignedLongLongMax10( string("18446744073709551615"),     10);
const StringNumber StringNumber::s_unsignedLongLongMax16( string("0xFFFFFFFFFFFFFFFF"),       16);

// signed long long
const StringNumber StringNumber::s_signedLongLongMax8(  string("0777777777777777777777"),      8);
const StringNumber StringNumber::s_signedLongLongMax10( string("9223372036854775807"),        10);
const StringNumber StringNumber::s_signedLongLongMax16( string("0x7FFFFFFFFFFFFFFF"),         16);
//
////////////////////////////////////////////////////////////////////////////////////////////////////





StringNumber::StringNumber(const string& strnumber, int base)
    : m_strNumber(strnumber)
    , m_stripedNumber("")
    , m_base(base)
{
    setData(strnumber,base);
}


void StringNumber::setData(const string& strnumber, int base)
{
    m_stripedNumber = "";
    m_strNumber = strnumber;
    m_base = base;

    if ( m_base == 10 || m_base == 8 || m_base == 16 ) {
        if ( m_base == 10 ) {
            processBase10();
        } else if ( m_base == 8 ) {
            processBase8();
        } else {
            processBase16();
        }
    } else {
        MyException e(E_THROW_INVALID_NUMBER_BASE);
        e.setDetail( string(" m_base = ") +  to_string(m_base) );
        throw e;
    }
}




bool StringNumber::operator < (const StringNumber& right)
{
    if ( this->m_base != right.m_base ) {
        MyException e( E_THROW_DIFFERENT_NUMBER_BASE );
        e.setDetail( to_string(this->m_base) + " <--> " + to_string(right.m_base) );
        throw e;
    }

    int leftSz  = static_cast<int>( this->m_stripedNumber.size() );
    int rightSz = static_cast<int>( right.m_stripedNumber.size() );
    if ( leftSz < rightSz ) {
        // leftNumber < rightNumber   :  => true
        return true;
    } else if ( leftSz > rightSz ) {
        // leftNumber > rightNumber   :  => false
        return false;
    } else {
        // leftSz == rightSz
        auto decideRet = false; // == means   false
        for( int idx = 0; idx < leftSz; ++idx)
        {
            char lft = m_stripedNumber.at(idx);
            char rgt = right.m_stripedNumber.at(idx);
            lft = static_cast<char>( toupper( static_cast<int>(lft) ) );
            rgt = static_cast<char>( toupper( static_cast<int>(rgt) ) );
            if ( lft != rgt ) {
                if ( lft < rgt ) {
                    decideRet = true;
                    break;
                } else {
                    // lft > rgt
                    decideRet = false;
                    break;
                }
            } 
            // else  continue;
        }

        return decideRet;
    }

}

bool StringNumber::operator == (const StringNumber& right)
{
    if ( this->m_base != right.m_base ) {
        MyException e( E_THROW_DIFFERENT_NUMBER_BASE );
        e.setDetail( to_string(this->m_base) + " <--> " + to_string(right.m_base) );
        throw e;
    }

    int leftSz  = static_cast<int>( this->m_stripedNumber.size() );
    int rightSz = static_cast<int>( right.m_stripedNumber.size() );
    if ( leftSz == rightSz ) {
        auto isEqual = true;
        for( int idx = 0; idx < leftSz; ++idx)
        {
            char lft = m_stripedNumber.at(idx);
            char rgt = right.m_stripedNumber.at(idx);
            if ( lft != rgt ) {
                isEqual = false;
                break;
            }
        }

        return isEqual;
    } else {
        return false;
    }

}


bool StringNumber::operator <=  (const StringNumber& right)
{
    return       ( this->operator < ( right) )
             ||  ( this->operator == ( right) );
}


bool StringNumber::operator > (const StringNumber& right)
{
    return !( this->operator < ( right) );
    // return !(this <  right);
}



void StringNumber::processBase8()
{
    string copyStr(m_strNumber);
    if ( copyStr.empty() ) {
        MyException e(E_THROW_INVALID_OCTAL_NUMBER);
        e.setDetail(" empty octal number " );
        throw e;
    }

    char firstCh = copyStr.front();
    if ( firstCh != '0' ) {
        MyException e(E_THROW_INVALID_OCTAL_NUMBER);
        e.setDetail(" Octal number is not start with '0' " );
        throw e;
    }

    copyStr = copyStr.substr(1);
    if ( copyStr.empty() ) {
        MyException e(E_THROW_INVALID_OCTAL_NUMBER);
        e.setDetail(" empty octal number after cuting the beginning '0' " );
        throw e;
    }

    auto isAllZero = true;
    int len = static_cast<int>( copyStr.size() );
    int noneZeroIdx = 0;
    for( noneZeroIdx = 0; noneZeroIdx < len; ++noneZeroIdx )
    {
        char ch = copyStr.at(noneZeroIdx);
        if ( ch != '0' ) {
            isAllZero = false;
            break;
        }
    }

    if ( isAllZero ) {
        m_stripedNumber = "0";
    } else {
        m_stripedNumber = copyStr.substr(noneZeroIdx);
    }
}

void StringNumber::processBase10()
{
    string copyStr(m_strNumber);
    if ( copyStr.empty() ) {
        MyException e(E_THROW_INVALID_DECIMAL_NUMBER);
        e.setDetail(" empty decimal number " );
        throw e;
    }

    auto isAllZero = true;
    int len = static_cast<int>( copyStr.size() );
    int noneZeroIdx = 0;
    for( noneZeroIdx = 0; noneZeroIdx < len; ++noneZeroIdx )
    {
        char ch = copyStr.at(noneZeroIdx);
        if ( ch != '0' ) {
            isAllZero = false;
            break;
        }
    }

    if ( isAllZero ) {
        m_stripedNumber = "0";
    } else {
        m_stripedNumber = copyStr.substr(noneZeroIdx);
    }

}

void StringNumber::processBase16()
{
    string copyStr(m_strNumber);
    int len = static_cast<int>( copyStr.size() );
    if ( len <=2 ) {
        MyException e(E_THROW_INVALID_HEX_NUMBER);
        e.setDetail(" invalid hex number , len <=2 " );
        throw e;
    }

    string prefix = copyStr.substr(0,2);
    if ( prefix != "0x" &&    prefix != "0X" ) {
        MyException e(E_THROW_INVALID_HEX_NUMBER);
        e.setDetail(" Hex number is not start with \"0x\" " );
        throw e;
    }

    copyStr = copyStr.substr(2);
    if ( copyStr.empty() ) {
        MyException e(E_THROW_INVALID_HEX_NUMBER);
        e.setDetail(" empty Hex number after cuting the beginning \"0x\" " );
        throw e;
    }


    auto isAllZero = true;
    len = static_cast<int>( copyStr.size() );
    int noneZeroIdx = 0;
    for( noneZeroIdx = 0; noneZeroIdx < len; ++noneZeroIdx )
    {
        char ch = copyStr.at(noneZeroIdx);
        if ( ch != '0' ) {
            isAllZero = false;
            break;
        }
    }

    if ( isAllZero ) {
        m_stripedNumber = "0";
    } else {
        m_stripedNumber = copyStr.substr(noneZeroIdx);
    }
}


string StringNumber::getStrNumber() const
{
    return m_strNumber;
}


