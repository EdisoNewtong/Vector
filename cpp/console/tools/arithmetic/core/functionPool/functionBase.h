#ifndef FUNCTION_BASE_H
#define FUNCTION_BASE_H

class TokenBase;

#include <string>
#include <list>


class FunctionBase
{
public:
    FunctionBase();
    virtual ~FunctionBase();

	void pushOneArgumentExpr( const std::list<TokenBase*>& oneArgumentExpr );

    TokenBase* executeFunction( );
    bool       preCheckArgCount();

protected:
    virtual TokenBase* doCall() = 0;
    virtual int getRequiredArgumentsCount() = 0;

protected:
    std::string m_funcName;
	std::list< std::list<TokenBase*> > m_argumentList;
};

#endif
