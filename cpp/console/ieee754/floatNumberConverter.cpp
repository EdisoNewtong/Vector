#include "floatNumberConverter.h"

#include <iostream> // Debug Only
#include <sstream>
#include <regex>
#include <iterator>
#include <algorithm>
using namespace std;

namespace HelperUtil
{
    int char2Num(char ch)
    {
        int nNum = ch - constPart::ZERO_ASCII_CODE;
        return nNum;
    }

    char num2Char(int num)
    {
        char ch = constPart::ZERO_ASCII_CODE + num;
        return ch;
    }


    string numberTrimmed(const string& numSeq, bool beforeDot)
    {
        using namespace constPart;

        string retstr;
        if ( numSeq.empty() ) {
            retstr = "0";
            return retstr;
        }

        string::size_type pos = string::npos;

        if ( beforeDot ) {
            pos = numSeq.find_first_not_of(ZERO_ASCII_CODE);
            if ( pos == string::npos ) {
                // all char(s) are '0';
                retstr = "0";
            } else {
                // skip the prefix '0'
                retstr = numSeq.substr(pos);
            }
        } else {
            pos = numSeq.find_last_not_of(ZERO_ASCII_CODE);
            if ( pos == string::npos ) {
                // all char(s) are '0';
                retstr = "0"; // merge a lot of '0' to one '0'
            } else {
                // trim the tail '0'
                retstr = numSeq.substr(0, pos+1);
            }
        }

        return retstr;
    }
}


floatInfo::floatInfo()
{
    reset();
}






void floatInfo::reset()
{

    inputString = "";

    prefixBlank.first = prefixBlank.second = "";
    sign1.first = sign1.second = "";
    numberBeforeDot.first = numberBeforeDot.second = "";
    dotFlag.first = dotFlag.second = "";
    numberAfterDot.first = numberAfterDot.second = "";
    eFlag.first = eFlag.second = "";
    sign2.first = sign2.second = "";
    numberAfterE.first = numberAfterE.second = "";
    fFlag.first = fFlag.second = "";
    suffixBlank.first = suffixBlank.second = "";

    processedString = "";
    convertIntPart = "";
    convertFloatPart = "";
    floatPartCalcList.clear();

    isFloatPartCalcFinished = false;
    isFloatPartLoop = false;
    loopStartIdx = 0;
    loopEndIdx = 0;
    power2 = 0;

    cvtIntBinary = 0U;
    cvtLLBinary = 0UL;

    resetAry();
}


