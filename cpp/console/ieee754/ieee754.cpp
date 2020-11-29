#include <iostream>
#include <string>
#include <list>
#include <map>
#include <regex>
using namespace std;



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
    bool bMeetLoop;
    size_t loopStart;
    size_t loopEnd;

    map<string,int>  calcTmpMap;

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

        bMeetLoop = false;
        loopStart = 0;
        loopEnd = 0;

        calcTmpMap.clear();
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

string positiveIntpart2binary(const string& decimalstrNum)
{
    //------------------------------------
    // 1011(2) = 8 + 3 = 11(10) = B
    // 11
    //------------------------------------
    // 11/2 = 5 ... 1
    // 5 /2 = 2 ... 1
    // 2 /2 = 1 ... 0
    // 1 /2 = 0 ... 1

    list< pair<string,int> > calcProcessLst;
    calcProcessLst.clear();
    
    auto dividend = decimalstrNum;
    auto quotient = 0;
    do {

        auto hasPreFlag = false;
        auto previousNum = 0;

        auto num2 = 0;
        auto quotient = 0;
        auto mod = 0;

        // calc = quotient 
        for ( auto ch : dividend  ) {
            int num = ch - '0';
            if ( hasPreFlag ) {
                num2 = previousNum * 10 + num;
                quotient = num / 2;
                mod = num % 2;
                // calcProcessLst.push_front( make_pair() );
            } else {
                // not preFlag
                if ( num < 2 ) {
                    hasPreFlag = true;
                    previousNum = num;
                    continue;
                } else {
                    auto  quotient = num / 2;
                    auto  mod = num % 2;
                    
                }
            }
        }


    } while( quotient != 0  );


    return "";
}


bool convertFloatIntoBinary(ConvertFloatInfo* cvt)
{
    // 1+8+23  = 32(bits) :  float
    // 1+11+52 = 64(bits) :  double
    
    // 1.1...1 ( there is one 1 and 23 1     (not 22 bits 1)   )
    /*    
    
    127-23 = 104

    24-bits     104-bits
    1...1        0...0    

    128 bits

    FFF
    */
    

    // [ -128 ~  0 ~ 127 ]
    // 1.1...1 (23bits1)
    

    // float : [ 1.17549e-38 ~ 3.40282e+38 ]


    // 2^8 = 256    -128 ~ +127
    
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
    auto hasFFlag = !cvt->strFFlag.empty(); // hasFFlag => float , else the data type is double
    (void)sign;
    (void)hasFFlag;

    if ( !hasDot ) {
        // only interger part 
        // strnumBefore must contain  at least 1 number
        auto pos = cvt->strnumBefore.find_first_not_of('0');
        if ( pos == string::npos ) {
            // all is '0'  , such as '0000000000'
        } else {
            // fount it , maybe   '000123' => 123
            auto strgetridofbegin0 =  cvt->strnumBefore.substr(pos);
        }
    } else {

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
            b = convertFloatIntoBinary(&cvt);
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
