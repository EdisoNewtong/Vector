#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>
using namespace std;


namespace constPart
{
    static constexpr int BASE = 2;
    static constexpr char ZERO_ASCII_CODE = '0';
}

struct ConvertFloatInfo
{
    static constexpr size_t   BINARY_ARY_SIZE = 64;

    string fullstr;       //  full inputed string

    string strBegSpace;   // \t  
    string strPorN;       // +/-
    string strnumBefore;  //  3.
    string strDot;        // .
    string strnumAfter;   //  .14
    string strFFlag;      //  f/F
    string strEndSpace;   //   \t   \t
    string strCore;       // core part

    size_t nAvalibleBits;
    char binaryAry[BINARY_ARY_SIZE];
    bool bMeetLoopDuringCalc;
    bool bInfiniteCalc;
    size_t loopStart;
    size_t loopEnd;

    vector< pair<string,int> >  calcTmpList;

    void reset() 
    {
        fullstr.clear();
        strBegSpace.clear();
        strPorN.clear();
        strnumBefore.clear();
        strDot.clear();
        strnumAfter.clear();
        strFFlag.clear();
        strEndSpace.clear();
        strCore.clear();

        nAvalibleBits = 0;
        for ( size_t  i = 0; i < BINARY_ARY_SIZE; ++i ) {
            binaryAry[i] = 0;
        }

        bMeetLoopDuringCalc = false;
        bInfiniteCalc = true;
        loopStart = 0;
        loopEnd = 0;

        calcTmpList.clear();
    }

    ConvertFloatInfo() 
    {
        reset();
    }

    ~ConvertFloatInfo() 
    {
        reset();
    }
};



bool isAFloatNumber(const string& originalStr, ConvertFloatInfo* cvtInfo)
{
    bool bret = false;
    if ( cvtInfo == nullptr ) {
        cout << "[ERROR] : Sorry ConvertFloatInfo is a nullptr " << endl;
        return bret;
    }

    try {
        const string spaceGrp = "([ \t]*)";
        const string positiveOrNegative = "([+-]?)";
        const string numberGrp = "([0-9]*)";
        const string dotGrp = "(\\.?)";
        const string fGrp = "([fF]?)";

        //                     "      "     +/-                 3             .       14159265    f/F     "    "
        //                       grp1      grp2                 grp3         grp4     grp5        grp6     grp7
        const string fullexp = spaceGrp + positiveOrNegative + numberGrp +  dotGrp + numberGrp + fGrp + spaceGrp;

        regex floatReg( fullexp.c_str() );
        std::cmatch m;
        bool bMatched = regex_match(originalStr.c_str(), m, floatReg);
        if ( bMatched ) {
            size_t sz = m.size();
            cout << "result : Matched , sz = " << sz << endl;

            if ( sz == 8 ) {
                // m[1].str()   // positive or negative      +/-
                // m[2].str()   // number before    dot       .
                // m[3].str()   //     has dot or not
                // m[4].str()   // number after dot
                
                auto debugFlag = false;
                if ( debugFlag  ) {
                    cout << R"(+/-         :   ")" << m[2].str() << R"(")" << endl;
                    cout << R"(number.     :   ")" << m[3].str() << R"(")" << endl;
                    cout << R"(.           :   ")" << m[4].str() << R"(")" << endl;
                    cout << R"(.number     :   ")" << m[5].str() << R"(")" << endl;
                    cout << R"(.number     :   ")" << m[6].str() << R"(")" << endl;
                }


                // save seperate part of a float number
                cvtInfo->fullstr = originalStr;

                cvtInfo->strBegSpace = m[1].str();
                cvtInfo->strPorN = m[2].str();
                cvtInfo->strnumBefore = m[3].str();
                cvtInfo->strDot = m[4].str();
                cvtInfo->strnumAfter = m[5].str();
                cvtInfo->strFFlag = m[6].str();
                cvtInfo->strEndSpace = m[7].str();

                cvtInfo->strCore = cvtInfo->strPorN 
                                   + cvtInfo->strnumBefore 
                                   + cvtInfo->strDot
                                   + cvtInfo->strnumAfter 
                                   + cvtInfo->strFFlag;

                // It is not allow that there is not number existed in the string
                if ( cvtInfo->strnumBefore.empty() && cvtInfo->strnumAfter.empty() ) {
                    // such kind of string "." / "+" / "-" / "+." / "-." /        is an invalid float number
                    bret = false;
                } else {
                    // OK
                    bret = true;
                }
            } 
        } else {
            cout << "result : not Matched" << endl;
        } 
    } catch ( const regex_error& error ) {
        cout << "[ERROR] : regex is not valid  "  << endl
             << "        what = " << error.what() << endl
             << "        code = " << error.code() << endl;
        bret = false;
    }

    return bret;
}

