#include <iostream>
#include <string>
#include <utility>
#include <cstdint>
#include <exception>
#include <iomanip>
using namespace std;

#include "floatNumberConverter.h"


union FloatIntUnion 
{
	float f_num;
	uint32_t ui_num;
};

union DoubleIntUnion 
{
	double d_num;
	unsigned long long dull_num;
};


int main( int argc, char* argv[], char* env[] )
{
    if ( argc == 1 ) {
        cout << 
R"([ERROR] : missing float number argument"
   Usage :
          ./<ProgramName>  <floatnumber>[f|F]
)"
        << endl;
        return -1;
    } else if ( argc != 2 ) {
        cout <<
R"([ERROR] : invalid command-line format"
   Usage :
          ./<ProgramName>  <floatnumber>[f|F]
)"
        << endl;
        return -1;
    }


    //
    // argc == 2
    //
    string strfloat = argv[1];
    char lastCh = strfloat.back();
    auto isLastfF = (lastCh == 'f' || lastCh == 'F');

    FloatIntUnion fui;
    DoubleIntUnion dui;
    auto convertSuccessful = false;

    try {
        if ( isLastfF ) {
            fui.f_num = stof(strfloat);
        } else {
            dui.d_num = stod(strfloat);
        }
        convertSuccessful = true;
    } catch ( const std::exception& e ) {
        cout << "[ERROR] : " << e.what() << endl;
    } catch ( ... ) {
        cout << "[ERROR] : catch an unexpected  exception , maybe convert from string -->  float/double failed " << endl;
    }

    if( !convertSuccessful ) {
        if ( isLastfF ) {
            fui.f_num = 0.0f;
        } else {
            dui.d_num = 0.0;
        }
        cout << "[ERROR] : stof(argv[1]) or stod(argv[1])  failed , exit" << endl;
        return -1;
    }


    try {
        FloatConverter cvt;
        string errorMsg;
        auto bret = cvt.isValidFloat( strfloat, errorMsg);
        if ( !bret ) {
            cout << "[ERROR] , Parse String \"" << strfloat << "\" as Float Failed : " << errorMsg << endl;
        } else {
            bret = cvt.doConvert(errorMsg);
            const auto& cvtRet = cvt.getCvt();
            if ( !bret ) {
                cout << "[ERROR] , Convert String as Float Failed : " << errorMsg << endl;
            } else {
                auto bConvertEqualFlag = false;
                if ( isLastfF ) {
                    bConvertEqualFlag = (cvtRet.cvtIntBinary == fui.ui_num);
                } else {
                    bConvertEqualFlag = (cvtRet.cvtLLBinary == dui.dull_num);
                }

                if ( bConvertEqualFlag ) {
                    if ( isLastfF ) {
                        cout << "[SUCCESSFUL] , Convert Equal. "   <<  "Number = " <<  strfloat <<  " = 0x" << std::hex << std::uppercase << fui.ui_num << " => 32 bits , 1 integer number " << endl;
                    } else {
                        cout << "[SUCCESSFUL] , Convert Equal. "   <<  "Number = " <<  strfloat <<  " = 0x" << std::hex << std::uppercase << dui.dull_num << " => 64 bits , 1 unsigned long long " << endl;
                    }
                } else {
                    cout << "[FAILED] , Convert <Not> Equal ! "       <<  endl
                         << "number = \""   << strfloat  << "\"" << endl
                         << "       = 0x" << std::hex << fui.ui_num    << " ( Union   Convert )" << endl
                         << "      != 0x" << std::hex << cvtRet.cvtIntBinary << " ( Program Convert )" << endl;
                }
            }
        }
    } catch ( const std::exception& e ) {
        cout << "[ERROR] : " << e.what() << endl;
    } catch ( ... ) {
        cout << "[ERROR] : catch an unexpected  exception , maybe convert from string -->  float/double failed " << endl;
    }

	return 0;
}

