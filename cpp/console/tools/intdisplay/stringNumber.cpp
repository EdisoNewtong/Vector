
#include "stringNumber.h"
#include "myException.h"
#include "charutil.h"

// #include <iostream>
#include <limits>
#include <cmath>
#include <unordered_map>

using namespace charutil;

//
// static member init
//



/* static */ const int StringNumber::s_2to8Bit      = 3;
/* static */ const int StringNumber::s_2to16Bit     = 4;
/* static */ const int StringNumber::s_nBitsPerByte = 8;





StringNumber::StringNumber(const string& snumber,int base) 
    : m_snumber(snumber) 
    , m_orignalStr(snumber)
    , m_base(base)
{

    if ( m_snumber.empty() ) {
        throw MyException(" StringNumber::StringNumber(const string& snumber,int base) because empty string");
    }

    if ( m_base == 2 ) {
        check2();
    } else if ( m_base == 8 ) {
        check8();
    } else if ( m_base == 10 ) {
        check10();
    } else if ( m_base == 16 ) {
        check16();
    }
}


StringNumber::StringNumber(const  StringNumber& rnum) 
    : m_snumber(rnum.m_snumber) 
    , m_orignalStr(rnum.m_orignalStr)
    , m_base(rnum.m_base)
{ 
}


StringNumber& StringNumber::operator = (const StringNumber& right) {
    this->m_snumber = right.m_snumber;
    this->m_base    = right.m_base;
    this->m_orignalStr = right.m_orignalStr;
    return *this;
}


void StringNumber::resetNumber(const string& snumber, int base)
{
    m_snumber = snumber;
    m_base    = base;
}


StringNumber StringNumber::operator + (const StringNumber& right) 
{
    //
    // Addition 2 Decimal Numbers Only Function
    //
    string strLeftNumber  = m_snumber;
    string strRightNumber = right.m_snumber;

    int leftbit  = static_cast<int>( strLeftNumber.size() );
    int rightbit = static_cast<int>( strRightNumber.size() );
    int sameLen = 0;
    if ( leftbit != rightbit ) {
        if ( leftbit < rightbit ) {
            int delta = rightbit - leftbit;
            strLeftNumber.insert(0,delta,'0'); // Make two number same bit
            sameLen = rightbit;
        } else {
            // leftbit > rightbit
            // rightbit < leftbit
            int delta = leftbit - rightbit;
            strRightNumber.insert(0,delta,'0'); // Make two number same bit
            sameLen = leftbit;
        }
    }
    else {
        sameLen = leftbit;
    }

    string errorMsg;
    string finalresult;
    finalresult.resize(sameLen+1, '0');
    int carryBit = 0;
    int lastIdx = sameLen-1;
    for( int idx = lastIdx; idx >=0; --idx )
    {
        char leftNumber  = strLeftNumber.at(idx);
        char rightNumber = strRightNumber.at(idx);
        int  nLeft  = static_cast<int>( leftNumber  - '0');
        int  nRight = static_cast<int>( rightNumber - '0');
        int     sum = nLeft + nRight + carryBit;

        int mapedIdx = idx+1;
        if ( sum >= 10 ) {
            finalresult[mapedIdx] = static_cast<char>( char('0') + (sum-10) );
            carryBit = 1;
        } else {
            finalresult[mapedIdx] = static_cast<char>( char('0') + sum );
            carryBit = 0;
        }
    }

    if ( carryBit == 1 ) {
        finalresult[0] = '1';
    } else {
        // erase beginning '0' if necessary
        if ( finalresult.at(0) == '0' ) {
            finalresult.erase(0, 1);
        }
    }

    return StringNumber(finalresult, 10);
}

// StringNumber StringNumber::operator - (const StringNumber& right) 
// {
//     StringNumber result(*this);
//     return result;
// }

StringNumber StringNumber::operator * (const StringNumber& right) 
{
    //
    // Multiplication 2 Decimal Numbers Only Function
    //
    string strLeftNumber  = m_snumber;
    string strRightNumber = right.m_snumber;

    auto isAllZero1 = true;
    for( int idx = 0; idx < static_cast<int>(strLeftNumber.size()); ++idx ) {
        char nBit = strLeftNumber.at(idx);
        if ( nBit != '0' ) {
            isAllZero1 = false;
            break;
        }
    }

    if ( isAllZero1 ) {
        if ( strLeftNumber.empty() ) {
            throw MyException("Empty left number in operator + (right)");
        } else {
            return StringNumber(string("0"), 10);
        }
    }

    auto isAllZero2 = true;
    for( int idx = 0; idx < static_cast<int>(strRightNumber.size()); ++idx ) {
        char nBit = strRightNumber.at(idx);
        if ( nBit != '0' ) {
            isAllZero2 = false;
            break;
        }
    }

    if ( isAllZero2 ) {
        if ( strRightNumber.empty()  ) {
            throw MyException("Empty right number in operator + (right)");
        } else {
            return StringNumber(string("0"), 10);
        }
    }


    auto isLeft_1 = false;
    auto isRight_1 = false;
    if (       (isLeft_1  = (strLeftNumber.size()  == 1 && strLeftNumber.at(0) == '1')  )
            || (isRight_1 = (strRightNumber.size() == 1 && strRightNumber.at(0) == '1') )  )
    {
        if ( isLeft_1 ) {
            return StringNumber(right);
        } else {
            // return Left number
            return StringNumber(m_snumber, 10);
        }
    }

    StringNumber copyRight(right);
    StringNumber multiplyResult("0", 10);
    do {
        multiplyResult = multiplyResult + (*this);
        copyRight.decimalMinus1();
    } while( !copyRight.isZero() );

    return multiplyResult;
}


