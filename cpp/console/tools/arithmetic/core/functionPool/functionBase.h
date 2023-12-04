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

    /****************************************************************************************************
	 
     e.g.
           addFunc(  aPrice * aCount ,  bPrice * bCount);

			addFunc(int a,int b) 
			{ 
				return  a + b; 
			}

	   =>  Total-Price = (aPrice * aCount) + (bPrice * bCount)

			                  |
			                  |
		First time push list  |  Second time push list
		      0. aPrice       |         0. bPrice
			  1. *            |         1. *
			  2. aCount       |         2. bCount
			                  |

    *****************************************************************************************************/
	void pushOneArgumentExpr( const std::list<TokenBase*>& oneArgumentExpr );

    TokenBase* executeFunction( );
    bool       preCheckArgCount(bool needThrow);

protected:
    virtual TokenBase* doCall() = 0;
    virtual int getRequiredArgumentsCount() = 0;

protected:
    std::string m_funcName;
	std::list< std::list<TokenBase*> > m_argumentList;
};

#endif