#include "functionMgr.h"


#include "MyAbs.h"
#include "MyFmod.h"
#include "MyFmax.h"
#include "MyFmin.h"

#include "MyExp.h"
#include "MyExp2.h"
#include "MyLog.h"
#include "MyLog2.h"
#include "MyLog10.h"

#include "MyPow.h"
#include "MySqrt.h"
#include "MyCbrt.h"
#include "MyHypot.h"

#include "MySin.h"
#include "MyCos.h"
#include "MyTan.h"
#include "MyAsin.h"
#include "MyAcos.h"
#include "MyAtan.h"
#include "MyAtan2.h"

#include "MyCeil.h"
#include "MyFloor.h"
#include "MyTrunc.h"
#include "MyRound.h"

using namespace std;

// staitc  , set std-call call convention  as default
// bool    FunctionMgr::s_callingConvention_StdCall { true };
// bool    FunctionMgr::isUseStdCallConvension() { return FunctionMgr::s_callingConvention_StdCall; }
// void    FunctionMgr::setStdCallConvension(bool bIsStdCallConvension) { FunctionMgr::s_callingConvention_StdCall = bIsStdCallConvension; }


// static     
unordered_set<string> FunctionMgr::s_functionNameList{
//--------------------------------
//       Basic operations   
//--------------------------------
    // abs( x )
    "abs",

    // fmod( x, y)
    "fmod",

    // remainder( x, y)
    "remainder",

    // fmax(x, y),  max of (x,y)
    "fmax",

    // fmin(x, y),  min of (x,y)
    "fmin",

//--------------------------------
//       Exponential functions
//--------------------------------
    // exp( arg )   =>  e^(arg)
    "exp",

    // exp2( arg )   =>  2^(arg)
    "exp2",

    // log( arg )    =>  log  (arg)
    //                       e 
    "log",

    // log2( arg )    =>  log (arg)
    //                       2
    "log2",

    // log10( arg )    =>  log   (arg)
    //                        10
    "log10",

//--------------------------------
// Power functions
//--------------------------------
    // pow(base, exp)   =>   base^(exp)
    "pow",

    // sqrt(arg)   =>   sqrt(169) =>  13
    "sqrt",

    // cbrt(arg)   =>   cbrt(125) =>  5   ( 5*5*5 = 125 )
    "cbrt",

    // hypot( x, y)    =>    sqrt( x*x + y*y ) 
    "hypot",


//--------------------------------
// Trigonometric functions
//--------------------------------
    "sin",

    "cos",

    "tan",

    "asin",

    "acos",

    "atan",

    // atan2(x,y)   =>   arctan( y / x )
    "atan2",

// --------------------------------------------------
// Nearest integer floating point operations
//--------------------------------------------------
    // ceil(3.2)  =>  4  , ceil(-3.2) => -3
    "ceil",

    // floor(3.2)  =>  3  , floor(-3.2) => -4
    "floor",

    // trunc(3.2)  =>  3  , floor(-3.2) => -3
    "trunc",

/****************************************************************************************************

    round(+2.3) = 2  round(+2.5) = 3  round(+2.7) = 3
    round(-2.3) = -2  round(-2.5) = -3  round(-2.7) = -3
    round(-0.0) = -0
    round(-Inf) = -inf

****************************************************************************************************/
    "round",


};

// static 
list<FunctionBase*>   FunctionMgr::s_allocedFunctionTokenList{};


// static 
void FunctionMgr::init()
{
    s_allocedFunctionTokenList.clear();
}

// static 
void FunctionMgr::finalize()
{
    for( auto func : s_allocedFunctionTokenList ) {
        if ( func != nullptr ) {
            delete func;
        }
    }

    s_allocedFunctionTokenList.clear();
}



// static 
bool FunctionMgr::isInsideFunctionList(const string& funcName)
{
    return s_functionNameList.find(funcName) != s_functionNameList.end();
}



// static 
FunctionBase* FunctionMgr::generateFunctionObjectByName(const string& funcName)
{
	FunctionBase* fObject = nullptr;

	//////////////////////////////////////////////////////////////////////
	// Basic    abs / fmod / remainder / fmax / fmin
	//////////////////////////////////////////////////////////////////////
    if ( funcName == "abs" ) {
        fObject = new My_abs();
    } else if ( funcName == "fmod" ) {
		fObject = new My_fmod();
    } else if ( funcName == "fmax" ) {
		fObject = new My_fmax();
    } else if ( funcName == "fmin" ) {
		fObject = new My_fmin();
    } 
	//////////////////////////////////////////////////////////////////////
	// explog    exp / log
	//////////////////////////////////////////////////////////////////////
	else if ( funcName == "exp" ) {
		fObject = new My_exp();
    } else if ( funcName == "exp2" ) {
		fObject = new My_exp2();
    } else if ( funcName == "log" ) {
		fObject = new My_log();
    } else if ( funcName == "log2" ) {
		fObject = new My_log2();
    } else if ( funcName == "log10" ) {
		fObject = new My_log10();
	} 
	//////////////////////////////////////////////////////////////////////
	// pow / sqrt / cbrt
	//////////////////////////////////////////////////////////////////////
	else if ( funcName == "pow" ) {
		fObject = new My_pow();
	} else if ( funcName == "sqrt" ) {
		fObject = new My_sqrt();
	} else if ( funcName == "cbrt" ) {
		fObject = new My_cbrt();
	} else if ( funcName == "hypot" ) {
		fObject = new My_hypot();
	} 
	//////////////////////////////////////////////////////////////////////
	// sin / cos / tan / asin / acos / atan / atan2
	//////////////////////////////////////////////////////////////////////
	else if ( funcName == "sin" ) {
		fObject = new My_sin();
	} else if ( funcName == "cos" ) {
		fObject = new My_cos();
	} else if ( funcName == "tan" ) {
		fObject = new My_tan();
	} else if ( funcName == "asin" ) {
		fObject = new My_asin();
	} else if ( funcName == "acos" ) {
		fObject = new My_acos();
	} else if ( funcName == "atan" ) {
		fObject = new My_atan();
	} else if ( funcName == "atan2" ) {
		fObject = new My_atan2();
	}
	//////////////////////////////////////////////////////////////////////
	// ceil / floor / trunc / round
	//////////////////////////////////////////////////////////////////////
	else if ( funcName == "ceil" ) {
		fObject = new My_ceil();
	} else if ( funcName == "floor" ) {
		fObject = new My_floor();
	} else if ( funcName == "trunc" ) {
		fObject = new My_trunc();
	} else if ( funcName == "round" ) {
		fObject = new My_round();
	}

    s_allocedFunctionTokenList.push_back( fObject );

    return fObject;
}