pair<StringNumber,int> StringNumber::devide2()
{
    if ( m_base != 10 ) {
        throw MyException(" invalid function devide2 on the number which is not decimal  ");
    }

    string copyedDecimal = m_snumber;
    char firstCh = m_snumber.front();
    if ( firstCh == '-' ) {
        copyedDecimal.erase(0,1);
    }


    int len = static_cast<int>( copyedDecimal.size() );
    string strRes;
    int rest = 0;
    for( int i = 0; i < len; ++i ) 
    {
        char ch = copyedDecimal.at(i);
        int num = static_cast<int>( ch - '0');

        if ( rest == 0 ) {
            if ( num <= 1 ) {
                if ( i!=0 || len == 1 ) {
                    strRes.push_back('0');
                }
                rest = (num == 0 ? 0 : 1);
            } else {
                // num >=2
                int mod = num / 2;
                strRes.push_back( static_cast<char>('0' + mod) );
                rest = num % 2;
            }
        } else {
            // rest == 1
            num += 10;
            int quoitent = num / 2;
            int remainder = num % 2;
            strRes.push_back( static_cast<char>('0' + quoitent) );
            rest = remainder;
        }
    }

    return make_pair( StringNumber(strRes,10), rest);
}


void StringNumber::decimalMinus1()
{
    //
    //  Only Support Decimal Number
    //
    int lastIdx = m_snumber.size() - 1;
    char ch = m_snumber.at(lastIdx);

    if ( ch == '0' ) {
        if ( lastIdx == 0 ) {
            // Can't minus to negative number  , do not assign to "-1"
            m_snumber = "0";
        } else {
            // lastIdx > 0
            int preIdx = 0;
            for( preIdx = lastIdx-1; preIdx>=0;  )
            {
                char preCh = m_snumber.at(preIdx);

                if ( preCh == '0' ) {
                    m_snumber.at(preIdx) = '9';
                    --preIdx;
                } else {
                    m_snumber.at(preIdx) = preCh - 1;
                    break;
                }
            }

            if( m_snumber.size() > 1 && m_snumber.at(0) == '0'  ) {
                // delete 1st letter
                m_snumber.erase(0,1);
            }
        }
    } else {
        // ch :  inside [1 ~ 9]
        m_snumber.at(lastIdx) = ch - 1;
    }
}


string StringNumber::binMinus1(const string& input)
{

    string copyStr(input);
    if ( copyStr.empty() ) {
        throw MyException("Empty string @binMinus1(...)");
    }

    auto lastCh = copyStr.back();
    if ( isbB(lastCh) ) {
        // throw MyException("invalid binary string @binMinus1(...)");
        copyStr.pop_back();
    }

    if ( copyStr.empty() ) {
        throw MyException("Empty string after remove 'b/B' @binMinus1(...)");
    }

    
    int len = static_cast<int>( copyStr.size() );
    for( int idx = len-1; idx>=0; --idx )
    {
        char ch = copyStr.at(idx);
        if ( ch == '1' ) {
            copyStr.at(idx) = '0';
            break;
        } else {
            // ch == '0'
            copyStr.at(idx) = '1';
        }
    }

    return copyStr;
}


string StringNumber::flipBin(const string& input)
{
    string copyStr(input);
    int len = static_cast<int>( copyStr.size() );
    for( int idx = 0; idx < len; ++idx )
    {
        char ch = copyStr.at(idx);
        if ( ch == '0' ) {
            copyStr.at(idx) = '1';
        } else if ( ch == '1' ) {
            copyStr.at(idx) = '0';
        } else {
            throw MyException(string("invalid ch at binary(0/1 only) str : ") + copyStr );
        }
    }

    return copyStr;
}






bool StringNumber::isZero()
{
    if ( m_snumber.empty() ) {
        throw MyException("Empty string check isZero()");
    }

    auto isAllZero = true;
    for( int idx = 0; idx < static_cast<int>(m_snumber.size()); ++idx ) {
        char nBit = m_snumber.at(idx);
        if ( nBit != '0' ) {
            isAllZero = false;
            break;
        }
    }

    return isAllZero;
}




/*
bool StringNumber::operator < (const StringNumber& right)
{
    (void)right;
    return true;
}

bool StringNumber::operator <= (const StringNumber& right)
{
    (void)right;
    return true;
}

bool StringNumber::operator == (const StringNumber& right)
{
    (void)right;
    return true;
}

bool StringNumber::operator > (const StringNumber& right)
{
    (void)right;
    return true;
}

bool StringNumber::operator >= (const StringNumber& right)
{
    (void)right;
    return true;
}
*/



void StringNumber::check2()
{
    char lastCh = m_snumber.back();
    if ( !isbB(lastCh) ) {
        throw MyException("Missing b/B at binary string suffix");
    }

    string errorMsg;
    string cutbB = m_snumber.substr(0, m_snumber.size() - 1);
    if ( cutbB.empty() ) {
        throw MyException("Empty binary string \"cutbB\" @check2()");
    }

    int len = static_cast<int>( cutbB.size() );
    for( auto idx = 0; idx < len; ++idx ) {
        char ch = cutbB.at(idx);
        if ( !isBinaryNumber(ch) ) {
            errorMsg = string("Invalid char : '")  + ch + "' at binary string";
            throw MyException(errorMsg);
        }
    }

    int sz = static_cast<int>( cutbB.size() );
    int placehold = getBinaryPlaceholdZeroCnt(sz);
    if ( placehold != 0 ) {
        cutbB.insert(0, placehold,'0');
    }

    m_snumber = cutbB;
}

