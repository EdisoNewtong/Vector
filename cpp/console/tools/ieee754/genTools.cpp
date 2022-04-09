/* Usage :
 *   $ ./generateTestCase   
 *		  --datatype <float/double>  
 *		  --intBit     <number>        // a number >=1
 *		  --floatBit   <number>        // a number >=1
 *		  (--negativeSign)
*/



#include <iostream>
#include <iomanip>
#include <fstream>
#include <list>
#include <string>
#include <utility>
#include <stdexcept>
#include <exception>
#include <sstream>
#include <cmath> // for power(...) use
using namespace std;

#include "floatNumberConverter.h"

#define FLOAT2String(num) #num 
#define MAKE_STRING_FLOAT_PAIR(num) FLOAT2String(num) , num

namespace CONST_TEMPLATE 
{
	// Common Part
	const string HEADER =
R"(/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	AUTO GENERATE SOURCE CODE , NEVER MANUALLY MODIFIED THIS FILE 

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
#include <iostream>
#include <string>
#include <utility>
using namespace std;

#include "floatNumberConverter.h"

#define FLOAT2String(num) #num 
#define MAKE_STRING_FLOAT_PAIR(num) FLOAT2String(num) , num

)";

	const string MAIN_HEADER = 
R"(
int main( int argc, char* argv[], char* env[] )
{
)";

	const string MAIN_END = 
R"(

	return 0;
})";


	namespace FLOAT_PART
	{
		const string UNION_DEC =
R"(
union FloatIntUnion 
{
	float f_num;
	unsigned int ui_num;
};

)";
		const string UNION_TYPE = "FloatIntUnion ";
		const string PAIR_DEC = "using stringfloatpair  = pair<string,float>;";
		const string PR_TYPE = "stringfloatpair ";
		const string MEMBER_NAME_0 = "ui_num";
		const string MEMBER_NAME_1 = "f_num";
		const string MEMBER_NAME_2 = "cvtIntBinary";
	}




	namespace DOUBLE_PART
	{
		const string UNION_DEC =
R"(
union DoubleIntUnion 
{
	double d_num;
	unsigned long long ul_num;
};

)";
		const string UNION_TYPE = "DoubleIntUnion ";
		const string PAIR_DEC = "using stringdoublepair = pair<string,double>;";
		const string PR_TYPE = "stringdoublepair ";
		const string MEMBER_NAME_0 = "ul_num";
		const string MEMBER_NAME_1 = "d_num";
		const string MEMBER_NAME_2 = "cvtLLBinary";
	}
}
// 7 ~ 8 arguments


using namespace CONST_TEMPLATE;

void printUsage()
{
	const string helpContext = R"(
Command Line Usage : 
======================================================================
    $ ./genTools
 		  --datatype <float/double>  
 		  --intBit     <number>        // a number >=1
 		  --floatBit   <number>        // a number >=1
 		  (--negativeSign)
======================================================================
)";
	cout << helpContext;

}




