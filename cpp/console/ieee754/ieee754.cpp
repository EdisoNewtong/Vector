#include "floatNumberConverter.h"

#include <iostream>
#include <fstream>
using namespace std;

union intFloatNum
{
    unsigned int i_num;
    float        f_num;
};

union llDoubleNum
{
    unsigned long long i_num;
    double             f_num;
};

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

    intPart = HelperUtil::numberTrimmed(intPart, true);
    floatPart = HelperUtil::numberTrimmed(floatPart, false);

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


bool checkStringFloat(const string& text)
{
    string err;
    FloatConverter cvt;
    auto b = false;

    // string strFloatNumber = "0.05f";
    // string strFloatNumber = "0.36f";
    string strFloatNumber = text;
    b = cvt.isValidFloat(strFloatNumber,err);
    string floatOrDoubleString = !cvt.getCvt().fFlag.second.empty() ? "float" : "double";

    if ( !b ) {
        cout << strFloatNumber << "| " << err << " | Sorry : is <Not> a Valid Float-Point number" << endl;
        return false;
    } else {
        cout << strFloatNumber << "   is a <Valid> " << floatOrDoubleString << " number" << endl;
        b = cvt.doConvert(err);
        if ( !b ) {
            cout << strFloatNumber << " | " << err << " | Sorry doConvert(...)  Failed  " << endl;
            return false;
        } else {
            const auto& coreObj = cvt.getCvt();

            if ( !coreObj.fFlag.second.empty()  ) {
                // float type
                intFloatNum uNum;
                uNum.f_num = static_cast<float>( atof(strFloatNumber.c_str() ));

                if ( uNum.i_num == coreObj.cvtIntBinary ) {
                    cout << strFloatNumber << " Convert OK " << endl;
                } else {
                    cout << strFloatNumber << " | Sorry : Convert <Not> Equal  " << endl;
                    cout << "uNum.i_num           = 0x" << std::hex << uNum.i_num << endl;
                    cout << "coreObj.cvtIntBinary = 0x" << std::hex << coreObj.cvtIntBinary << endl;
                    return false;
                }
            } else {
                // double type
                llDoubleNum llNum;
                llNum.f_num = atof(strFloatNumber.c_str() );

                if ( llNum.i_num == coreObj.cvtLLBinary ) {
                    cout << strFloatNumber << " Convert OK " << endl;
                } else {
                    cout << strFloatNumber << " | Sorry : Convert <Not> Equal  " << endl;
                    cout << "llNum.i_num           = 0x" << std::hex << llNum.i_num << endl;
                    cout << "coreObj.cvtLLBinary   = 0x" << std::hex << coreObj.cvtLLBinary << endl;
                    return false;
                }
            }

        }
    }

    return true;
}

void readFloatByGenerate(int intBits,int floatBits, bool neFlag, bool fFlag)
{
    unsigned long long maxNum = static_cast<unsigned long long>( pow(10, intBits+floatBits) );
    for ( unsigned long long i = 0; i < maxNum; ++i ) {
        auto strRet = generateStrFloat(i, intBits, floatBits, neFlag, fFlag);
        auto b = checkStringFloat(strRet);
        if ( !b ) {
            cout << "[ERROR] : convert float/double on string \"" << strRet << "\"" << endl;
            break;
        }
    }
}


void testOnly()
{
    string err;
    FloatConverter cvt;
    auto b = false;
    string checkFloatStr = "0.01f";

    b = cvt.isValidFloat(checkFloatStr,err);
    if ( !b ) {
        cout << checkFloatStr << "| " << err << " | Sorry : is <Not> a Valid Float-Point number" << endl;
        return;
    } else {
        b = cvt.doConvert(err);
        if ( !b ) {
            cout << "0.01f" << " | " << err << " | Sorry doConvert(...)  Failed  " << endl;
            return;
        } else {
            const auto& coreObj = cvt.getCvt();

            if ( !coreObj.fFlag.second.empty()  ) {
                // float type
                intFloatNum uNum;
                /********************************
                   replace the hard-code number here
                ********************************/
                uNum.f_num = 0.01f;

                if ( uNum.i_num == coreObj.cvtIntBinary ) {
                    cout << checkFloatStr << " Convert OK " << endl;
                } else {
                    cout << checkFloatStr  << " | Sorry : Convert <Not> Equal  " << endl;
                    cout << "uNum.i_num           = 0x" << std::hex << uNum.i_num << endl;
                    cout << "coreObj.cvtIntBinary = 0x" << std::hex << coreObj.cvtIntBinary << endl;
                    return;
                }
            } else {
                // double type
                llDoubleNum llNum;
                /********************************
                   replace the hard-code number here
                ********************************/
                llNum.f_num = 0.01;

                if ( llNum.i_num == coreObj.cvtLLBinary ) {
                    cout << checkFloatStr << " Convert OK " << endl;
                } else {
                    cout << checkFloatStr << " | Sorry : Convert <Not> Equal  " << endl;
                    cout << "llNum.i_num           = 0x" << std::hex << llNum.i_num << endl;
                    cout << "coreObj.cvtLLBinary   = 0x" << std::hex << coreObj.cvtLLBinary << endl;
                    return;
                }
            }
        }
    }

}


int main(int argc, char* argv[],char* env[])
{
    auto testFlag = 0;
    if ( testFlag == 1 ) {
        testOnly();
        return 0;
    }


    if ( argc != 5 ) {
        cout << "Missing generate args " << endl;
        cout << "          int part   float part     negative       f|F flag"  << endl;
        cout << "./main   <intBits>   <floatBits>   <[ne|other]>  <[f|other]>" << endl;
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
    auto neFlag = (negativeStr == "ne");    // negative flag
    auto fFlag  = (fStr == "f");            // f|F   flag

    readFloatByGenerate(intBits, floatBits, neFlag, fFlag);

    return 0;
}