void StringNumber::check8()
{
    char firstCh = m_snumber.front();
    if ( firstCh != '0' ) {
        throw MyException("'0' is not at the first char of Octal int format");
    }

    string errorMsg;
    string cut0 = m_snumber.substr(1);
    if ( cut0.empty() ) {
        throw MyException("Empty Octal string \"cut0\" @check8()");
    }


    int len = static_cast<int>( cut0.size() );
    for( auto idx = 0; idx < len; ++idx ) {
        char ch = cut0.at(idx);
        if ( !isOctNumber(ch) ) {
            errorMsg = string("invalid char : '")  + ch + "' at octal string";
            throw MyException(errorMsg);
        }
    }

    m_snumber = cut0;
}

void StringNumber::check10()
{
    string cutNegative = m_snumber;
    int firstCh = cutNegative.front();
    if ( firstCh == '-' ) {
        cutNegative = m_snumber.substr(1);
    }

    if ( cutNegative.empty() ) {
        throw MyException("invalid decimal format , empty string after '-'");
    }

    string errorMsg;
    int len = static_cast<int>( cutNegative.size() );
    for( auto idx = 0; idx < len; ++idx ) {
        char ch = cutNegative.at(idx);
        if ( !isNumber(ch) ) {
            errorMsg = string("invalid char : '")  + ch + "' at decimal string";
            throw MyException(errorMsg);
        }
    }

}

void StringNumber::check16()
{
    int len = m_snumber.size();
    if ( len <= 2 ) {
        throw MyException("invalid hex format , str.length <=2");
    }

    string prefix = m_snumber.substr(0,2);
    if ( prefix != "0x" && prefix != "0X" ) {
        throw MyException("invalid hex format , prefix is neither 0x nor 0X");
    }

    string errorMsg;
    string strCut0x = m_snumber.substr(2);
    len = static_cast<int>( strCut0x.size() );
    for( auto idx = 0; idx < len; ++idx ) {
        char ch = strCut0x.at(idx);
        if ( !isHexNumber(ch) ) {
            errorMsg = string("invalid char : '")  + ch + "' at hex string";
            throw MyException(errorMsg);
        }
    }

    m_snumber = strCut0x;
}



string StringNumber::getOriginalStr() const
{
    return m_orignalStr;
}

string StringNumber::getResult() const
{
    return m_snumber;
}


int StringNumber::getBase() const
{
    return m_base;
}


string StringNumber::calcBinary()
{
    string retstr;
    if ( m_base == 2 ) {
        retstr = cutbB( m_orignalStr );
        int len = static_cast<int>( retstr.size() );
        int remain = getBinaryPlaceholdZeroCnt( len );
        if ( remain > 0 ) {
            retstr.insert(0, remain, '0');
        }
        retstr += 'B';
        return retstr;
    }

    // 8 10 16
    if ( m_base == 8 ) {
        // 8  -->  2
        retstr = base8_2( m_orignalStr );
    } else if ( m_base == 10 ) {
        // 10 -->  2
        retstr = base10_2( m_orignalStr );
    } else if ( m_base == 16 ) {
        // 16 -->  2
        retstr = base16_2( m_orignalStr );
    }

    return retstr;
}

string StringNumber::calcOctal()
{
    if ( m_base == 8 ) {
        return base2_8(base8_2(m_orignalStr));
    }

    string retstr;
    // 2 10 16
    if ( m_base == 2 ) {
        // 2  -->  8
        retstr = base2_8(m_orignalStr);
    } else if ( m_base == 10 ) {
        // 10 -->  8
        retstr = base2_8( base10_2( m_orignalStr ) );
    } else if ( m_base == 16 ) {
        // 16 -->  8
        retstr = base2_8( base16_2( m_orignalStr) );
    }

    return retstr;
}

string StringNumber::calcDecimal()
{
    if ( m_base == 10 ) {
        return m_orignalStr;
    }

    string retstr;
    // 2 8 16
    if ( m_base == 2 ) {
        // 2  -->  10 
        retstr = base2_10(m_orignalStr);
    } else if ( m_base == 8 ) {
        // 8  -->  10
        retstr = base2_10( base8_2( m_orignalStr ) );
    } else if ( m_base == 16 ) {
        // 16  -->  10 
        retstr = base2_10( base16_2( m_orignalStr) );
    }

    return retstr;
}


string StringNumber::calcDecimalWithSignedFlag()
{
    if ( m_base == 10 ) {
        return m_orignalStr;
    }


    string copyStr;
    string retstr;
    // 2 8 16
    if ( m_base == 2 ) {
        // 2  -->  10 
        copyStr = m_snumber;
    } else if ( m_base == 8 ) {
        // 8  -->  10
        copyStr = base8_2( m_orignalStr );
    } else if ( m_base == 16 ) {
        // 16  -->  10 
        copyStr = base16_2( m_orignalStr );
    }

    //
    // process if sign bit is '1'
    //
    int binLen = copyStr.size();
    int nBits = 0;
    if ( isbB(copyStr.back()) ) {
        nBits = binLen - 1;
    } else {
        nBits = binLen;
    }

    int placeholdCnt = getBinaryPlaceholdZeroCnt( nBits );
    if ( placeholdCnt == 0 )  {
        char firstCh = copyStr.front();
        if ( firstCh == '1') {
            // 
            // current is Negative number
            string minus1num = binMinus1(copyStr);
            string flipNum   = flipBin(minus1num);
            retstr = base2_10(flipNum);

            // Core Code : Add Negative Flag
            retstr = string("-") + retstr;
        } else {
            retstr = base2_10(copyStr);
        }
    } else {
        retstr = base2_10(copyStr);
    }


    return retstr;
}


