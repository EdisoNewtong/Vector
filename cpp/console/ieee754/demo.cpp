#include <iostream>
#include <string>
#include <utility>
using namespace std;

#include "floatNumberConverter.h"

#define FLOAT2String(num) #num 
#define MAKE_STRING_FLOAT_PAIR(num) FLOAT2String(num) , num

using stringfloatpair  = pair<string,float>;
// using stringdoublepair = pair<string,double>;

union FloatIntUnion 
{
	float f_num;
	unsigned int ui_num;
};

// union DoubleIntUnion 
// {
// 	double d_num;
// 	unsigned long long ui_num;
// };


int main( int argc, char* argv[], char* env[] )
{

	stringfloatpair ary[] = {
		make_pair( MAKE_STRING_FLOAT_PAIR(1.1f) ),
		make_pair( MAKE_STRING_FLOAT_PAIR(0.1f) ),
		make_pair( MAKE_STRING_FLOAT_PAIR(-3.4f) ),
	};

	for ( size_t i = 0; i < sizeof(ary)/sizeof(ary[0]); ++i ) {
		string strfloat = ary[i].first;

		FloatIntUnion fui;
		fui.f_num = ary[i].second; // auto convert from   fui.f_num  =>   fui.ui_num

		//
		// DoubleIntUnion fui;
		// fui.d_num = ary[i].second; // auto convert from   fui.d_num  =>   fui.ui_num

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
				cvt.printConvertDetail();
				if ( cvtRet.cvtIntBinary == fui.ui_num ) {
					cout << "[SUCCESSFUL] , Convert Equal. "   <<  "Number = " <<  strfloat <<  " = 0x" << std::hex << fui.ui_num << endl;
				} else {
					cout << "[FAILED] , Convert <Not> Equal ! "       <<  endl
						 << "number = \""   << strfloat  << "\"" << endl
						 << "       = 0x" << std::hex << fui.ui_num    << " ( Union   Convert )" << endl
						 << "      != 0x" << std::hex << cvtRet.cvtIntBinary << " ( Program Convert )" << endl;
				}
			}
		}
	}

	return 0;
}

