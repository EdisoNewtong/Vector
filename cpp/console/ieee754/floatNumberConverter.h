#ifndef FLOAT_NUMBER_CONVERTER_H
#define FLOAT_NUMBER_CONVERTER_H

#include <list>
#include <string>
using namespace std;

namespace constPart {
    static constexpr int BASE = 2;
    static constexpr char ZERO_ASCII_CODE = '0';
    static constexpr char ONE_ASCII_CODE = '1';

    static constexpr int FLOAT_MAX_FRACTION_BIT  = 23;
    static constexpr int DOUBLE_MAX_FRACTION_BIT = 52;

    static constexpr int FLOAT_EXPOENT_BIAS = 127;      // = 2^(8-1)  - 1 =  127
    static constexpr int DOUBLE_EXPOENT_BIAS = 1023;    // = 2^(11-1) - 1 = 1023

    static constexpr int BINARY_ARRAY_SIZE = 64;
}

namespace HelperUtil
{
    int char2Num(char ch);
    char num2Char(int num);

    string numberTrimmed(const string& numSeq, bool beforeDot);
}

struct floatInfo
{
    string inputString;

    pair<string,string> prefixBlank;     // 1
    pair<string,string> sign1;           // 2
    pair<string,string> numberBeforeDot; // 3
    pair<string,string> dotFlag;         // 4
    pair<string,string> numberAfterDot;  // 5
    pair<string,string> eFlag;           // 6
    pair<string,string> sign2;           // 7
    pair<string,string> numberAfterE;    // 8
    pair<string,string> fFlag;           // 9
    pair<string,string> suffixBlank;     // 10

    string processedString;

    string convertIntPart;
    string convertFloatPart;
    list< pair<string,int> > floatPartCalcList;

    bool   isFloatPartCalcFinished;
    bool   isFloatPartLoop;
    int    loopStartIdx;
    int    loopEndIdx;

    int    power2;
    char   binaryAry[constPart::BINARY_ARRAY_SIZE];

    unsigned int       cvtIntBinary;
    unsigned long long cvtLLBinary;

    floatInfo();
    void reset();
    void resetAry();
};


class FloatConverter
{
public:
    FloatConverter();

    bool isValidFloat(const string& content, string& errorStr);
    bool doConvert(string& errorMsg);
    const floatInfo& getCvt() const;
protected:
    void positiveIntPart2Binary();
    void floatPart2Binary();

    bool m_parseOK;
    
    floatInfo m_floatCvtInfo;
};

#endif