string StringNumber::calcHex()
{
    string retstr;
    if ( m_base == 16 ) {
        retstr = base2_16( base16_2( m_orignalStr ) );
    }

    // 2 8 10
    if ( m_base == 2 ) {
        // 2  -->  16 
        retstr = base2_16(m_orignalStr);
    } else if ( m_base == 8 ) {
        // 8  -->  16
        retstr = base2_16( base8_2( m_orignalStr ) );
    } else if ( m_base == 10 ) {
        // 10  -->  16 
        retstr = base2_16( base10_2( m_orignalStr) );
    }

    return retstr;
}




////////////////////////////////////////////////////////////////////////
//
// util
//
////////////////////////////////////////////////////////////////////////
string StringNumber::base2_16(const string& input)
{
    //
    // return hex format without prefix 0x|0X
    //
    auto useUppercase = true;

    static const unordered_map<string,char> fixedHexMp{
        { string("0000"), '0' },
        { string("0001"), '1' },
        { string("0010"), '2' },
        { string("0011"), '3' },
        { string("0100"), '4' },
        { string("0101"), '5' },
        { string("0110"), '6' },
        { string("0111"), '7' },
        { string("1000"), '8' },
        { string("1001"), '9' },
        { string("1010"), (useUppercase ? 'A' : 'a') },
        { string("1011"), (useUppercase ? 'B' : 'b') },
        { string("1100"), (useUppercase ? 'C' : 'c') },
        { string("1101"), (useUppercase ? 'D' : 'd') },
        { string("1110"), (useUppercase ? 'E' : 'e') },
        { string("1111"), (useUppercase ? 'F' : 'f') }
    };

    string copyInput(input);
    int len = static_cast<int>( copyInput.size() );
    if ( len == 1 ) {
        char ch = copyInput.at(0);
        if ( isbB(ch) ) {
            throw MyException(string(" invalid binary number string : ") + copyInput);
        }
    } else {
        // len >=2
        char ch = copyInput.back();
        if ( isbB(ch) ) {
            copyInput.pop_back();
            --len;
        }
    }

    string result;
    int placeholdCnt = getBinaryPlaceholdZeroCnt( len );
    if ( placeholdCnt != 0 ) {
        copyInput.insert(0, placeholdCnt, '0');
    }
    len = static_cast<int>( copyInput.size() );

    int grpCnt = len / s_2to16Bit;
    result.resize(grpCnt, '0');
    
    // config lowercase or uppercase

    for ( int i = 0; i < grpCnt; ++i ) {
        string part = copyInput.substr(i*s_2to16Bit , s_2to16Bit);
        auto it = fixedHexMp.find(part);
        if ( it != fixedHexMp.end() ) {
            result[i] = it->second;
        } else {
            throw MyException(string(" invalid binary -> hex , part number string : [") + part + "]");
        }
    }


    // Add prefix "0x"
    result = string("0x") + result;
    return result;
}

string StringNumber::base2_8(const string& input)
{
    static const unordered_map<string,char> fixedOctalMp{
        { string("0"), '0' },
        { string("1"), '1' },
        ///////////////////////////////
        { string("00"), '0' },
        { string("01"), '1' },
        { string("10"), '2' },
        { string("11"), '3' },
        ///////////////////////////////
        { string("000"), '0' },
        { string("001"), '1' },
        { string("010"), '2' },
        { string("011"), '3' },
        { string("100"), '4' },
        { string("101"), '5' },
        { string("110"), '6' },
        { string("111"), '7' }
    };

    //
    // return octal format without prefix '0'
    //
    string copyInput(input);
    int len = static_cast<int>( copyInput.size() );
    if ( len == 1 ) {
        char ch = copyInput.at(0);
        if ( isbB(ch)  ) {
            throw MyException(string(" invalid binary number string : ") + copyInput);
        }
    } else {
        // len >=2
        char ch = copyInput.back();
        if ( isbB(ch) ) {
            copyInput.pop_back();
            --len;
        }
    }

    string result;
    // int grpCnt = len / s_2to8Bit;
    int placeholdCnt = getBinaryPlaceholdZeroCnt( len );
    if ( placeholdCnt != 0 ) {
        copyInput.insert(0, placeholdCnt, '0');
    }
    len = static_cast<int>( copyInput.size() );

    int grpCnt = len / s_2to8Bit;
    int remain = len % s_2to8Bit;
    if ( remain > 0 ) {
        result.resize(grpCnt + 1, '0');

        string prestr = copyInput.substr(0, remain);
        auto itPre = fixedOctalMp.find(prestr);
        if ( itPre != fixedOctalMp.end() ) {
            result[0] = itPre->second;
        } else {
            throw MyException(string(" invalid binary -> octal , part number string : [") +  prestr + "]");
        }



        for ( int i = 0; i < grpCnt; ++i ) {
            string part = copyInput.substr(remain + i*s_2to8Bit , s_2to8Bit);
            auto it = fixedOctalMp.find(part);
            if ( it != fixedOctalMp.end() ) {
                result[1+i] = it->second;
            } else {
                throw MyException(string(" invalid binary -> octal , part number string : [") + part + "]");
            }
        }

    } else {
        result.resize(grpCnt, '0');

        for ( int i = 0; i < grpCnt; ++i ) {
            string part = copyInput.substr(i*s_2to8Bit , s_2to8Bit);
            auto it = fixedOctalMp.find(part);
            if ( it != fixedOctalMp.end() ) {
                result[i] = it->second;
            } else {
                throw MyException(string(" invalid binary -> octal , part number string : [") + part + "]");
            }
        }
    }


    // Add prefix '0'
    result.insert(0,1, '0');
    return result;
}