bool getRequiredArgs(int argc, char* argv[], bool &isFloat, int &iIntPartBit, int &iFloatPartBit, bool &bHasNegative, string &error)
{
	bool ret = false;
	if ( argc < 7 ) {
		// Error
		error = "Arguments less than 7";
	} else if ( argc > 8 ) {
		// Error
		error = "Arguments greater than 8";
	} else {
		// argc is in range [7,8]
		const string Sarg1("--datatype");
		const string Sarg3("--intBit");
		const string Sarg5("--floatBit");
		const string Sarg7("--negativeSign");
		if (   string(argv[1]) == Sarg1
			&& string(argv[3]) == Sarg3
			&& string(argv[5]) == Sarg5 ) 
		{
			bool parse1OK = true;
			// float / double
			if ( string(argv[2]) == "float" || string(argv[2]) == "double" ) {
				isFloat =  true;
			} else if ( string(argv[2]) == "double" ) {
				isFloat =  false;
			} else {
				error = "type must either float or double";
				parse1OK = false;
			}

			bool parseIntPart = false;
			bool parseFloatPart = false;
			if ( parse1OK  ) {
				try {
					iIntPartBit = std::stoi( string(argv[4]) );
					parseIntPart = true;
				} catch ( const invalid_argument& /*invError*/ ) {
					error = string(argv[4]) + " is invalid int number";
				} catch (const out_of_range& /*overflow*/ ) {
					error = string(argv[4]) + " is out of range";
				}	

				try {
					iFloatPartBit = std::stoi( string(argv[6]));
					parseFloatPart = true;
				} catch ( const std::invalid_argument& /* invError */ ) {
					error = string(argv[6]) + " is invalid int number";
				} catch (const out_of_range& /* overflow */ ) {
					error = string(argv[6]) + " is out of range";
				}	

				if ( parseIntPart && parseFloatPart ) {
					if ( argc == 8 ) {
						if( string(argv[7]) == Sarg7 ) {
							bHasNegative = true;

							ret = true;
						} else {
							error = string("unknown argument ") + string(argv[7]);
						}	
					} else {
						bHasNegative = false;

						ret = true;
					}
				}
			}
		} else {
			error = "Argument prefix is unknown";
		}	
	} 
	
	return ret;
}

void collectPlacehodeString(const string& originalStr, list< pair<string,int> >* sepList)
{
	if( sepList == nullptr ) {
		return;
	}
	
	string::size_type begpos = 0;
	string::size_type pos = originalStr.find("%",begpos);
	while ( pos != string::npos ) {
		string prestr = originalStr.substr( begpos, pos - begpos );
		sepList->push_back( std::make_pair(prestr,0) );
		begpos = pos + 2;
		sepList->push_back( std::make_pair("??",1) );
		pos = originalStr.find("%",begpos);
	}

	if ( begpos < originalStr.size() ) {
		string tailstr = originalStr.substr(begpos);
		sepList->push_back( std::make_pair(tailstr,0) );
	}
}

