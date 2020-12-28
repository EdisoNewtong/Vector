#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
using namespace std;

static const string STR_PART1 = R"(#include "floatNumberConverter.h"
#include <iostream>
#include <string>
using namespace std;

)";

static const string STR_PART2 = R"(union intFloatNum
{
    unsigned int i_num;
    float        f_num;
};

)";


static const string STR_PART3 = R"(union llDoubleNum
{
    unsigned long long i_num;
    double             f_num;
};

)";


static const string STR_PART4 = R"(int main(int argc, char* argv[])
{
    FloatConverter cvt;
    string floatStrNum;
    string errorStr;

    intFloatNum uNum;   

)";


static const string STR_PART5 = R"(int main(int argc, char* argv[])
{
    FloatConverter cvt;
    string floatStrNum;
    string errorStr;

    llDoubleNum uNum;   

)";


static const string STR_PART6 = R"(
    cout << "Processing string float number :  " << floatStrNum << "  | ";
    if ( cvt.isValidFloat(floatStrNum, errorStr) && cvt.doConvert(errorStr) ) {
        const auto& coreObj = cvt.getCvt();
        if ( coreObj.cvtIntBinary == uNum.i_num ) {
            cout << " OK. " << endl;
        } else {
            cout << " Failed. , <Not Equal> " << endl;
            return -1;
        }
    } else {
        cout << errorStr << " | Valid or Convert , Failed.  :-( " << endl;
        return -1;
    }

)";


static const string STR_PART7 = R"(
    cout << "Processing string float number :  " << floatStrNum << "  | ";
    if ( cvt.isValidFloat(floatStrNum, errorStr) && cvt.doConvert(errorStr) ) {
        const auto& coreObj = cvt.getCvt();
        if ( coreObj.cvtLLBinary == uNum.i_num ) {
            cout << " OK. " << endl;
        } else {
            cout << " Failed. , <Not Equal> " << endl;
            return -1;
        }
    } else {
        cout << errorStr << " | Valid or Convert , Failed.  :-( " << endl;
        return -1;
    }

)";



static const string STR_PART_END = R"(

    return 0;
}

)";

string numberTrimmed(const string& numSeq, bool beforeDot)
{
    string retstr;
    if ( numSeq.empty() ) {
        retstr = "0";
        return retstr;
    }

    string::size_type pos = string::npos;

    if ( beforeDot ) {
        pos = numSeq.find_first_not_of('0');
        if ( pos == string::npos ) {
            // all char(s) are '0';
            retstr = "0";
        } else {
            // skip the prefix '0'
            retstr = numSeq.substr(pos);
        }
    } else {
        pos = numSeq.find_last_not_of('0');
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

string generateStrFloat(unsigned long long num, int intBits,int floatBits, bool setNegative, bool fSuffixFlag)
{
    auto requireBits = intBits + floatBits;

    string finalStr;
    string strNum = to_string(num);
    auto sz = strNum.size();
    if ( sz < requireBits ) {
        auto delta = requireBits - sz;
        for ( int i = 0; i < delta; ++i ) {
            finalStr += '0';
        }

        finalStr += strNum;
    } else {
        finalStr = strNum;
    }

    string intPart   = finalStr.substr(0, intBits);
    string floatPart = finalStr.substr(intBits);

    intPart = numberTrimmed(intPart, true);
    floatPart = numberTrimmed(floatPart, false);

    finalStr = "";
    if ( setNegative ) {
        finalStr = "-";
    }

    finalStr += intPart;
    finalStr += ".";
    finalStr += floatPart;

    if ( fSuffixFlag ) {
        finalStr += "f";
    }

    return finalStr;
}


int main(int argc,char* argv[])
{
    if ( argc != 5 ) {
        cout << "Missing generate args " << endl;
        return -1;
    }

    int intBits = atoi(argv[1]);
    if ( intBits <= 0 ) {
        cout << "int bit count must > 0 " << endl;
        return -1;
    }
    int floatBits = atoi(argv[2]);
    if ( floatBits <= 0 ) {
        cout << "floatBits bit count must > 0 " << endl;
        return -1;
    }

    string negativeStr = argv[3];
    string fStr = argv[4];
    auto neFlag = (negativeStr == "ne");
    auto fFlag  = (fStr == "f");

    ofstream outfile("testCase.cpp", ios::out | ios::trunc);
    ofstream outfiletxt("floatNumber.txt", ios::out | ios::trunc);
    if ( !outfile ) {
        cout << "Sorry , Can't create file : testCase.cpp" << endl;
        return -1;
    }

    outfile << STR_PART1;
    if ( fFlag ) {
        // float
        outfile << STR_PART2;
        outfile << STR_PART4;
    } else {
        // double
        outfile << STR_PART3;
        outfile << STR_PART5;
    }


    unsigned long long maxNum = static_cast<unsigned long long>( pow(10, intBits+floatBits) );
    for ( unsigned long long i = 0; i < maxNum; ++i ) {
        auto strRet = generateStrFloat(i, intBits, floatBits, neFlag, fFlag);
        outfile << "    uNum.f_num = " << strRet << ";  " << "floatStrNum = \"" << strRet << "\";  errorStr = \"\";";
        outfiletxt << strRet << endl;
        if ( fFlag ) {
            outfile << STR_PART6;
        } else {
            outfile << STR_PART7;
        }
    }

    outfile << STR_PART_END;
    outfile.flush();
    outfile.close();

    outfiletxt << "#" << endl;
    outfiletxt.flush();
    outfiletxt.close();

    cout << "Generate    <testCase.cpp>      Done " << endl;
    cout << "Generate    <floatNumber.txt>   Done " << endl;

    return 0;
}