string StringNumber::base2_10(const string& input)
{
    string copyStr(input);
    if ( !copyStr.empty() && isbB(copyStr.back() ) ) {
        // remove 'b/B' at tail if necessary
        copyStr.pop_back();
    }


    StringNumber rate("2", 10);
    StringNumber convertRet("0", 10);
    int j = 0;
    int len = static_cast<int>( copyStr.size());
    StringNumber currentRate("1", 10);
    for( int idx = len-1; idx >= 0; --idx, ++j)
    {
        char ch = copyStr.at(idx);
        int num = static_cast<int>(ch - '0');

        if ( j > 0 ) {
            currentRate = currentRate * rate;
            if ( num == 1 ) {
                convertRet = convertRet + currentRate;
            }
        } else {
            // j = 0
            convertRet.m_snumber = (num == 1  ? "1" : "0");
        }
    }

    return convertRet.m_snumber;
}





string StringNumber::base16_2(const string& input)
{
    //
    // return binary format with suffix 'b|B'
    //
    string copyInput(input);
    string prefix = copyInput.substr(0,2);
    if ( prefix == "0x"  || prefix == "0X" ) {
        copyInput.erase(0,2);
    }

    string result;
    int arySz = s_2to16Bit * static_cast<int>( copyInput.size() );
    result.resize(arySz, '0');

    int idxBin = 0;
    int len = static_cast<int>( copyInput.size() );
    for( int idx = 0; idx < len; ++idx, idxBin += s_2to16Bit ) 
    {
        char ch = copyInput.at(idx);
        switch( ch )
        {
        case '0':
            result[idxBin]   = '0';
            result[idxBin+1] = '0';
            result[idxBin+2] = '0';
            result[idxBin+3] = '0';
            break;
        case '1':
            result[idxBin]   = '0';
            result[idxBin+1] = '0';
            result[idxBin+2] = '0';
            result[idxBin+3] = '1';
            break;
        case '2':
            result[idxBin]   = '0';
            result[idxBin+1] = '0';
            result[idxBin+2] = '1';
            result[idxBin+3] = '0';
            break;
        case '3':
            result[idxBin]   = '0';
            result[idxBin+1] = '0';
            result[idxBin+2] = '1';
            result[idxBin+3] = '1';
            break;
        case '4':
            result[idxBin]   = '0';
            result[idxBin+1] = '1';
            result[idxBin+2] = '0';
            result[idxBin+3] = '0';
            break;
        case '5':
            result[idxBin]   = '0';
            result[idxBin+1] = '1';
            result[idxBin+2] = '0';
            result[idxBin+3] = '1';
            break;
        case '6':
            result[idxBin]   = '0';
            result[idxBin+1] = '1';
            result[idxBin+2] = '1';
            result[idxBin+3] = '0';
            break;
        case '7':
            result[idxBin]   = '0';
            result[idxBin+1] = '1';
            result[idxBin+2] = '1';
            result[idxBin+3] = '1';
            break;
        case '8':
            result[idxBin]   = '1';
            result[idxBin+1] = '0';
            result[idxBin+2] = '0';
            result[idxBin+3] = '0';
            break;
        case '9':
            result[idxBin]   = '1';
            result[idxBin+1] = '0';
            result[idxBin+2] = '0';
            result[idxBin+3] = '1';
            break;
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        case 'a':
        case 'A':
            result[idxBin]   = '1';
            result[idxBin+1] = '0';
            result[idxBin+2] = '1';
            result[idxBin+3] = '0';
            break;
        case 'b':
        case 'B':
            result[idxBin]   = '1';
            result[idxBin+1] = '0';
            result[idxBin+2] = '1';
            result[idxBin+3] = '1';
            break;
        case 'c':
        case 'C':
            result[idxBin]   = '1';
            result[idxBin+1] = '1';
            result[idxBin+2] = '0';
            result[idxBin+3] = '0';
            break;
        case 'd':
        case 'D':
            result[idxBin]   = '1';
            result[idxBin+1] = '1';
            result[idxBin+2] = '0';
            result[idxBin+3] = '1';
            break;
        case 'e':
        case 'E':
            result[idxBin]   = '1';
            result[idxBin+1] = '1';
            result[idxBin+2] = '1';
            result[idxBin+3] = '0';
            break;
        case 'f':
        case 'F':
            result[idxBin]   = '1';
            result[idxBin+1] = '1';
            result[idxBin+2] = '1';
            result[idxBin+3] = '1';
            break;
        default:
            throw MyException(string(" invalid hex char : ") + ch);
            break;
        }
    }

    int placeholdCnt = getBinaryPlaceholdZeroCnt(arySz);
    if ( placeholdCnt != 0 ) {
        result.insert(0, placeholdCnt, '0');
    }

    // Add Suffix 'B'
    result += 'B';
    return result;
}