string positiveIntPart2Binary(const string& decimalstrNum)
{
    using namespace constPart;
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

    string retBinaryStr;
    vector< pair<string,int> > calcProcessLst;
    calcProcessLst.clear();
    
    // fomula = dividend / divisor  = quotient ... remainder
    auto strdividend = decimalstrNum;

    do {
        string strNextQuotient = "";

        auto nQuotient = 0;
        auto nRemainder = 0;
        auto nStrLen = static_cast<int>(strdividend.size());

        for ( auto i = 0; i < nStrLen; ++i ) {
            char ch = strdividend.at(i);
            int nNum = ch - ZERO_ASCII_CODE;
            auto realDividend = nRemainder * 10 + nNum;
            nQuotient = realDividend / BASE;
            nRemainder = realDividend % BASE;
            strNextQuotient += static_cast<char>(ZERO_ASCII_CODE + nQuotient);
        }

        // trim the beginning  '0';
        auto noneZeroPos = strNextQuotient.find_first_not_of(ZERO_ASCII_CODE);
        if ( noneZeroPos != string::npos ) {
            strNextQuotient = strNextQuotient.substr(noneZeroPos);
        } else {
            // all all '0'
            strNextQuotient = "0";
        }
        strdividend = strNextQuotient;
        calcProcessLst.push_back( make_pair(strdividend,  nRemainder) );
    } while( strdividend != "0"  );

    for ( auto it = calcProcessLst.rbegin(); it!= calcProcessLst.rend(); ++it ) {
        retBinaryStr += static_cast<char>( ZERO_ASCII_CODE + it->second);
    }
    
    return retBinaryStr;
}

string floatPart2Binary(ConvertFloatInfo* cvt)
{
    if ( cvt == nullptr ) {
        return string("");
    }

    using namespace constPart;
    string retBinaryStr;

    string strCalcRet = cvt->strnumAfter;
    // clear it first
    // none empty => f => float
    // empty      =>      double
    int nloopCount = 0;
    int loopMaxCnt = cvt->strFFlag.empty() ? 52 : 23;
    while ( true )
    {
        // the only added bit must be 1 , because the biggest bit 9*2 = 18 , 18 is the biggest result among [0-9]*2
        bool needAddtoNextBit = false;
        auto calcMiddleStr = strCalcRet;
        for ( int idx = strCalcRet.size()-1; idx>=0; --idx )
        {
            char ch = strCalcRet.at(idx);
            int num = ch - ZERO_ASCII_CODE;
            num = num * 2 + (needAddtoNextBit ? 1 : 0);
            if ( num >= 10 ) {
                needAddtoNextBit = true;
                num -= 10;
            } else {
                needAddtoNextBit = false;
            }

            calcMiddleStr.at(idx) = ZERO_ASCII_CODE + num;
        }
        ++nloopCount;

        auto leaderBit = needAddtoNextBit ? 1 : 0;
        cvt->calcTmpList.push_back( make_pair(calcMiddleStr,  leaderBit) );

        auto pos = calcMiddleStr.find_first_not_of( ZERO_ASCII_CODE );
        if ( pos == string::npos ) {
            // all bits are '0'
            cvt->bInfiniteCalc = false; // calc successfully, set flag
            break;
        } else {
            auto foundSame = false;
            for ( auto it = cvt->calcTmpList.rbegin(); it!=cvt->calcTmpList.rend(); ++it ) {
                if ( it->first == calcMiddleStr ) {
                    foundSame = true;
                    cvt->loopStart = distance(cvt->calcTmpList.begin() , it.base()) - 1; // - 1 to make sure the index is corrent
                    cvt->loopEnd   = cvt->calcTmpList.size() - 1;

                    break; // jump out of the loop
                }
            }

            if ( foundSame ) {
                cvt->bMeetLoopDuringCalc = true;
                break;
            } else {
                if ( nloopCount >= loopMaxCnt ) {
                    break;
                }
            }
        }

        strCalcRet = calcMiddleStr;
    } // end while

    if( !cvt->bInfiniteCalc ) {
        for ( auto& ele : cvt->calcTmpList ) {
            retBinaryStr += (ZERO_ASCII_CODE + ele.second);
        }
    } else if ( cvt->bMeetLoopDuringCalc ) {
        auto nloop = 0;
        auto hasReachLoopEnd = false;
        do {
            if ( !hasReachLoopEnd ) {
                for ( auto& ele : cvt->calcTmpList ) {
                    retBinaryStr += (ZERO_ASCII_CODE + ele.second);
                }

                hasReachLoopEnd = true;
                nloop += cvt->calcTmpList.size();
            } else {
                for ( auto i = cvt->loopStart; i<=cvt->loopEnd; ++i ) {
                    if ( nloop < loopMaxCnt ) {
                        retBinaryStr += (ZERO_ASCII_CODE + cvt->calcTmpList.at(i).second);
                        ++nloop;
                    } else {
                        break;
                    }
                }
            }
        } while( true );
    } else {
        // forever loop without stop
        for ( auto& ele : cvt->calcTmpList ) {
            retBinaryStr += (ZERO_ASCII_CODE + ele.second);
        }
    }

    return retBinaryStr;
}