string genNumberAry(bool isFloat, int iIntPartBit, int iFloatPartBit, bool bHasNegative)
{
	auto totalBit = iIntPartBit + iFloatPartBit;
	int  totalCnt = static_cast<int>( pow(10.0f,totalBit) );
	// if ( bHasNegative ) {
	// 	totalCnt *= 2;
	// 	--totalCnt;   // 0.0    But No   -0.0 , Because they are equal
	// }

	string finalret;
	int idx = 0;
	// Gen Positive Array
	// make_pair( MAKE_STRING_FLOAT_PAIR( 1.1f) ),
	for ( int i = 0; i < totalCnt; ++i, ++idx ) {
		stringstream ss;
		stringstream sint;
		stringstream sFloat;
		stringstream ssfinalfloat;
		string strnum;
		ss << setw(totalBit) << setfill('0') << i;
		strnum = ss.str();
		// Debug log
		// cout << i << ". " << strnum << endl;

		string strIntPart   = strnum.substr(0, iIntPartBit);
		string finalIntPart;
		string strFloatPart = strnum.substr(iIntPartBit);
		string finalFloatPart;

		auto posInt = strIntPart.find_first_not_of('0');
		if ( posInt == string::npos ) {
			// all bit are '0'
			sint << "0";
		} else {
			strIntPart = strIntPart.substr(posInt);
			sint << strIntPart;
		}
		finalIntPart = sint.str();

		auto posFloat = strFloatPart.find_last_not_of('0');
		if ( posFloat == string::npos ) {
			// all bit are '0'
			sFloat << "0";
		} else {
			strFloatPart = strFloatPart.substr(0, posFloat+1);
			sFloat << strFloatPart;
		}
		finalFloatPart = sFloat.str();

		string strfinalfloat =  finalIntPart + "." + finalFloatPart + (isFloat ? "f" : "");
		ssfinalfloat << setw( totalBit + 1 + (isFloat ? 1 : 0) ) << setfill(' ') << std::left << strfinalfloat;

		string line; 
		line += ("\t\tmake_pair( MAKE_STRING_FLOAT_PAIR(" + ssfinalfloat.str()  + ") ), // " + to_string(idx+1) + "\n");
		finalret += line;
	}

	// Gen Negative Array
	if ( bHasNegative ) {
		for ( int i = 0; i < totalCnt; ++i ) {
			if ( i == 0 ) {
				// skip   -0.0
				continue;
			}

			stringstream ss;
			stringstream sint;
			stringstream sFloat;
			stringstream ssfinalfloat;
			string strnum;
			ss << setw(totalBit) << setfill('0') << i;
			strnum = ss.str();
			// Debug log
			// cout << i << ". " << strnum << endl;

			string strIntPart   = strnum.substr(0, iIntPartBit);
			string finalIntPart;
			string strFloatPart = strnum.substr(iIntPartBit);
			string finalFloatPart;

			auto posInt = strIntPart.find_first_not_of('0');
			if ( posInt == string::npos ) {
				// all bit are '0'
				sint << "0";
			} else {
				strIntPart = strIntPart.substr(posInt);
				sint << strIntPart;
			}
			finalIntPart = sint.str();

			auto posFloat = strFloatPart.find_last_not_of('0');
			if ( posFloat == string::npos ) {
				// all bit are '0'
				sFloat << "0";
			} else {
				strFloatPart = strFloatPart.substr(0, posFloat+1);
				sFloat << strFloatPart;
			}
			finalFloatPart = sFloat.str();

			string strfinalfloat =  finalIntPart + "." + finalFloatPart + (isFloat ? "f" : "");
			ssfinalfloat << setw( totalBit + 1 + (isFloat ? 1 : 0) ) << setfill(' ') << std::left << strfinalfloat;

			string line; 
			line += ("\t\tmake_pair( MAKE_STRING_FLOAT_PAIR(-" + ssfinalfloat.str()  + ") ), // " + to_string(idx+1) + "\n");
			finalret += line;
			++idx;
		}

	}

	return finalret;
}