string StringNumber::base8_2(const string& input)
{
    //
    // return binary format with suffix 'b|B'
    //
    string copyInput(input);
    int len = static_cast<int>( copyInput.size() );
    if ( len >= 2 ) {
        char firstCh = copyInput.at(0);
        if ( firstCh == '0' ) {
            copyInput.erase(0,1); // remove beginning '0'
            --len;
        }
    }

    string result;
    result.resize(s_2to8Bit * len, '0');
    int arySz = static_cast<int>( s_2to8Bit * len);

    int binIdx = 0;
    for( int idx = 0; idx < len; ++idx, binIdx += s_2to8Bit)
    {
        char ch = copyInput.at(idx);
        switch( ch )
        {
        case '0':
            result[binIdx]   = '0';
            result[binIdx+1] = '0';
            result[binIdx+2] = '0';
            break;
        case '1':
            result[binIdx]   = '0';
            result[binIdx+1] = '0';
            result[binIdx+2] = '1';
            break;
        case '2':
            result[binIdx]   = '0';
            result[binIdx+1] = '1';
            result[binIdx+2] = '0';
            break;
        case '3':
            result[binIdx]   = '0';
            result[binIdx+1] = '1';
            result[binIdx+2] = '1';
            break;
        case '4':
            result[binIdx]   = '1';
            result[binIdx+1] = '0';
            result[binIdx+2] = '0';
            break;
        case '5':
            result[binIdx]   = '1';
            result[binIdx+1] = '0';
            result[binIdx+2] = '1';
            break;
        case '6':
            result[binIdx]   = '1';
            result[binIdx+1] = '1';
            result[binIdx+2] = '0';
            break;
        case '7':
            result[binIdx]   = '1';
            result[binIdx+1] = '1';
            result[binIdx+2] = '1';
            break;
        default:
            throw MyException(string(" invalid octal char : ") + ch);
            break;
        }
    }

    int remain = getBinaryPlaceholdZeroCnt(arySz);
    if ( remain > 0 ) {
        result.insert(0, remain, '0');
    }

    
    // Add Suffix 'B'
    result += 'B';
    return result;
}


string StringNumber::base10_2(const string& input)
{
    //
    // return binary format with suffix 'b|B'
    //
    string copyInput(input);
    if ( copyInput.empty() ) {
        throw MyException( " empty string @ base10_2(...)" );
    }

    char firstCh = copyInput.at(0);
    auto bIsNegative = false;
    if ( firstCh == '-' ) {
        bIsNegative = true;
        copyInput.erase(0,1);
    }

    string retstr;
    StringNumber cpNumber(copyInput,10);
    do {
        auto pr = cpNumber.devide2();
        cpNumber = pr.first;
        int rest = pr.second;
        retstr.insert(0,1, static_cast<int>('0' + rest) );
    } while ( !(cpNumber.m_snumber.size() == 1 && cpNumber.m_snumber.at(0) == '0') );

    int len = static_cast<int>(retstr.size());
    if ( bIsNegative ) {

        int remain = getBinaryPlaceholdZeroCnt(len);
        if ( remain > 0 ) {
            len += remain;
            retstr.insert(0, remain, '0');
        }


        // do op ~    shift 0 -> 1  ,  1 -> 0
        for( auto i = 0; i < len; ++i )
        {
            char ch = retstr.at(i);
            retstr.at(i) = (ch == '0' ? '1' : '0');
        }

        // flip number += 1
        // int carry = 0;
        for( int rIdx = len-1; rIdx >=0; --rIdx )
        {
            char ch = retstr.at(rIdx);

            if ( ch == '0' ) {
                retstr.at(rIdx) = '1';
                // carry = 0;
                break;
            } else {
                // ch == '1'
                // '1' + '1' = "10"
                retstr.at(rIdx) = '0';
                // carry = 1;
            }
        }

        auto ch1st = retstr.at(0);
        if ( ch1st != '1' ) {
            // As a negative number ,  the highest bit must be 1 , otherwise the existed bits can't meet the demand , the memory size of the varible is insufficent
            // should be expand to double 

            // int newlen = len * 2;
            retstr.insert(0, len, '1');
        }
    } else {
        int remain = getBinaryPlaceholdZeroCnt(len);
        if ( remain > 0 ) {
            retstr.insert(0, remain, '0');
        }
        
    }


    // Add Suffix 'B'
    retstr += 'B';
    return retstr;
}


int StringNumber::getBinaryPlaceholdZeroCnt(int nBinaryBits)
{
    if ( nBinaryBits <= 0 ) {
        throw MyException( std::string(" invalid arg : nBinaryBits = ") + to_string(nBinaryBits) + " <= 0 @ getBinaryPlaceholdZeroCnt(...) ");
    }

    int ret = 0;

    float exp = log2( static_cast<float>(nBinaryBits) );
    int   upCeil = static_cast<int>( ceil(exp) );
    // 2^3 = 8 = 1 byte
    int nBytes = 0;
    if ( upCeil < 3 ) {
        // 1 byte
        nBytes = 1;
    } else {
        // upCeil >= 3
        nBytes = static_cast<int>( pow(2.0f, upCeil) ) / 8;
    }

    ret = nBytes * s_nBitsPerByte - nBinaryBits;
    return ret;
}