bool convert2Binary(ConvertFloatInfo* cvt)
{
    using namespace constPart;
    // 1+8+23  = 32(bits) :  float
    // 1+11+52 = 64(bits) :  double
    if ( cvt == nullptr ) {
        cout << "[ERROR] : Sorry ConvertFloatInfo is a nullptr " << endl;
        return false;
    }

    // string strPorN;       // +/-
    // string strnumBefore;  //  3.   : the number is without  +/-
    // string strDot;        // .
    // string strnumAfter;   //  .14
    // string strFFlag;      //  f/F
    int  sign = 0;
    if ( !cvt->strPorN.empty() ) {
        sign = (cvt->strPorN == "+" ? 1 : -1); // else "-"
    }

    auto hasDot = !cvt->strDot.empty();
    // float/double
    auto hasFFlag = !cvt->strFFlag.empty(); // hasFFlag => float , else the data type is double
    (void)sign;
    (void)hasFFlag;

    if ( !hasDot ) {
        // only interger part 
        // strnumBefore must contain  at least 1 number
        auto pos = cvt->strnumBefore.find_first_not_of(ZERO_ASCII_CODE);
        if ( pos == string::npos ) {
            // all the characters are '0'  , such as '0000000000'
        } else {
            // fount it , maybe   '000123' => 123
            auto strGetRidOfBegin0 =  cvt->strnumBefore.substr(pos);
            auto retIntStrNumBeforeDot = positiveIntPart2Binary(strGetRidOfBegin0);
        }
    } else {
        // has dot ,  there are 3 conditions
        // 1.     xxx.xxx
        // 2.     .xxx
        // 3.     xxx.

        // complete number before if necessary
        if ( cvt->strnumBefore.empty() ) {
            cvt->strnumBefore = "0";
        } else {
            auto pos = cvt->strnumBefore.find_first_not_of( ZERO_ASCII_CODE );
            if ( pos == string::npos ) {
                // all the characters are '0'  , such as '0000000000'
                cvt->strnumBefore = "0";
            } else {
                // trim head of zero
                cvt->strnumBefore = cvt->strnumBefore.substr(pos);
            }
        }

        // complete number after if necessary
        if ( cvt->strnumAfter.empty() ) {
            cvt->strnumAfter = "0";
        } else {
            auto pos = cvt->strnumAfter.find_last_not_of( ZERO_ASCII_CODE );
            if ( pos == string::pos ) {
                // from tail to head , all the characters are '0'  , such as '0000000000'
                cvt->strnumAfter = "0";
            } else {
                // trim end of zero
                cvt->strnumAfter = cvt->strnumAfter.substr(0, pos+1);
            }
        }
    }

    return true;
}


int main(int argc, char* argv[],  char* env[])
{
    string choice;
    ConvertFloatInfo cvt;
    do {
        string strnumber;
        cvt.reset();
        cout << "Please input a string to check if it is a float number or not : ";
        getline(cin, strnumber);

        auto b = isAFloatNumber(strnumber, &cvt);
        cout << "==================================================" << endl;
        if ( b ) {
            cout << "\tOK : \"" << cvt.fullstr << "\" is a valid float " << endl;
            b = convert2Binary(&cvt);
        } else {
            cout << "\tFailed : \"" << cvt.fullstr  << "\" is not a valid float " << endl;
        }
        cout << "==================================================" << endl;

        choice.clear();
        cout << "again (q to Quit) ? ";
        getline(cin, choice);
    } while( choice != "q");

    return 0;
}


