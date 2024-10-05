#include <cctype>
#include <bitset>

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






const unordered_map<char, deque<int> > StringNumber::s_OctMap{
	 make_pair('0', deque<int>{ 0, 0, 0 }),
	 make_pair('1', deque<int>{ 0, 0, 1 }),
	 make_pair('2', deque<int>{ 0, 1, 0 }),
	 make_pair('3', deque<int>{ 0, 1, 1 }),
	 make_pair('4', deque<int>{ 1, 0, 0 }),
	 make_pair('5', deque<int>{ 1, 0, 1 }),
	 make_pair('6', deque<int>{ 1, 1, 0 }),
	 make_pair('7', deque<int>{ 1, 1, 1 })
};

const unordered_map<char, deque<int> > StringNumber::s_HexMap{
	make_pair('0', deque<int>{ 0, 0, 0, 0 }),
	make_pair('1', deque<int>{ 0, 0, 0, 1 }),
	make_pair('2', deque<int>{ 0, 0, 1, 0 }),
	make_pair('3', deque<int>{ 0, 0, 1, 1 }),
	make_pair('4', deque<int>{ 0, 1, 0, 0 }),
	make_pair('5', deque<int>{ 0, 1, 0, 1 }),
	make_pair('6', deque<int>{ 0, 1, 1, 0 }),
	make_pair('7', deque<int>{ 0, 1, 1, 1 }),
	make_pair('8', deque<int>{ 1, 0, 0, 0 }),
	make_pair('9', deque<int>{ 1, 0, 0, 1 }),
	make_pair('A', deque<int>{ 1, 0, 1, 0 }),
	make_pair('B', deque<int>{ 1, 0, 1, 1 }),
	make_pair('C', deque<int>{ 1, 1, 0, 0 }),
	make_pair('D', deque<int>{ 1, 1, 0, 1 }),
	make_pair('E', deque<int>{ 1, 1, 1, 0 }),
	make_pair('F', deque<int>{ 1, 1, 1, 1 })
};



//
////////////////////////////////////////////////////////////////////////////////////////////////////





StringNumber::StringNumber(const string& strnumber, int base)
    : m_strNumber(strnumber)
    , m_stripedNumber("")
    , m_base(base)
	, m_binaryDigitList()
{
    setData(strnumber,base);
}