void floatInfo::resetAry()
{
    for( int i = 0; i < constPart::BINARY_ARRAY_SIZE; ++i) {
        binaryAry[i] = '0';
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////

FloatConverter::FloatConverter()
    : m_parseOK(false)
{
    
} 


/*

######################################################################
       Standard float representation
######################################################################

   e.g.

   "        -123.456e-789f             "
   | 1    |2| 3 |4| 5 |6|7| 8 |9|  10  |
   |      |-|123|.|456|e|-|789|f|      |

======================================================================
      Group Descrption
======================================================================

     1.   prefix   blank                  : "     "
     2.   positive/negative symbol        : +/-   or      if omitted (means  + )
     3.   number sequence 1 before   dot  : 123
     4.   dot                             : .
     5.   number sequence 2 after    dot  : 456
     6.   e flag                          : e|E
     7.   positive/negative symbol        : +/-   or      if omitted (means  + )
     8.   10 ^ number                     : 789
     9.   float suffix flag               : f/F
     10.  suffix blank                    : "     "

======================================================================

*/
bool FloatConverter::isValidFloat(const string& content, string& errorStr)
{
    using namespace constPart;
    using namespace HelperUtil;
    stringstream ssErrorStr;
    m_parseOK = false;

    m_floatCvtInfo.reset();

    string numSeq = "([0-9]*)";
    string blank = "([ \t]*)";
    string flagSign = "([+-]?)";  

    string dot = "(\\.?)";
    string flagE = "([eE]?)";
    string flagF = "([fF]?)";

    // "        -123.456e-789f                  "

    std::cmatch m;
    try {
        std::regex floatReg(  blank     // 1.  prefix blank "     "
                           +  flagSign  // 2.  -    (+/-)
                           +  numSeq    // 3.  123
                           +  dot       // 4.  .
                           +  numSeq    // 5.  456
                           +  flagE     // 6.  e    (e/E)
                           +  flagSign  // 7.  -    (+/-)
                           +  numSeq    // 8.  789
                           +  flagF     // 9.  f    (f/F)
                           +  blank);   // 10. suffix blank "     "

        bool matched = regex_match(content.c_str(), m, floatReg);
        if ( !matched ) {
            ssErrorStr << "[ERROR] : current string : \"" << content <<  "\" doesn't matched  Float number format" << endl;
            errorStr = ssErrorStr.str();
            return false;
        }
    } catch ( const std::regex_error& e  ) {
        ssErrorStr << "[ERROR] : regex_error = " << e.what() << endl;
        errorStr = ssErrorStr.str();
        return false;
    } catch ( ... ) {
        ssErrorStr << "[ERROR] : catch(...) = " << endl;
        errorStr = ssErrorStr.str();
        return false;
    }


    // Check string part before flag    e/E
    if ( m[4].str().empty() ) {
        // No dot
        if ( m[3].str().empty() ) {
            ssErrorStr << "[ERROR] : No dot found , there must be a number sequence before the Dot position" << endl;
            errorStr = ssErrorStr.str();
            return false;
        }
    } else {
        // has dot
        if ( m[3].str().empty() && m[5].str().empty() ) {
            ssErrorStr << "[ERROR] : dot existed , But there is neither number sequence before dot nor number sequence after Dot" << endl;
            errorStr = ssErrorStr.str();
            return false;
        }
    }


    // Check string part after flag   e/E
    if ( m[6].str().empty() ) {
        // No e/E 
        if ( !m[7].str().empty()    ||   !m[8].str().empty() ) {
            ssErrorStr << "[ERROR] : No e|E flag found , But some invalid suffix was found " << endl;
            errorStr = ssErrorStr.str();
            return false;
        }
    } else {
        // e/E found
        if ( m[8].str().empty() ) {
            ssErrorStr << "[ERROR] : e|E flag found , But no 10^??? Power number sequence was found " << endl;
            errorStr = ssErrorStr.str();
            return false;
        }
    }

    //
    // fill data into struct
    //
    m_floatCvtInfo.inputString = content;

    m_floatCvtInfo.prefixBlank.first = m_floatCvtInfo.prefixBlank.second = m[1].str();
    m_floatCvtInfo.sign1.first = m_floatCvtInfo.sign1.second = m[2].str();
    if ( m_floatCvtInfo.sign1.second.empty() ) {
        m_floatCvtInfo.sign1.second = "+";
    }

    // prepare main float number sequence
    if ( m[4].str().empty() ) {
        // No Dot
        m_floatCvtInfo.numberBeforeDot.first = m_floatCvtInfo.numberBeforeDot.second = m[3].str();
        m_floatCvtInfo.numberBeforeDot.second = numberTrimmed(m_floatCvtInfo.numberBeforeDot.second, true);

        m_floatCvtInfo.dotFlag.first = "";
        m_floatCvtInfo.dotFlag.second = ".";

        m_floatCvtInfo.numberAfterDot.first = "";
        m_floatCvtInfo.numberAfterDot.second = "0";
    } else {
        // Has Dot
        m_floatCvtInfo.numberBeforeDot.first = m_floatCvtInfo.numberBeforeDot.second = m[3].str();
        m_floatCvtInfo.numberBeforeDot.second = numberTrimmed(m_floatCvtInfo.numberBeforeDot.second, true);

        m_floatCvtInfo.dotFlag.first = m_floatCvtInfo.dotFlag.second = m[4].str();

        m_floatCvtInfo.numberAfterDot.first = m_floatCvtInfo.numberAfterDot.second = m[5].str();
        m_floatCvtInfo.numberAfterDot.second = numberTrimmed(m_floatCvtInfo.numberAfterDot.second, false);
    }

    // prepare suffix e/E sequence
    m_floatCvtInfo.eFlag.first = m_floatCvtInfo.eFlag.second = m[6].str();
    m_floatCvtInfo.sign2.first = m_floatCvtInfo.sign2.second = m[7].str();
    if ( !m_floatCvtInfo.eFlag.first.empty() ) {
        if ( m_floatCvtInfo.sign2.second.empty() ) {
            m_floatCvtInfo.sign2.second = "+";
        }
    }

    m_floatCvtInfo.numberAfterE.first = m_floatCvtInfo.numberAfterE.second = m[8].str();
    m_floatCvtInfo.numberAfterE.second = numberTrimmed(m_floatCvtInfo.numberAfterE.second, true);

    m_floatCvtInfo.fFlag.first        = m_floatCvtInfo.fFlag.second = m[9].str();
    m_floatCvtInfo.suffixBlank.first  = m_floatCvtInfo.suffixBlank.second = m[10].str();

    //
    // Final Process
    //
    string headerFloatWithoutSign =   m_floatCvtInfo.numberBeforeDot.second
                                    + m_floatCvtInfo.dotFlag.second
                                    + m_floatCvtInfo.numberAfterDot.second;

    if ( m_floatCvtInfo.eFlag.first.empty() ) {
        // add sign
        m_floatCvtInfo.processedString = m_floatCvtInfo.sign1.second + headerFloatWithoutSign;
    } else {
        // has E flag
        int integerPartLen = static_cast<int>( m_floatCvtInfo.numberBeforeDot.second.size() );
        int floatPartLen   = static_cast<int>( m_floatCvtInfo.numberAfterDot.second.size() );
        string processNumBeforeDot;
        string processNumAftereDot;

        if ( m_floatCvtInfo.numberAfterE.second == "0" ) {
            // 10^0     => do nothing
            m_floatCvtInfo.processedString = m_floatCvtInfo.sign1.second + headerFloatWithoutSign;
        } else {
            int nMoveStep = atoi( m_floatCvtInfo.numberAfterE.second.c_str() );

            if ( m_floatCvtInfo.sign2.second == "-" ) {
                // dot shift    Left
                if ( nMoveStep < integerPartLen ) {
                    auto delta = integerPartLen - nMoveStep;
                    processNumBeforeDot = m_floatCvtInfo.numberBeforeDot.second.substr(0, delta);
                    processNumAftereDot = m_floatCvtInfo.numberBeforeDot.second.substr(delta) + m_floatCvtInfo.numberAfterDot.second; 
                } else {
                    // nMoveStep >= integerPartLen
                    auto prefixZeroCount = nMoveStep - integerPartLen + 1;
                    if ( prefixZeroCount == 1 ) {
                        processNumBeforeDot = "0";
                        processNumAftereDot = m_floatCvtInfo.numberBeforeDot.second + m_floatCvtInfo.numberAfterDot.second;
                    } else {
                        // >= 2
                        string prefixZeroStr(prefixZeroCount-1, ZERO_ASCII_CODE);
                        processNumBeforeDot = "0";
                        processNumAftereDot = prefixZeroStr + m_floatCvtInfo.numberBeforeDot.second + m_floatCvtInfo.numberAfterDot.second;
                    }
                }
            } else {
                // +   dot shift    Right
                if ( nMoveStep < floatPartLen ) {
                    processNumBeforeDot = m_floatCvtInfo.numberBeforeDot.second + m_floatCvtInfo.numberAfterDot.second.substr(0,nMoveStep);
                    // may be trim the head '0'
                    processNumAftereDot = m_floatCvtInfo.numberAfterDot.second.substr(nMoveStep);
                } else {
                    // nMoveStep >= floatPartLen
                    if ( nMoveStep == floatPartLen ) {
                        processNumBeforeDot = m_floatCvtInfo.numberBeforeDot.second + m_floatCvtInfo.numberAfterDot.second;
                        // may be trim the head '0'
                        processNumAftereDot = "0";
                    } else {
                        // nMoveStep > floatPartLen
                        string suffixZeroStr(nMoveStep - floatPartLen , ZERO_ASCII_CODE);
                        processNumBeforeDot = m_floatCvtInfo.numberBeforeDot.second + m_floatCvtInfo.numberAfterDot.second + suffixZeroStr;
                        // may be trim the head '0'
                        processNumAftereDot = "0";
                    }
                }
            }

            m_floatCvtInfo.processedString = m_floatCvtInfo.sign1.second + processNumBeforeDot + m_floatCvtInfo.dotFlag.second + processNumAftereDot;
            // trimmed unused '0'
            m_floatCvtInfo.numberBeforeDot.second = numberTrimmed(processNumBeforeDot, true);
            m_floatCvtInfo.numberAfterDot.second = numberTrimmed(processNumAftereDot, false);
        }
    }
    // everything is OK

    m_parseOK = true;
    return true;
}



void FloatConverter::positiveIntPart2Binary()
{
    m_floatCvtInfo.convertIntPart = "";
    if ( m_floatCvtInfo.numberBeforeDot.second == "0" ) {
        m_floatCvtInfo.convertIntPart = "0";
        return;
    }

    using namespace constPart;
    using namespace HelperUtil;


    // cout << "calc num : " << decimalstrNum << endl;
    //------------------------------------
    // e.g.
    //          67
    //------------------------------------
    //          67/2 = 33 ... 1
    //          33/2 = 16 ... 1
    //          16/2 = 8  ... 0
    //           8/2 = 4  ... 0
    //           4/2 = 2  ... 0
    //           2/2 = 1  ... 0
    //           1/2 = 0  ... 1
    /////////////////////////////////
    //
    // 67(10) = 0100 0011(2)
    //
    /////////////////////////////////

    // why use pair<string,int>  rather than pair<int,int>
    // because the Quotient might be a very big interger whose range is out of  <unsigned long long>
    list< pair<string,int> > calcProcessLst;
    calcProcessLst.clear();
    
    // fomula = dividend / divisor  = quotient ... remainder
    auto strdividend = m_floatCvtInfo.numberBeforeDot.second;

    do {
        string strNextQuotient = "";

        auto nQuotient = 0;
        auto nRemainder = 0;
        auto nStrLen = static_cast<int>(strdividend.size());

        for ( auto i = 0; i < nStrLen; ++i ) {
            char ch = strdividend.at(i);
            int nNum = char2Num(ch);
            auto realDividend = nRemainder * 10 + nNum;
            nQuotient = realDividend / BASE;
            nRemainder = realDividend % BASE;
            strNextQuotient += num2Char(nQuotient);
        }

        // trim the beginning  '0' , such as  100/2 = 050 , trim the leader '0'
        strNextQuotient = numberTrimmed(strNextQuotient,true);

        strdividend = strNextQuotient;
        calcProcessLst.push_back( make_pair(strdividend,  nRemainder) );
    } while( strdividend != "0"  );

    // finally : collect the reminder in reverse order
    for ( auto it = calcProcessLst.rbegin(); it!= calcProcessLst.rend(); ++it ) {
        m_floatCvtInfo.convertIntPart += num2Char(it->second);
    }

}


/*
e.g.
----------------------------------------------------------------------------------------------------
                                int part
                            
   0.8125 * 2 = 1.6250     ==>     1
   0.6250 * 2 = 1.2500     ==>     1
   0.25   * 2 = 0.5        ==>     0
   0.5    * 2 = 1.0        ==>     1  ,    number seq after .   is  0 , <Done>

   0.8125(10) => 0.1101(2)
----------------------------------------------------------------------------------------------------
  0.625 * 2 = 1.25         ==>     1
  0.25  * 2 = 0.5          ==>     0
  0.5   * 2 = 1.0          ==>     1
  0.625(10)  => 0.101(2)
----------------------------------------------------------------------------------------------------

    infinate loop example.
    
  0.7   * 2 = 1.4          ==>     1
  #########################################
   0.4   * 2 = 0.8          ==>     0
   0.8   * 2 = 1.6          ==>     1
   0.6   * 2 = 1.2          ==>     1
   0.2   * 2 = 0.4          ==>     0
  #########################################
   0.4   * 2 = 0.8          ==>     0
   0.8   * 2 = 1.6          ==>     1
   0.6   * 2 = 1.2          ==>     1
   0.2   * 2 = 0.4          ==>     0
  #########################################

  0.7(10) = 0.1|0110|0110|(loop part  is #0110#)   (2)

*/
void FloatConverter::floatPart2Binary()
{
    using namespace constPart;
    using namespace HelperUtil;

    // float  : 1 +  8 + 23 = 32      =>   8 bits (0~255) or (-128 ~ +127)    , realpow + 127 
    // double : 1 + 11 + 52 = 64
    //--------------------------------------------------
    // expoent bias = 2^(e-1) - 1
    // e.g.
    //       float  :  2^(8-1)  - 1 = 2^7  - 1 = 127
    //       double :  2^(11-1) - 1 = 2^10 - 1 = 1023
    //--------------------------------------------------
    auto maxCount = 0;
    if ( !m_floatCvtInfo.fFlag.second.empty() ) {
        // has f|F  suffix flag
        maxCount = FLOAT_MAX_FRACTION_BIT;
    } else {
        // no f|F  suffix flag , double type
        maxCount = DOUBLE_MAX_FRACTION_BIT;
    }
    maxCount *= 2; // for movement condition     

    m_floatCvtInfo.convertFloatPart = "";
    m_floatCvtInfo.floatPartCalcList.clear();
    if ( m_floatCvtInfo.numberAfterDot.second == "0" ) {
        m_floatCvtInfo.isFloatPartCalcFinished = true;
        m_floatCvtInfo.isFloatPartLoop = false;
        for( int i = 0; i < maxCount; ++i ) {
            m_floatCvtInfo.convertFloatPart += ZERO_ASCII_CODE;
        }
        return;
    }


    string multiplier = m_floatCvtInfo.numberAfterDot.second;
    // why use pair<string,int>  rather than pair<int,int>
    // because the Quotient might be a very big interger whose range is out of  <unsigned long long>
    list< pair<string,int> > calcProcessLst;
    list< pair<string,int> > loopLst;
    calcProcessLst.clear();

    while( true ) {
        int len = static_cast<int>( multiplier.size() );

        list<int> bitsArray;
        bitsArray.clear();

        int additive = 0;
        for ( int i = len-1; i>=0; --i ) {
            char ch = multiplier.at(i);
            int nNum = char2Num(ch);
            auto mulRet = nNum * BASE + additive;

            additive = mulRet / 10;
            auto reminder = mulRet % 10;
            bitsArray.push_front(reminder);
        }

        string strConjNumber;
        for ( const auto& nBit : bitsArray ) {
            char ch = num2Char(nBit);
            strConjNumber += ch;
        }

        strConjNumber = numberTrimmed(strConjNumber, false);
        multiplier = strConjNumber;

        auto needSkip = false;
        auto loopFlag = false;

        if ( multiplier == "0" ) {
            // m_floatCvtInfo.convertFloatPart
            m_floatCvtInfo.isFloatPartCalcFinished = true;
            calcProcessLst.push_back( make_pair(multiplier,additive) );
            m_floatCvtInfo.floatPartCalcList.push_back( make_pair(multiplier,additive) );
            needSkip = true;
        } else if ( !calcProcessLst.empty() ) {

            auto loopStart = 0;
            auto fIt = calcProcessLst.begin();
            // auto loopEnd   = 0;
            for ( auto it = calcProcessLst.rbegin(); it!=calcProcessLst.rend(); ++it ) {
                if ( it->first == multiplier ) {
                    fIt = it.base();    // it pointer to the number after real pointer
                    loopStart = static_cast<int>( std::distance(calcProcessLst.begin(), fIt) );
                    loopFlag = true;
                    break;
                }
            }

            if ( loopFlag ) {
                calcProcessLst.push_back( make_pair(multiplier,additive) );
                m_floatCvtInfo.floatPartCalcList.push_back( make_pair(multiplier,additive) );

                m_floatCvtInfo.isFloatPartLoop = true;
                m_floatCvtInfo.loopStartIdx = loopStart;
                m_floatCvtInfo.loopEndIdx = calcProcessLst.size() - 1;
                
                loopLst.clear();
                auto sz = m_floatCvtInfo.loopEndIdx - m_floatCvtInfo.loopStartIdx + 1;
                loopLst.resize( sz);

                auto it1 = calcProcessLst.begin();
                std::advance(it1, loopStart);
                std::copy_n(it1, sz, loopLst.begin() );
                
                needSkip = true;
            } else {
                calcProcessLst.push_back( make_pair(multiplier,additive) );
                m_floatCvtInfo.floatPartCalcList.push_back( make_pair(multiplier,additive) );
                if ( static_cast<int>( calcProcessLst.size() ) == maxCount ) {
                    needSkip = true;
                }
            }
        } else {
            // calcProcessLst is empty()
            calcProcessLst.push_back( make_pair(multiplier,additive) );
            m_floatCvtInfo.floatPartCalcList.push_back( make_pair(multiplier,additive) );
        }

        if ( needSkip ) {
            // set convert Float
            for ( const auto& pr : calcProcessLst ) {
                m_floatCvtInfo.convertFloatPart += num2Char(pr.second);
            }

            // add '0' at the end of the string to fill the full bit
            int nCalcIdx = 0;
            while ( static_cast<int>( m_floatCvtInfo.convertFloatPart.size() ) < maxCount ) {
                if ( !loopFlag ) {
                    m_floatCvtInfo.convertFloatPart += "0";
                } else {
                    auto curIt = loopLst.begin();
                    std::advance(curIt,nCalcIdx);
                    m_floatCvtInfo.convertFloatPart += num2Char(curIt->second);

                    ++nCalcIdx;
                    if ( nCalcIdx >= static_cast<int>( loopLst.size() ) ) {
                       nCalcIdx = 0; 
                    }
                }
            }
            // cout << "m_floatCvtInfo.convertFloatPart = "  << m_floatCvtInfo.convertFloatPart << endl;

            break;
        }
    }
}



bool FloatConverter::doConvert(string& errorMsg)
{
    using namespace constPart;
    auto bRet = false;
    stringstream ssErrorStr;

    if ( m_parseOK ) {
        // cout << "m_parseOK  : OK" << endl;
        positiveIntPart2Binary();
        // cout << "m_floatCvtInfo.convertIntPart = " << m_floatCvtInfo.convertIntPart << endl;
        floatPart2Binary();
        // cout << "m_floatCvtInfo.convertFloatPart = " << m_floatCvtInfo.convertFloatPart << endl;

        auto    allFractionBits = !m_floatCvtInfo.fFlag.second.empty() ? FLOAT_MAX_FRACTION_BIT : DOUBLE_MAX_FRACTION_BIT;
        auto    condition = 0;
        unsigned int exponent = 0U;
        auto    exponent_bias = !m_floatCvtInfo.fFlag.second.empty() ? FLOAT_EXPOENT_BIAS : DOUBLE_EXPOENT_BIAS;
        string intValidBitsString;
        string floatValidBitsString;
        string fractionPartString;

        auto pos1 = string::npos;
        auto pos2 = string::npos;
        pos1 = m_floatCvtInfo.convertIntPart.find(ONE_ASCII_CODE);
        if ( pos1 == string::npos ) {
            // int part's bits are all '0' , find 1st valid bit from m_floatCvtInfo.convertFloatPart
            condition = 1;
            intValidBitsString = "";
        } else {
            if ( (pos1+1) < m_floatCvtInfo.convertIntPart.size() ) {
                intValidBitsString = m_floatCvtInfo.convertIntPart.substr(pos1+1);
            } else {
                intValidBitsString = "";
            }
            condition = 2;
        }

        /*
            0.36 * 2 = 0.72 | 0 <--
            0.72 * 2 = 1.44 | 1
            0.44 * 2 = 0.88 | 0
            0.88 * 2 = 1.76 | 1
            0.76 * 2 = 1.52 | 1
            0.52 * 2 = 1.04 | 1
            0.04 * 2 = 0.08 | 0 
            0.08 * 2 = 0.16 | 0 
            0.16 * 2 = 0.32 | 0 
            0.32 * 2 = 0.64 | 0
            0.64 * 2 = 1.28 | 1
            0.28 * 2 = 0.56 | 0
            0.56 * 2 = 1.12 | 1
            0.12 * 2 = 0.24 | 0
            0.24 * 2 = 0.48 | 0
            0.48 * 2 = 0.96 | 0
            0.96 * 2 = 1.92 | 1
            0.92 * 2 = 1.84 | 1
            0.84 * 2 = 1.68 | 1
            0.68 * 2 = 1.36 | 1 <--
            0.36 * 2 = 0.72 | 0 <--
            0.72 * 2 = 1.44 | 1
            0.44 * 2 = 0.88 | 0
            0.88 * 2 = 1.76 | 1
            0.76 * 2 = 1.52 | 1
            0.52 * 2 = 1.04 | 1
            0.04 * 2 = 0.08 | 0 
            0.08 * 2 = 0.16 | 0 
            0.16 * 2 = 0.32 | 0 
            0.32 * 2 = 0.64 | 0
            0.64 * 2 = 1.28 | 1
            0.28 * 2 = 0.56 | 0
            0.56 * 2 = 1.12 | 1
            0.12 * 2 = 0.24 | 0
            0.24 * 2 = 0.48 | 0
            0.48 * 2 = 0.96 | 0
            0.96 * 2 = 1.92 | 1
            0.92 * 2 = 1.84 | 1
            0.84 * 2 = 1.68 | 1
            0.68 * 2 = 1.36 | 1 <--

            0.36 
                  = 0.01011100001010001111|01011100001010001111|01011100001010001111
            = 2^(-2)* 1.0111000010100011110101110000101000111101011100001010001111           

fraction:
  123456789012345678901234
=>0111000010100011110101110000101000111101011100001010001111
because float use 23 bits fraction , so calculate !!<valid>!!! fractions part for the 24th bit
if the 24th bit is 1 , then increase to 23th

  123456789012345678901234
= 0111000010100011110101110000101000111101011100001010001111
->01110000101000111101100

----------------------------------------------------------------------------------------------------

            0.05 * 2 = 0.1 | 0
            0.1  * 2 = 0.2 | 0 
            0.2  * 2 = 0.4 | 0 <--
            0.4  * 2 = 0.8 | 0
            0.8  * 2 = 1.6 | 1
            0.6  * 2 = 1.2 | 1 <--
            0.2  * 2 = 0.4 | 0 <--
            0.4  * 2 = 0.8 | 0
            0.8  * 2 = 1.6 | 1
            0.6  * 2 = 1.2 | 1 <--

            0.05
                 =    0.0000110011001100110011001100110011
                 = 2^(-5)* 1.1001100
fraction:
  123456789012345678901234
=>100110011001100110011001100110011

because float use 23 bits fraction , so calculate !!<valid>!!! fractions part for the 24th bit
if the 24th bit is 1 , then increase to 23th

  123456789012345678901234
= 100110011001100110011001100110011
->10011001100110011001101

        */

        if ( condition == 1 ) {
            pos2 = m_floatCvtInfo.convertFloatPart.find(ONE_ASCII_CODE);
            if ( pos2 == string::npos ) {
                // number = 0.000000
                // all float bits are '0'  && all int bits are '0'
                for( int i = 0; i < allFractionBits; ++i ) {
                    floatValidBitsString += ZERO_ASCII_CODE;
                }
                exponent = 0U;
            } else {
                // pos2 != string::npos
                // 1. split all valid bits
                floatValidBitsString = m_floatCvtInfo.convertFloatPart.substr(pos2+1);
                // 2. split to size allFractionBits , trimmed other tail bits
                floatValidBitsString = floatValidBitsString.substr(0,allFractionBits + 1);
                exponent = (-1) * static_cast<int>(pos2+1) + exponent_bias;

                if ( floatValidBitsString.back() == ZERO_ASCII_CODE ) {
                    floatValidBitsString = floatValidBitsString.substr(0,allFractionBits);
                } else {
                    // back == '1' ,    + '1' to previous bit
                    for ( int i = floatValidBitsString.size()-2; i>=0; --i ) {
                        auto ch = floatValidBitsString.at(i);
                        if ( ch == ZERO_ASCII_CODE ) {
                            floatValidBitsString.at(i) = ONE_ASCII_CODE;
                            break;
                        } else {
                            floatValidBitsString.at(i) = ZERO_ASCII_CODE;
                        }
                    }
                    floatValidBitsString = floatValidBitsString.substr(0,allFractionBits);
                }
            }
        } else {
            // condition = 2
            int rest = allFractionBits - intValidBitsString.size();
            exponent = static_cast<int>(intValidBitsString.size()) + exponent_bias;

            if ( rest < 0 ) {
                ssErrorStr << "int part is out of max fraction part length , len = " << intValidBitsString.size();
                errorMsg = ssErrorStr.str();
                return false;
            } else if ( rest == 0 ) {
                // float valid bits are all ignored
                floatValidBitsString = "";
            } else {
                // cout << "convertFloatPart = " << m_floatCvtInfo.convertFloatPart << endl;
                floatValidBitsString = m_floatCvtInfo.convertFloatPart.substr(0, rest+1);
                if ( floatValidBitsString.back() == ZERO_ASCII_CODE ) {
                    floatValidBitsString = m_floatCvtInfo.convertFloatPart.substr(0, rest);
                } else {
                    // floatValidBitsString.back() == '1' ,     + '1' to previous bit
                    for ( int i = floatValidBitsString.size()-2; i>=0; --i ) {
                        auto ch = floatValidBitsString.at(i);
                        if ( ch == ZERO_ASCII_CODE ) {
                            floatValidBitsString.at(i) = ONE_ASCII_CODE;
                            break;
                        } else {
                            floatValidBitsString.at(i) = ZERO_ASCII_CODE;
                        }
                    }
                    floatValidBitsString = floatValidBitsString.substr(0,rest);
                }
            }
        }


        fractionPartString = intValidBitsString + floatValidBitsString;
        // cout << "fractionPartString = " << fractionPartString << endl;

        // fill all bits
        m_floatCvtInfo.resetAry();
        int maxSz = 0;
        if ( !m_floatCvtInfo.fFlag.second.empty() ) {
            maxSz = sizeof(float) * 8;
            // float  : 1 +  8 + 23 = 32      =>   8 bits (0~255) or (-128 ~ +127)    , realpow + 127 
            m_floatCvtInfo.binaryAry[0] = (m_floatCvtInfo.sign1.second == "-") ? ONE_ASCII_CODE : ZERO_ASCII_CODE;
            for ( int i = 1; i < 9; ++i ) {
                // exponent
                m_floatCvtInfo.binaryAry[i] = ( ((exponent >> (8-i)) & 0x1U) == 0x1U) ? ONE_ASCII_CODE : ZERO_ASCII_CODE;
            }

            int idx = 0;
            for ( int i = 9; i < maxSz; ++i, ++idx ) {
                m_floatCvtInfo.binaryAry[i] = fractionPartString.at(idx);
            }

            m_floatCvtInfo.cvtIntBinary = 0U;
            for ( int i = 0; i < maxSz; ++i ) {
                auto bit = m_floatCvtInfo.binaryAry[i];
                if ( bit == ONE_ASCII_CODE ) {
                    unsigned int mask = 1 << (maxSz-i-1);
                    m_floatCvtInfo.cvtIntBinary |= mask;
                }
            }

        } else {
            maxSz = sizeof(double) * 8;
            // double : 1 + 11 + 52 = 64
            m_floatCvtInfo.binaryAry[0] = (m_floatCvtInfo.sign1.second == "-") ? ONE_ASCII_CODE : ZERO_ASCII_CODE;
            // cout << "exponent = " << exponent << endl;
            for( int i = 1; i < 12; ++i ) {
                m_floatCvtInfo.binaryAry[i] = ( ((exponent >> (11-i)) & 0x1U) == 0x1U) ? ONE_ASCII_CODE : ZERO_ASCII_CODE;
            }

            int idx = 0;
            for ( int i = 12; i < maxSz; ++i, ++idx ) {
                m_floatCvtInfo.binaryAry[i] = fractionPartString.at(idx);
            }


            m_floatCvtInfo.cvtLLBinary  = 0UL;
            for ( unsigned long long i = 0; i < static_cast<unsigned long long>(maxSz); ++i ) {
                auto bit = m_floatCvtInfo.binaryAry[i];
                if ( bit == ONE_ASCII_CODE ) {
                    // unsigned long long mask = 1UL << (maxSz-i-1);
                    unsigned long long mask = 1UL << (maxSz-i-1);
                    m_floatCvtInfo.cvtLLBinary |= mask;
                }
            }
        }

        bRet = true;
    } else {

        bRet = false;
        ssErrorStr << "string to parse is not a valid float format string";
        errorMsg = ssErrorStr.str();
    }

    return bRet;
}

const floatInfo& FloatConverter::getCvt() const
{
    return m_floatCvtInfo;
}


