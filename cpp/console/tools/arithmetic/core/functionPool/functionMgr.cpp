#include "functionMgr.h"
#include "MyAbs.h"
#include "MyFmod.h"
#include "MyRemainder.h"
#include "MyFmax.h"
#include "MyFmin.h"

using namespace std;

// staitc  , set std-call call convention  as default
bool    FunctionMgr::s_callingConvention_StdCall { true };


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

    // fma(x, y),  max of (x,y)
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
    //                      10
    "log",

    // log2( arg )    =>  log (arg)
    //                       2
    "log2",

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
    if ( funcName == "abs" ) {
        fObject = new My_abs();
    } else if ( funcName == "fmod" ) {
		fObject = new My_fmod();
    } else if ( funcName == "remainder" ) {
		fObject = new My_remainder();
    } else if ( funcName == "fmax" ) {
		fObject = new My_fmax();
    } else if ( funcName == "fmin" ) {
		fObject = new My_fmin();
    }

    return fObject;
}