string StringNumber::cutbB( const string& input)
{
    string ret(input);
    if ( !ret.empty() && isbB(ret.back() ) ) {
        ret.pop_back(); // remove 'b/B'
    }

    return ret;

}

string StringNumber::cutZero( const string& input)
{
    string ret(input);
    if ( !ret.empty() &&    (ret.front() == '0') ) {
        int len = static_cast<int>( ret.size() );
        if ( len > 1 ) {
            ret = ret.substr(1);
        }
    }

    return ret;
}


string StringNumber::cut0x( const string& input)
{
    string ret(input);
    int len = static_cast<int>( ret.size() );
    if ( len > 2 ) {
        string prefix = ret.substr(0,2);
        if ( prefix == "0x" || prefix == "0X" ) {
            ret = ret.substr(2);
        }
    }

    return ret;
}



string StringNumber::getDetail()
{
    static const string inputTitle("Input          : ");
    static const string sperateLine("---------------------------------------");
    static const string binTitle("Binary           : ");
    static const string nBitTitle("N-th Bit         : ");
    static const string octTitle("Octal            : ");
    static const string de1Title("Decimal unsigned : ");
    static const string de2Title("Decimal   signed : ");
    static const string hexTitle("Hex              : ");
    static const string leftParathesis(" ( ");
    static const string sbits(" bits , ");
    static const string sbytes(" byte(s) ");
    static const string rightParathesis(" ) ");
    static const string endline("\n");
    static const string prefixBlankSpace("| ");
    static const string space(" ");
    static const string suffixEnd("|");
    static const string rangeHyphen(" ~ ");
    // 
    static const string strBin("Binary Type");
    static const string strOct("Octal Type");
    static const string strDec("Decimal Type");
    static const string strHex("Hex Type");
    // 
    static const string strChar("<char>");
    static const string strShort("<short>");
    static const string strInt( sizeof(int) == 4 ? "<int>" : "<long>");
    static const string strLongLong("<long long>");
    static const string strLarger(" >= sizeof(long long) ");

    string detail;
    string binStr = calcBinary();  // with 'B' suffix
    string octalStr = calcOctal(); // with '0' prefix
    string unsignedDecimal = calcDecimal();
    string signedDecimal = calcDecimalWithSignedFlag();
    string hexStr = calcHex(); // with 0x prefix

    // if ( !binStr.empty() &&   isbB(binStr.back()) ) {
    //     binStr.pop_back(); // remove 'b/B'
    // }

    detail += (sperateLine + endline);
    // print Original Info
    detail += (inputTitle + getOriginalStr() + leftParathesis); 
    if ( m_base == 2 ) {
        detail += strBin;
    } else if ( m_base == 8 ) {
        detail += strOct;
    } else if ( m_base == 10 ) {
        detail += strDec;
    } else if ( m_base == 16 ) {
        detail += strHex;
    }
    detail += (rightParathesis + endline);
    detail += (sperateLine + endline);

    ////////////////////////////////////////////////////////////////
    //
    // Binary Part
    //
    detail += (binTitle + binStr);        // + endline);
    string strcutbB = cutbB(binStr);
    int nBits = static_cast<int>( strcutbB.size() );
    int nBytes = nBits / s_nBitsPerByte;
    // Add detail
    string strDataType;
    if ( nBytes == 1 ) {
        strDataType = strChar;
    } else if ( nBytes == 2 ) {
        strDataType = strShort;
    } else if ( nBytes == 4 ) {
        strDataType = strInt;
    } else if ( nBytes == 8 ) {
        strDataType = strLongLong;
    } else {
        strDataType = strLarger;
    }

    detail += (leftParathesis + space + strDataType + space + to_string(nBits) + sbits + to_string(nBytes) + sbytes + rightParathesis + endline);
    string unsignedRange;
    string   signedRange;
    if (  nBytes <= static_cast<int>( sizeof(unsigned long long) ) ) {
        if ( nBytes == 1 ) {
            int ucharMin = static_cast<int>( numeric_limits<unsigned char>::min() );
            int ucharMax = static_cast<int>( numeric_limits<unsigned char>::max() );
            int scharMin = static_cast<int>( numeric_limits<  signed char>::min() );
            int scharMax = static_cast<int>( numeric_limits<  signed char>::max() );
            unsignedRange += ( to_string(ucharMin) + rangeHyphen  + to_string(ucharMax));
              signedRange += ( to_string(scharMin) + rangeHyphen  + to_string(scharMax));
        } else if ( nBytes == 2 ) {
            int ushortMin = static_cast<int>( numeric_limits<unsigned short>::min() );
            int ushortMax = static_cast<int>( numeric_limits<unsigned short>::max() );
            int sshortMin = static_cast<int>( numeric_limits<  signed short>::min() );
            int sshortMax = static_cast<int>( numeric_limits<  signed short>::max() );
            unsignedRange += ( to_string(ushortMin) + rangeHyphen + to_string(ushortMax) );
              signedRange += ( to_string(sshortMin) + rangeHyphen + to_string(sshortMax) );
        } else if ( nBytes == 4 ) {
            unsigned int uintMin = numeric_limits<unsigned int>::min();
            unsigned int uintMax = numeric_limits<unsigned int>::max();
              signed int sintMin = numeric_limits<  signed int>::min();
              signed int sintMax = numeric_limits<  signed int>::max();
            unsignedRange += ( to_string(uintMin) + rangeHyphen + to_string(uintMax) );
              signedRange += ( to_string(sintMin) + rangeHyphen + to_string(sintMax) );
        } else if ( nBytes == 8 ) {
            unsigned long long ullMin = numeric_limits<unsigned long long>::min();
            unsigned long long ullMax = numeric_limits<unsigned long long>::max();
              signed long long sllMin = numeric_limits<  signed long long>::min();
              signed long long sllMax = numeric_limits<  signed long long>::max();
            unsignedRange += ( to_string(ullMin) + rangeHyphen + to_string(ullMax) );
              signedRange += ( to_string(sllMin) + rangeHyphen + to_string(sllMax) );
        } else {
            unsignedRange += ( string("???") + rangeHyphen + string("???") );
              signedRange += ( string("???") + rangeHyphen + string("???") );
        }
    } else {
        int nAllBits = nBytes * s_nBitsPerByte;

        StringNumber uMin("0",10);
        string strumax(nAllBits,'1');
        strumax += "B";
        StringNumber uMax( strumax, 2);
        

        string strSmin = "1";
        strSmin += string(nAllBits-1, '0');
        strSmin += "B";
        StringNumber sMin(strSmin, 2);

        string strSmax = "0";
        strSmax += string(nAllBits-1, '1');
        strSmax += "B";
        StringNumber sMax(strSmax, 2);

            unsignedRange += ( uMin.calcDecimal() + rangeHyphen + uMax.calcDecimal() );
              signedRange += ( sMin.calcDecimalWithSignedFlag() + rangeHyphen + sMax.calcDecimalWithSignedFlag() );


    }

    detail += (octTitle + octalStr        + endline);

    detail += (de1Title + unsignedDecimal + leftParathesis + unsignedRange + rightParathesis + endline);
    detail += (de2Title + signedDecimal   + leftParathesis +   signedRange + rightParathesis + endline);
    detail += (hexTitle + hexStr          + endline);



    ////////////////////////////////////////////////////////////////
    //   "| xxxx xxxx "
    const int nbitsPerHex = 4;
    string splitLine(12 * nBytes + nBitTitle.size() + 10, '-');
    detail += (splitLine + endline);

    detail += nBitTitle;
    int grpCnt = nBytes * 2;
    for( int grpIdx = 0; grpIdx < grpCnt; ++grpIdx )
    {
        int idxBit = nBits -grpIdx * nbitsPerHex;
        string strIdxBit = to_string(idxBit);
        int decBits = static_cast<int>( strIdxBit.size() );
        int makeUpBits = nbitsPerHex - decBits;


        auto isEvenNumber = (grpIdx % 2 == 0);
        if ( isEvenNumber ) {
            detail += prefixBlankSpace;
        } 

        detail += strIdxBit;
        if ( makeUpBits > 0 ) {
            string dynamicSpace(makeUpBits,' ');
            detail += dynamicSpace;
        }
        detail += space;
    }
    detail += (suffixEnd + endline);
    // detail += (string(binTitle.size() ,' '));
    detail += binTitle; 


    for( int grpIdx = 0; grpIdx < grpCnt; ++grpIdx )
    {
        string part1Hex = strcutbB.substr(grpIdx * nbitsPerHex, nbitsPerHex);
        auto isEvenNumber = (grpIdx % 2 == 0);
        if ( isEvenNumber ) {
            detail += prefixBlankSpace;
        } 

        detail += part1Hex;
        detail += space;
    }

    detail += (suffixEnd + endline);
    // detail += (splitLine + endline);

    ////////////////////////////////////////////////////////////////
    // Print Hex Part
    detail += hexTitle;
    hexStr = cut0x(hexStr);
    for( int grpIdx = 0; grpIdx < grpCnt; ++grpIdx )
    {
        string hexCode = hexStr.substr(grpIdx,1);
        auto isEvenNumber = (grpIdx % 2 == 0);
        if ( isEvenNumber ) {
            detail += prefixBlankSpace;
        } 

        detail += (string(3,' '));
        detail += hexCode;
        detail += space;
    }
    detail += (suffixEnd + endline);


    ////////////////////////////////////////////////////////////////
    // Print Octal Part
    detail += octTitle;
    octalStr = cutZero(octalStr);
    int octLen = static_cast<int>( octalStr.size() );

    int remain = nBits % s_2to8Bit;
    int calcCnt = 0;
    for( int i = 0; i < octLen; ++i )
    {
        char ch = octalStr.at(i);
        if ( i == 0 ) {
            detail += prefixBlankSpace;
            if ( remain == 1 ) {
                detail += ch;
                calcCnt += 1;
            } else {
                // remain == 2
                detail += space;
                detail += ch;

                calcCnt += 2;
            }
        } else {
            for( int j = 1; j <= s_2to8Bit; ++j )
            {
                ++calcCnt;

                if ( j == s_2to8Bit ) {
                    detail += ch;
                } else {
                    detail += space;
                }

                if ( calcCnt % s_2to16Bit == 0 ) {
                    if ( calcCnt % s_nBitsPerByte == 0 ) {
                        detail += space;
                        if ( i != octLen-1 ) { 
                            detail += prefixBlankSpace;
                        } 
                    } else {
                        detail += space;
                    }
                }
            }
        }
    }
    detail += (suffixEnd + endline);
    //
    ////////////////////////////////////////////////////////////////

    detail += (splitLine + endline);

    return detail;
}