bool genCase(bool isFloat, int iIntPartBit, int iFloatPartBit, bool bHasNegative, string &error)
{
	if ( iIntPartBit <= 0 ){
		error = "[ERROR] : intPartBit must be >= 1";
		return false;
	}

	if ( iFloatPartBit <= 0 ) {
		error = "[ERROR] : floatPartBit must be >= 1";
		return false;
	}

	string sTemplateCode = R"abc(
	%0 ary[] = {
%1
	};

	for ( size_t i = 0; i < sizeof(ary)/sizeof(ary[0]); ++i ) {
		string strfloat = ary[i].first;

		%2 fui;
		fui.%3 = ary[i].second; // auto convert from   fui.f_num  =>   fui.ui_num by union

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
				if ( cvtRet.%4 == fui.%5 ) {
					cout << "[SUCCESSFUL] , Convert Equal : "   <<  " , number = " <<  strfloat <<  " = 0x" << std::hex << fui.%6 << endl;
				} else {
					cout << "[FAILED] , Convert <Not> Equal ! "       <<  endl
						 << "number = \""   << strfloat  << "\"" << endl
						 << "       = 0x" << std::hex << fui.%7    << " ( Union   Convert )" << endl
						 << "      != 0x" << std::hex << cvtRet.%8 << " ( Program Convert )" << endl;
				}
			}
		}
	}
)abc";


	list< pair<string,int> > sepList;
	collectPlacehodeString(sTemplateCode,&sepList); 

	const string fixedfilename("genCode.cpp");
	ofstream f(fixedfilename.c_str() , ios::out | ios::trunc);
	f.write(HEADER.c_str(), HEADER.size() );
	if ( isFloat ) {
		f.write( FLOAT_PART::UNION_DEC.c_str(), FLOAT_PART::UNION_DEC.size() );
		f.write( FLOAT_PART::PAIR_DEC.c_str(),  FLOAT_PART::PAIR_DEC.size() );
		f.write( "\n\n", 2);
	} else {
		f.write( DOUBLE_PART::UNION_DEC.c_str(), DOUBLE_PART::UNION_DEC.size() );
		f.write( DOUBLE_PART::PAIR_DEC.c_str(),  DOUBLE_PART::PAIR_DEC.size() );
		f.write( "\n\n", 2);
	}
	f.write( MAIN_HEADER.c_str(), MAIN_HEADER.size() );
	
	int placeholdpartIdx = 0;
	for( const auto& pr : sepList ) 
	{
		const string fixpart = pr.first;
		auto flag = pr.second;
		if ( flag == 0 ) {
			// fixed string
			f.write(fixpart.c_str(), fixpart.size() );
		} else {
			// flag == 0 , placehode part
			string splacehold;
			switch ( placeholdpartIdx ) 
			{
			case 0:
				// FLOAT_PART::PR_TYPE || DOUBLE_PART::PR_TYPE
				splacehold = isFloat ? FLOAT_PART::PR_TYPE : DOUBLE_PART::PR_TYPE;
				break;
			case 1:
				splacehold = genNumberAry(isFloat, iIntPartBit, iFloatPartBit, bHasNegative);
				break;
			case 2:
				splacehold = isFloat ? FLOAT_PART::UNION_TYPE : DOUBLE_PART::UNION_TYPE;
				break;
			case 3:
				splacehold = isFloat ? FLOAT_PART::MEMBER_NAME_1 : DOUBLE_PART::MEMBER_NAME_1;
				break;
			case 4:
				splacehold = isFloat ? FLOAT_PART::MEMBER_NAME_2 : DOUBLE_PART::MEMBER_NAME_2;
				break;
			case 5:
				splacehold = isFloat ? FLOAT_PART::MEMBER_NAME_0 : DOUBLE_PART::MEMBER_NAME_0;
				break;
			case 6:
				splacehold = isFloat ? FLOAT_PART::MEMBER_NAME_0 : DOUBLE_PART::MEMBER_NAME_0;
				break;
			case 7:
				splacehold = isFloat ? FLOAT_PART::MEMBER_NAME_1 : DOUBLE_PART::MEMBER_NAME_1;
				break;
			case 8:
				splacehold = isFloat ? FLOAT_PART::MEMBER_NAME_2 : DOUBLE_PART::MEMBER_NAME_2;
				break;
			default:
				break;
			}

			if ( !splacehold.empty() ) {
				f.write( splacehold.c_str(), splacehold.size() );
			}
			++placeholdpartIdx;
		}
	}

	f.write(MAIN_END.c_str(), MAIN_END.size() );
	f.flush();
	f.close();
	cout << "[DONE] : Successful write code into file : " << fixedfilename << endl;
	return true;
}



int main(int argc, char* argv[], char* env[])
{
	bool isFloat = false;
	int intPartBit = 0;
	int floatPartBit = 0;
	bool isNegative = false;
	string errorMsg;
	if ( !getRequiredArgs( argc, argv, isFloat, intPartBit, floatPartBit, isNegative, errorMsg ) ) {
		cout << "[ERROR] Parse Arguments error : " << errorMsg << endl;
		printUsage();
		return -1;
	}

	int a = 1;
	if ( a ) {
		cout << "argc = " << argc << endl;
		cout << "\t" << "isFloat = " << isFloat << endl;
		cout << "\t" << "intPartBit = " << intPartBit << endl;
		cout << "\t" << "floatPartBit = " << floatPartBit << endl;
		cout << "\t" << "isNegative = " << isNegative << endl;
	}


	if( !genCase(isFloat, intPartBit, floatPartBit, isNegative, errorMsg) ) {
		cout << "[ERROR] : " << errorMsg << endl;
		return -1;
	}

	cout << "[DONE] : Write Successful ! " << endl;

	return 0;
}


