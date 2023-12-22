#ifndef FUNCTION_MGR_H
#define FUNCTION_MGR_H

#include "functionBase.h"

#include <unordered_set>
#include <list>
#include <string>

class FunctionMgr
{
public:
    static void init();
    static void finalize();

    static bool isInsideFunctionList(const std::string& funcName);

    static FunctionBase* generateFunctionObjectByName(const std::string& funcName);
    static bool          isUseStdCallConvension();
protected:
    static std::unordered_set<std::string> s_functionNameList;

    static std::list<FunctionBase*>        s_allocedFunctionTokenList;

	/***************************************************

	     int a = 1;

	     int add(int n1, int n2) { return n1+n2; }

	    c call  : add( a, a = 10);  =>   n1 <-  1,  n2 = 10 ,  return  1+10; return 11;
	    stdcall : add( a, a = 10);  =>   n1 <- 10,  n2 = 10 ,  return 10+10; return 20;

	****************************************************/
    static bool                            s_callingConvention_StdCall;

private:
    FunctionMgr() = delete;
    virtual ~FunctionMgr() = delete;
};

#endif
