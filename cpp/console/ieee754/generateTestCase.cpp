/* Usage :
 *   $ ./generateTestCase   
 *		  --datetype <float/double>  
 *		  --intBit     <number>        // a number >=1
 *		  --floatBit   <number>        // a number >=1
 *		  (--negativeSign)
*/



#include <iostream>
#include <string>
#include <utility>
using namespace std;

#include "floatNumberConverter.h"

#define FLOAT2String(num) #num 
#define MAKE_STRING_FLOAT_PAIR(num) FLOAT2String(num) , num

namespace CONST_TEMPLATE 
{
	// Common Part
	const string HEADER =
R"(
/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	AUTO GENERATE SOURCE CODE , NEVER MAUNUAL MODIFIED THIS FILE 

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
	}




	namespace DOUBLE_PART
	{
		const string UNION_DEC =
R"(
union DoubleIntUnion 
{
	double d_num;
	unsigned long long ui_num;
};

)";
		const string UNION_TYPE = "DoubleIntUnion ";
		const string PAIR_DEC = "using stringdoublepair = pair<string,double>;"
		const string PR_TYPE = "stringdoublepair ";

	}
}
// 7 ~ 8 arguments


