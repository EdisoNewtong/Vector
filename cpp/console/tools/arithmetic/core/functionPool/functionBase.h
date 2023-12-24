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

    void       setBeginParenthesis(TokenBase* funcName, TokenBase* pOpenParenthesis);
    void       setCloseParenthesisValidFlag(TokenBase* pEndParenthesis);

	bool       isFunctionEndupValid();

    static std::string   getSuffixExpString( const std::list<TokenBase*>& oneArgumentExpr);
    std::string   buildFuncitonDetailString(const std::list<std::string>& strExpList);
protected:
    virtual TokenBase* doCall() = 0;
    virtual int getRequiredArgumentsCount() = 0;

protected:
    std::string m_funcName;
	std::list< std::list<TokenBase*> > m_argumentList;
	std::list<TokenBase*>              m_argResultList;
	std::list<std::string>             m_argStrSuffixExprList;

    TokenBase* m_pFuncToken;
    TokenBase* m_pOpenParenthesis;
    TokenBase* m_pCloseParenthesis;


};

#endif