void StringNumber::setData(const string& strnumber, int base)
{
    m_stripedNumber = "";
    m_strNumber = strnumber;
    m_base = base;
	m_binaryDigitList.clear();

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



size_t StringNumber::oct2Bin()
{
	m_binaryDigitList.clear();

    string copyStr(m_strNumber);
    if ( copyStr.empty()  ) {
        MyException e(E_THROW_INVALID_OCTAL_NUMBER);
        e.setDetail(" empty octal number " );
        throw e;
    }

    if ( copyStr[0] != '0' ) {
        MyException e(E_THROW_INVALID_OCTAL_NUMBER);
        e.setDetail(" octal number's prefix is not \'0\' " );
        throw e;
    }

	copyStr = copyStr.substr(1);
    if ( copyStr.empty()  ) {
        MyException e(E_THROW_INVALID_OCTAL_NUMBER);
        e.setDetail(" after cut prefix '0', empty octal number " );
        throw e;
    }

	size_t len  = copyStr.size();
	for( size_t i = 0; i < len; ++i ) {
		char ch = copyStr.at(i);
		auto it = s_OctMap.find(ch);
		if ( it == s_OctMap.end() ) {
			MyException e(E_THROW_INVALID_OCTAL_NUMBER);
			e.setDetail( string("Invalid ch : \'") + string(1, ch) + string("\' inside Octal Number. ") );
			throw e;
		}

		for( const auto& zeroOrOne : it->second ) {
			m_binaryDigitList.push_back(zeroOrOne);
		}
	}

	return m_binaryDigitList.size();
}

size_t StringNumber::dec2Bin()
{
	m_binaryDigitList.clear();

	// Dividend 被除数
	// Divisor    除数
	// Quotient  除法的 商数
	// Remainder 除法的 余数
	string strQuotient;
	                     
	unsigned int previousRemainder = 0;

    string strDecimalDivident(m_strNumber);
	size_t len = 0;
	do {
		len  = strDecimalDivident.size();
		strQuotient.resize(len, '0');
		previousRemainder = 0;

		bool hasSkipFlag = false;
		for( size_t i = 0; i < len; ++i ) {
			unsigned int decValue = static_cast<unsigned int>( strDecimalDivident.at(i) - '0' );
			unsigned int dividend = previousRemainder * 10 + decValue;
			unsigned int iQuotient = (dividend >> 1);   // =>      iQuotient = dividend / 2
			if ( i == 0 && len >=2 && iQuotient == 0 ) {
				// skip
				hasSkipFlag = true;
			} else {
				strQuotient[i] = ('0' + iQuotient);
			}
			previousRemainder = dividend & 0x1;  // =>  previousRemainder = (dividend % 2)
 
		}

		if ( hasSkipFlag ) {
			strQuotient = strQuotient.substr(1);
		}
		strDecimalDivident = strQuotient;
		m_binaryDigitList.push_front( previousRemainder );
	} while ( strQuotient != "0" );

	return m_binaryDigitList.size();
}

size_t StringNumber::hex2Bin()
{
	m_binaryDigitList.clear();

    string copyStr(m_strNumber);
    if ( copyStr.size() <= 2  ) {
        MyException e(E_THROW_INVALID_HEX_NUMBER);
        e.setDetail(" Invalid hex number , not start with \'0x\' or \'0X\' " );
        throw e;
    }

	string prefix2 = copyStr.substr(0,2);
    if ( (prefix2 != "0x") && (prefix2 != "0X") ) {
        MyException e(E_THROW_INVALID_HEX_NUMBER);
        e.setDetail(" Hex number\'s prefix is not \'0x\' or \'0X\' " );
        throw e;
    }

	copyStr = copyStr.substr(2);
    if ( copyStr.empty()  ) {
        MyException e(E_THROW_INVALID_HEX_NUMBER);
        e.setDetail(" after cut prefix '0x', empty hex number " );
        throw e;
    }

	size_t len  = copyStr.size();
	for( size_t i = 0; i < len; ++i ) {
		char ch = static_cast<char>( toupper( static_cast<unsigned char>(copyStr.at(i) ) ) );
		auto it = s_HexMap.find(ch);
		if ( it == s_HexMap.end() ) {
			MyException e(E_THROW_INVALID_HEX_NUMBER);
			e.setDetail( string("Invalid ch : \'") + string(1, ch) + string("\' inside Hex Number. ") );
			throw e;
		}

		for( const auto& zeroOrOne : it->second ) {
			m_binaryDigitList.push_back(zeroOrOne);
		}
	}

	return m_binaryDigitList.size();
}



size_t StringNumber::convert2Bin()
{
	size_t ret = 0;
	if ( m_base == 8 ) {
		ret = oct2Bin();
	} else if ( m_base == 10 ) {
		ret = dec2Bin();
	} else if ( m_base == 16 ) {
		ret = hex2Bin();
	} 

	return ret;
}


void StringNumber::truncateToTargetSize(size_t targetSz)
{
	while( m_binaryDigitList.size() > targetSz ) {
		m_binaryDigitList.pop_front();
	}
}



unsigned long      StringNumber::getULongValue()
{
	static const size_t FIXED_BIT_COUNT = (sizeof(long) * 8);
	bitset<FIXED_BIT_COUNT> bits; bits.reset();

	size_t idx = 0;
	for( auto it = m_binaryDigitList.rbegin(); it != m_binaryDigitList.rend(); ++it, ++idx ) {
		bool flag = (*it != 0);
		bits[idx] = flag;
	}

	return bits.to_ulong();

}

unsigned long long StringNumber::getULongLongValue()
{
	bitset<64> bits; bits.reset();

	size_t idx = 0;
	for( auto it = m_binaryDigitList.rbegin(); it != m_binaryDigitList.rend(); ++it, ++idx ) {
		bool flag = (*it != 0);
		bits[idx] = flag;
	}

	return bits.to_ullong();
}

